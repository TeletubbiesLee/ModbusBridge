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

#define STRING_LENTH DATA_NAME_LENTH_MAX

int SaveBitsFile(uint8_t *tabBits, ConfigFile *modbusCoil);
int SaveRegistersFile(uint16_t *tabRegisters, ConfigFile *modbusRegister);
int SaveDataInfo(int filefd, ConfigFile *dataInfo);
int Int2String(int number, char *string);
int SaveIntToFile(int fileFd, int number);
int SaveStringToFile(int fileFd, char *string, int stringLenth);
int GetTimeStr(char *timeStr);
int ParseBitsFile(char *fileName, uint8_t *tabBits, int tabNumber, ConfigFile *modbusCoil);
int ReadStringFromFile(int fileFd, char *string, int stringLenth);
int String2Int(char *string, int StringLenth);

#endif
