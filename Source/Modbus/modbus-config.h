/*
 * config.h
 *
 *  Created on: 2019年8月28日
 *      Author: Administrator
 */

#ifndef _MODBUS_CONFIG_H_
#define _MODBUS_CONFIG_H_


/* Constants defined by configure.ac */
#define HAVE_INTTYPES_H @HAVE_INTTYPES_H@
#define HAVE_STDINT_H @HAVE_STDINT_H@

#ifdef HAVE_INTTYPES_H
    #include <inttypes.h>
#endif

#ifdef HAVE_STDINT_H
    #ifndef _MSC_VER
        #include <stdint.h>
    #else
        #include "stdint.h"
    #endif
#endif

#define SERVER_ID           1
#define UART_DEVICE_NAME    "/dev/ttymxc1"
#define UART_BANDRATE       115200
#define UART_PARITY         'N'
#define UART_DATA_BIT       8
#define UART_STOP_BIT       1

#define UT_BITS_NUMBER 				20
#define UT_INPUT_BITS_NUMBER		20
#define UT_REGISTERS_NUMBER			50
#define UT_INPUT_REGISTERS_NUMBER	50


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

enum {
    COIL_BITS = 1,
    DISCRETE_INPUTS,
    HOLDING_REGISTER,
    INPUT_REGISTER
};


#endif /* _MODBUS_CONFIG_H_ */
