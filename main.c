/*
 * Copyright Â© 2008-2014 StÃ©phane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include ".\Modbus\modbus.h"
#include ".\Struct2Json\JsonFileOperation.h"
#include ".\Struct2Json\ConfigFile.h"
#ifdef _WIN32
# include <winsock2.h>
#else
# include <sys/socket.h>
#endif

/* For MinGW */
#ifndef MSG_NOSIGNAL
# define MSG_NOSIGNAL 0
#endif

#include ".\Modbus\unit-test.h"

#define BUG_REPORT(_cond, _format, _args ...) \
    printf("\nLine %d: assertion error for '%s': " _format "\n", __LINE__, # _cond, ## _args);

#define ASSERT_TRUE(_cond, _format, __args...) {  \
            if (_cond) {                                  \
                printf("OK\n");                           \
            } else {                                      \
                BUG_REPORT(_cond, _format, ## __args);    \
                goto close;                               \
            }                                             \
        };


enum {
    TCP,
    TCP_PI,
    RTU
};


int equal_dword(uint16_t *tab_reg, const uint32_t value) {
    return ((tab_reg[0] == (value >> 16)) && (tab_reg[1] == (value & 0xFFFF)));
}



/* client主机 */
int main(int argc, char *argv[])
{
    int useBackend;             //Modbus的类型
    modbus_t *ctx = NULL;       //成功打开设备后返回的结构体指针
    uint8_t *tabBits = NULL;            //bit位的空间
    uint16_t *tabRegisters = NULL;      //寄存器的空间

    int nbPoints;               //空间大小
    int ret = -1;               //返回值
    int i = 0, j = 0;
//    float realFloatValue = 0.0;           //浮点数的实际值

    vPort_s2j_init();
    ConfigFileInit();
    Get_JsonFile();

    /* 判断Modbus的类型 */
    if (argc > 1) {
        if (0 == strcmp(argv[1], "tcp")) {
            useBackend = TCP;
        } else if (0 == strcmp(argv[1], "tcppi")) {
            useBackend = TCP_PI;
        } else if (0 == strcmp(argv[1], "rtu")) {
            useBackend = RTU;
        } else {
            printf("Usage:\n  %s [tcp|tcppi|rtu] - Modbus client for unit testing\n\n", argv[0]);
            exit(1);
        }
    } else {
        /* By default */
        useBackend = RTU;
    }

    /* 根据Modbus_RTU的类型建立连接 */
    if (TCP == useBackend) {
        ctx = modbus_new_tcp("127.0.0.1", 1502);
    } else if (TCP_PI == useBackend) {
        ctx = modbus_new_tcp_pi("::1", "1502");
    } else {
        ctx = modbus_new_rtu("/dev/ttymxc1", 115200, 'N', 8, 1);
    }
    if (NULL == ctx) {
        fprintf(stderr, "Unable to allocate libmodbus context\n");
        return -1;
    }

    modbus_set_debug(ctx, TRUE);        //设置Dubug模式
    modbus_set_error_recovery(ctx, MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL);

    /* 设置从机ID */
    if (RTU == useBackend) {
        modbus_set_slave(ctx, SERVER_ID);
    }

    /* 建立连接 */
    if (-1 == modbus_connect(ctx)) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }
    printf("Connection Successful!\r\n");

    /* 为bit和寄存器分配内存空间 */
    nbPoints = (UT_BITS_NB > UT_INPUT_BITS_NB) ? UT_BITS_NB : UT_INPUT_BITS_NB;
    tabBits = (uint8_t *) malloc(nbPoints * sizeof(uint8_t));
    memset(tabBits, 0, nbPoints * sizeof(uint8_t));

    nbPoints = (UT_REGISTERS_NB > UT_INPUT_REGISTERS_NB) ? UT_REGISTERS_NB : UT_INPUT_REGISTERS_NB;
    tabRegisters = (uint16_t *) malloc(nbPoints * sizeof(uint16_t));
    memset(tabRegisters, 0, nbPoints * sizeof(uint16_t));

    /* TODO:对bit，input bit，holding register，input register的读写命令 */
    while(1)
    {
    	for(j = 0; j < MODBUS_CONFIG_STRUCT_MAX; j++)
    	{
    		if(g_ModbusConfigFile[j].functionCode == 1) {			/** Coil Bits **/
				ret = modbus_read_bits(ctx, g_ModbusConfigFile[j].startAddress,
										g_ModbusConfigFile[j].number, tabBits);

				printf("%s: ", g_ModbusConfigFile[j].dataName);
				for(i = 0; i < ret; i++)
				{
					printf("%d, ", tabBits[i]);
				}
				printf("\n");

			}else if(g_ModbusConfigFile[j].functionCode == 2) {		/** Discrete Inputs **/
				ret = modbus_read_input_bits(ctx, g_ModbusConfigFile[j].startAddress,
												g_ModbusConfigFile[j].number, tabBits);

				printf("%s: ", g_ModbusConfigFile[j].dataName);
				for(i = 0; i < ret; i++)
				{
					printf("%d, ", tabBits[i]);
				}
				printf("\n");

			}else if(g_ModbusConfigFile[j].functionCode == 3) {		/** Holding Registers **/
				ret = modbus_read_registers(ctx, g_ModbusConfigFile[j].startAddress,
											g_ModbusConfigFile[j].number, tabRegisters);

				printf("%s: ", g_ModbusConfigFile[j].dataName);
				for(i = 0; i < ret; i++)
				{
					printf("%d, ", tabRegisters[i]);
				}
				printf("\n");

			}else if(g_ModbusConfigFile[j].functionCode == 4) {
				ret = modbus_read_input_registers(ctx, g_ModbusConfigFile[j].startAddress,
													g_ModbusConfigFile[j].number, tabRegisters);

				printf("%s: ", g_ModbusConfigFile[j].dataName);
				for(i = 0; i < ret; i++)
				{
					printf("%d, ", tabRegisters[i]);
				}
				printf("\n");
			}
    	}

    	sleep(10);
    }


    /* MASKS */
#if 0
    printf("1/1 Write mask: ");
    ret = modbus_write_register(ctx, UT_REGISTERS_ADDRESS, 0x12);
    ret = modbus_mask_write_register(ctx, UT_REGISTERS_ADDRESS, 0xF2, 0x25);
    ASSERT_TRUE(ret != -1, "FAILED (%x == -1)\n", ret);
    ret = modbus_read_registers(ctx, UT_REGISTERS_ADDRESS, 1, tabRegisters);
    ASSERT_TRUE(tabRegisters[0] == 0x17,
                "FAILED (%0X != %0X)\n",
                tabRegisters[0], 0x17);
#endif


    /** FLOAT **/
#if 0
    printf("1/4 Set/get float ABCD: ");
    modbus_set_float_abcd(UT_REAL, tabRegisters);
    ASSERT_TRUE(equal_dword(tabRegisters, UT_IREAL_ABCD), "FAILED Set float ABCD");
    realFloatValue = modbus_get_float_abcd(tabRegisters);
    ASSERT_TRUE(realFloatValue == UT_REAL, "FAILED (%f != %f)\n", realFloatValue, UT_REAL);

    printf("2/4 Set/get float DCBA: ");
    modbus_set_float_dcba(UT_REAL, tabRegisters);
    ASSERT_TRUE(equal_dword(tabRegisters, UT_IREAL_DCBA), "FAILED Set float DCBA");
    realFloatValue = modbus_get_float_dcba(tabRegisters);
    ASSERT_TRUE(realFloatValue == UT_REAL, "FAILED (%f != %f)\n", realFloatValue, UT_REAL);

    printf("3/4 Set/get float BADC: ");
    modbus_set_float_badc(UT_REAL, tabRegisters);
    ASSERT_TRUE(equal_dword(tabRegisters, UT_IREAL_BADC), "FAILED Set float BADC");
    realFloatValue = modbus_get_float_badc(tabRegisters);
    ASSERT_TRUE(realFloatValue == UT_REAL, "FAILED (%f != %f)\n", realFloatValue, UT_REAL);

    printf("4/4 Set/get float CDAB: ");
    modbus_set_float_cdab(UT_REAL, tabRegisters);
    ASSERT_TRUE(equal_dword(tabRegisters, UT_IREAL_CDAB), "FAILED Set float CDAB");
    realFloatValue = modbus_get_float_cdab(tabRegisters);
    ASSERT_TRUE(realFloatValue == UT_REAL, "FAILED (%f != %f)\n", realFloatValue, UT_REAL);
#endif


    /* Free the memory */
    free(tabBits);
    free(tabRegisters);

    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}






































/**
 *               ii.                                         ;9ABH,
 *              SA391,                                    .r9GG35&G
 *              &#ii13Gh;                               i3X31i;:,rB1
 *              iMs,:,i5895,                         .5G91:,:;:s1:8A
 *               33::::,,;5G5,                     ,58Si,,:::,sHX;iH1
 *                Sr.,:;rs13BBX35hh11511h5Shhh5S3GAXS:.,,::,,1AG3i,GG
 *                .G51S511sr;;iiiishS8G89Shsrrsh59S;.,,,,,..5A85Si,h8
 *               :SB9s:,............................,,,.,,,SASh53h,1G.
 *            .r18S;..,,,,,,,,,,,,,,,,,,,,,,,,,,,,,....,,.1H315199,rX,
 *          ;S89s,..,,,,,,,,,,,,,,,,,,,,,,,....,,.......,,,;r1ShS8,;Xi
 *        i55s:.........,,,,,,,,,,,,,,,,.,,,......,.....,,....r9&5.:X1
 *       59;.....,.     .,,,,,,,,,,,...        .............,..:1;.:&s
 *      s8,..;53S5S3s.   .,,,,,,,.,..      i15S5h1:.........,,,..,,:99
 *      93.:39s:rSGB@A;  ..,,,,.....    .SG3hhh9G&BGi..,,,,,,,,,,,,.,83
 *      G5.G8  9#@@@@@X. .,,,,,,.....  iA9,.S&B###@@Mr...,,,,,,,,..,.;Xh
 *      Gs.X8 S@@@@@@@B:..,,,,,,,,,,. rA1 ,A@@@@@@@@@H:........,,,,,,.iX:
 *     ;9. ,8A#@@@@@@#5,.,,,,,,,,,... 9A. 8@@@@@@@@@@M;    ....,,,,,,,,S8
 *     X3    iS8XAHH8s.,,,,,,,,,,...,..58hH@@@@@@@@@Hs       ...,,,,,,,:Gs
 *    r8,        ,,,...,,,,,,,,,,.....  ,h8XABMMHX3r.          .,,,,,,,.rX:
 *   :9, .    .:,..,:;;;::,.,,,,,..          .,,.               ..,,,,,,.59
 *  .Si      ,:.i8HBMMMMMB&5,....                    .            .,,,,,.sMr
 *  SS       :: h@@@@@@@@@@#; .                     ...  .         ..,,,,iM5
 *  91  .    ;:.,1&@@@@@@MXs.                            .          .,,:,:&S
 *  hS ....  .:;,,,i3MMS1;..,..... .  .     ...                     ..,:,.99
 *  ,8; ..... .,:,..,8Ms:;,,,...                                     .,::.83
 *   s&: ....  .sS553B@@HX3s;,.    .,;13h.                            .:::&1
 *    SXr  .  ...;s3G99XA&X88Shss11155hi.                             ,;:h&,
 *     iH8:  . ..   ,;iiii;,::,,,,,.                                 .;irHA
 *      ,8X5;   .     .......                                       ,;iihS8Gi
 *         1831,                                                 .,;irrrrrs&@
 *           ;5A8r.                                            .:;iiiiirrss1H
 *             :X@H3s.......                                .,:;iii;iiiiirsrh
 *              r#h:;,...,,.. .,,:;;;;;:::,...              .:;;;;;;iiiirrss1
 *             ,M8 ..,....,.....,,::::::,,...         .     .,;;;iiiiiirss11h
 *             8B;.,,,,,,,.,.....          .           ..   .:;;;;iirrsss111h
 *            i@5,:::,,,,,,,,.... .                   . .:::;;;;;irrrss111111
 *            9Bi,:,,,,......                        ..r91;;;;;iirrsss1ss1111
 *
 *								狗头保佑，永无BUG！
 */















