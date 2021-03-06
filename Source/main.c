/**
 * @file Main.c
 * @brief main函数程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.02.17
 *
 * @author Lei.L
 * @version ver 1.0
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include ".\Modbus\modbus.h"
#include ".\Struct2Json\JsonFileOperation.h"
#include ".\Struct2Json\ConfigFile.h"
#include "DataHandle.h"
#include ".\Modbus\modbus-config.h"
#include "Config.h"

#ifdef _WIN32
# include <winsock2.h>
#else
# include <sys/socket.h>
#endif

/* For MinGW */
#ifndef MSG_NOSIGNAL
# define MSG_NOSIGNAL 0
#endif


void ModbusPrintf(char* dataName, uint8_t *bitData, unsigned short *registerData, int dataNumber);


/* modbus主机 */
int main(int argc, char *argv[])
{
    modbus_t *ctx = NULL;       //成功打开设备后返回的结构体指针
    uint8_t *tabBits = NULL;            //bit位的空间
    uint16_t *tabRegisters = NULL;      //寄存器的空间

    int nbPoints;               //空间大小
    int ret = -1;               //返回值
    int i = 0, j = 0, tempValue = 0;
    ConfigFile *configTemp = NULL;  //用于临时保存结构体
    int modbusGroupNumber = 0;

    if(argc < 2)
    {
    	printf_debug("error: too few arguments to program \'%s\'!\n", argv[0]);
    	return FEW_ARGUMENTS;
    }
    else
    {
    	modbusGroupNumber = String2Int(argv[1], strlen(argv[1]));
    	if(modbusGroupNumber < 0 || modbusGroupNumber > MODBUS_CONFIG_STRUCT_MAX)
    	{
    		printf_debug("error: arguments \'%s\' is wrong to program \'%s\'!\n", argv[1], argv[0]);
			return ERROR_ARGUMENTS;
    	}
    }

    Struct2JsonInit();
    ConfigFileInit();
    GetJsonFile();

    /* 根据Modbus_RTU的类型建立连接 */
    ctx = modbus_new_rtu(UART_DEVICE_NAME, UART_BANDRATE, UART_PARITY, UART_DATA_BIT, UART_STOP_BIT);
    if(NULL == ctx)
    {
    	printf_debug("Unable to allocate libmodbus context\n");
        return FUNCTION_FAIL;
    }

    modbus_set_debug(ctx, TRUE);        //设置Dubug模式
    modbus_set_error_recovery(ctx, MODBUS_ERROR_RECOVERY_LINK | MODBUS_ERROR_RECOVERY_PROTOCOL);

    /* 设置从机ID */
    modbus_set_slave(ctx, SERVER_ID);

    /* 建立连接 */
    if(-1 == modbus_connect(ctx))
    {
    	printf_debug("Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return FUNCTION_FAIL;
    }
    printf("Connection Successful!\r\n");

    /* 为bit和寄存器分配内存空间 */
    nbPoints = (UT_BITS_NUMBER > UT_INPUT_BITS_NUMBER) ? UT_BITS_NUMBER : UT_INPUT_BITS_NUMBER;
    tabBits = (uint8_t *) malloc(nbPoints * sizeof(uint8_t));
    memset(tabBits, 0, nbPoints * sizeof(uint8_t));

    nbPoints = (UT_REGISTERS_NUMBER > UT_INPUT_REGISTERS_NUMBER) ? UT_REGISTERS_NUMBER : UT_INPUT_REGISTERS_NUMBER;
    tabRegisters = (uint16_t *) malloc(nbPoints * sizeof(uint16_t));
    memset(tabRegisters, 0, nbPoints * sizeof(uint16_t));

    /* 查看是否存在存放CSV数据文件的文件夹，不存在则创建 */
    if(access(CSV_DIR_NAME, F_OK) != 0)
    {
    	if(mkdir(CSV_DIR_NAME, S_IRWXU) == -1)
    	{
    		printf_debug("mkdir %s failed!\n", CSV_DIR_NAME);
    		return FUNCTION_FAIL;
    	}
    }

    /* 对bit，input bit，holding register，input register的读写命令 */
    while(1)
    {
    	for(j = 0; j < modbusGroupNumber; j++)
    	{
            configTemp = &g_ModbusConfigFile[j];
            for(i = 0; i < configTemp->number; i += tempValue)
            {
            	tempValue = ((configTemp->number - i) > 125 ? 125 : configTemp->number - i);
            	printf("tempValue = %d, configTemp->number = %d\n", tempValue, configTemp->number);
            	printf("functionCode = %d, i = %d\n", configTemp->functionCode, i);
				switch(configTemp->functionCode)
				{
					case COIL_BITS:
						ret = modbus_read_bits(ctx, configTemp->startAddress + i, tempValue, &tabBits[i]);
						if(ret != -1 && i + tempValue == configTemp->number)
							SaveBitsFile(tabBits, configTemp);
						break;
					case DISCRETE_INPUTS:
						ret = modbus_read_input_bits(ctx, configTemp->startAddress + i, tempValue, &tabBits[i]);
						if(ret != -1 && i + tempValue == configTemp->number)
							SaveBitsFile(tabBits, configTemp);
						break;
					case HOLDING_REGISTER:
						ret = modbus_read_registers(ctx, configTemp->startAddress + i, tempValue, &tabRegisters[i]);
						if(ret != -1 && i + tempValue == configTemp->number)
							SaveRegistersFile(tabRegisters, configTemp);
						break;
					case INPUT_REGISTER:
						ret = modbus_read_input_registers(ctx, configTemp->startAddress + i, tempValue, &tabRegisters[i]);
						if(ret != -1 && i + tempValue == configTemp->number)
							SaveRegistersFile(tabRegisters, configTemp);
						break;
					default:
						ret = -1;
						break;
				}
				sleep(1);
            }
            configTemp = NULL;

    	}
    	sleep(10);
    }

    /* Free the memory */
    free(tabBits);
    free(tabRegisters);

    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);

    return NO_ERROR;
}


/**
 * @fn ConfigFileInit
 * @brief 配置文件初始化
 * @param dataName 数据的名称
 * @param dataNumber 数据的数量
 * @param bitData 线圈数据的首地址
 * @param registerData 寄存器数据首地址
 * @return void
 */
void ModbusPrintf(char* dataName, uint8_t *bitData, uint16_t *registerData, int dataNumber)
{
    int i = 0;
    printf("%s: \n", dataName);
    if(bitData != NULL)
    {
    	for(i = 0; i < dataNumber; i++)
		{
			printf("%d, ", bitData[i]);
			if((i%10) == 9)
			{
				printf("\n");
			}
		}
    }

    if(registerData != NULL)
	{
		for(i = 0; i < dataNumber; i++)
		{
			printf("%d, ", registerData[i]);
			if((i%10) == 9)
			{
				printf("\n");
			}
		}
	}

    printf("\n");
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















