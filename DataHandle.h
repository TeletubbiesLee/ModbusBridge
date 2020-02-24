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


int SaveBits(uint8_t *tabBits, ConfigFile *modbusCoil);
int SaveRegisters(uint16_t *tabRegisters, ConfigFile *modbusRegister);
int SaveDataInfo(int filefd, ConfigFile *dataInfo);
int Int2Char(int number, char *string);
int SaveIntToFile(int fileFd, int number);
int SaveStringToFile(int fileFd, char *string, int stringLenth);
int GetTimeStr(char *timeStr);

#endif
