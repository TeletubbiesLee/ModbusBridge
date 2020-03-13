/**
 * @file DataHandle.h
 * @brief 数据处理的相关程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company SOJO
 * @date 2020.02.20
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _DATA_HANDLE_H_
#define _DATA_HANDLE_H_

#include "./Struct2Json/ConfigFile.h"

#define STRING_LENTH DATA_NAME_LENTH_MAX
#define IS_BIT 0
#define IS_REGISTER 1

int SaveBitsFile(uint8_t *tabBits, ConfigFile *modbusCoil);
int SaveRegistersFile(uint16_t *tabRegisters, ConfigFile *modbusRegister);
int ParseCSVDataFile(char *fileName, uint8_t *bitData, uint16_t *registerData, int arrayNumber);
int String2Int(char *string, int StringLenth);


#endif
