/*
 * Copyright © 2008-2014 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _UNIT_TEST_H_
#define _UNIT_TEST_H_

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

const uint16_t UT_BITS_ADDRESS = 0x0;
const uint16_t UT_BITS_NB = 0x10;
const uint8_t UT_BITS_TAB[] = {0xA5};

const uint16_t UT_INPUT_BITS_ADDRESS = 0x10;
const uint16_t UT_INPUT_BITS_NB = 0x10;
const uint8_t UT_INPUT_BITS_TAB[] = {0xDB};

const uint16_t UT_REGISTERS_ADDRESS = 0x20;
const uint16_t UT_REGISTERS_NB = 0xA;
const uint16_t UT_REGISTERS_NB_MAX = 0x10;
const uint16_t UT_REGISTERS_TAB[] = {0x022B, 0x0001, 0x0064, 0x4538, 0xAC8E, 0x220B, 0x0F01, 0x6004, 0x4C85, 0xEA83};

const uint16_t UT_INPUT_REGISTERS_ADDRESS = 0x30;
const uint16_t UT_INPUT_REGISTERS_NB = 0x8;
const uint16_t UT_INPUT_REGISTERS_TAB[] = {0x0001, 0x0020, 0x0300, 0x4000, 0x0055, 0x0660, 0x7700, 0x0808};


#endif /* _UNIT_TEST_H_ */
