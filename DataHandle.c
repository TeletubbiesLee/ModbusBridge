/**
 * @file DataHandle.c
 * @brief 数据处理的相关程序文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company SOJO
 * @date 2020.02.20
 *
 * @author Lei.L
 * @version ver 1.0
 */

#include "DataHandle.h"
#include "ConfigFile.h"
#include <fcntl.h>
#include <time.h>


/**
 * @fn SaveCoilBits
 * @brief 保存离散线圈的数据
 * @param tabBits 需要保存的数据
 * @param modbusCoil 数据的相关信息结构体
 * @return 成功:0 错误:-1
 */
int SaveBits(uint8_t *tabBits, ConfigFile *modbusCoil)
{
    int fileFd = 0;         //文件描述符
    ssize_t writeNum = 0;   //写入的数据长度
    int i = 0;
    char fileName[40] = {0};    //文件名

    GetTimeStr(fileName);
    strncat(fileName, "Bits.csv", strlen("Bits.csv"));
    fileFd = open(fileName, O_RDONLY | O_CREATE);
    if(fileFd < 0)
		return -1;

    SaveDataInfo(fileFd, modbusCoil);

    for(i = 0; i < modbusCoil->number; i++)      //保存数据
    {
        if(tabBits[i] == 0)
        {
            SaveStringToFile(fileFd, "0,", strlen("0,"));
        }
        else if(tabBits[i] == 1)
        {
            SaveStringToFile(fileFd, "1,", strlen("1,"));
        }
        else
        {
            SaveStringToFile(fileFd, "error,", strlen("error,"));
        }

        if((i%10) == 9)
        {
            SaveStringToFile(fileFd, "\n", strlen("\n"));
        }
    }

    close(fileFd);
        
    return 0;
}


/**
 * @fn SaveRegisters
 * @brief 保存寄存器的数据
 * @param tabRegisters 需要保存的数据
 * @param modbusRegister 数据的相关信息结构体
 * @return 成功:0 错误:-1
 */
int SaveRegisters(uint16_t *tabRegisters, ConfigFile *modbusRegister)
{
    int fileFd = 0;         //文件描述符
    ssize_t writeNum = 0;   //写入的数据长度
    int i = 0;
    char fileName[40] = {0};    //文件名

    GetTimeStr(fileName);
    strncat(fileName, "Regiters.csv", strlen("Regiters.csv"));
    fileFd = open(fileName, O_RDONLY | O_CREATE);
    if(fileFd < 0)
		return -1;

    SaveDataInfo(fileFd, modbusRegister);

    for(i = 0; i < modbusRegister->number; i++)      //保存数据
    {
        SaveIntToFile(fileFd, (int)tabRegisters[i]);

        if((i%10) == 9)
        {
            SaveStringToFile(fileFd, "\n", strlen("\n"));
        }
    }

    close(fileFd);
}


/**
 * @fn SaveDataInfo
 * @brief 保存数据的说明信息
 * @param filefd 文件描述符
 * @param dataInfo 数据的相关信息结构体
 * @return 成功:0 错误:-1
 */
int SaveDataInfo(int filefd, ConfigFile *dataInfo)
{
    /* 保存信息的格式：Data Name,数据名称,Start Address,起始地址,Number,数量, */
    SaveStringToFile(fileFd, "Data Name,", strlen("Data Name,"));
    SaveStringToFile(fileFd, dataInfo->dataName, strlen(dataInfo->dataName));
    SaveStringToFile(fileFd, ",Start Address,", strlen(",Start Address,"));
    SaveIntToFile(fileFd, dataInfo->startAddress);
    SaveStringToFile(fileFd, ",Number,", strlen(",Number,"));
    SaveIntToFile(fileFd, dataInfo->number);
    SaveStringToFile(fileFd, ",\n", strlen(",\n"));

    return 0;
}


/**
 * @fn Int2Char
 * @brief 整型数转字符串
 * @param number 需要转换的数
 * @param string 转换后的字符串
 * @return 成功:转换后字符串长度 错误:-1
 */
int Int2Char(int number, char *string)
{
    int i = 0;
    bool isPositive = true;
    int temp = number;

    if(string == NULL)
    {
        return -1;
    }

    if(temp < 0)        //判断正负数
    {
        temp = -temp;
        string[i++] = '-';
        isPositive = false;
    }
    for(; temp > 0; i++)        //数字转换为字符
    {
        string[i] = temp % 10 + '0';
        temp /= 10;
    }
    string[i] = '\0';

    return i;
}


/**
 * @fn SaveIntToFile
 * @brief 保存整型数到文件
 * @param fileFd 文件描述符
 * @param number 需要保存的数
 * @return 成功:0 错误:-1
 */
int SaveIntToFile(int fileFd, int number)
{
    char string[20] = {0};  //存放数字转换后的字符串
    int stringLenth = 0;

    stringLenth = Int2Char(number, string);
    if(stringLenth == -1)
        return -1;
    return SaveStringToFile(fileFd, string);
}


/**
 * @fn SaveStringToFile
 * @brief 保存字符串到文件
 * @param fileFd 文件描述符
 * @param string 需要保存的字符串
 * @param stringLenth 字符串长度
 * @return 成功:0 错误:-1
 */
int SaveStringToFile(int fileFd, char *string, int stringLenth)
{
    ssize_t writeNum = 0;   //写入的数据长度
    writeNum = write(fileFd, string, stringLenth);
    if(writeNum < 0)
    {
        printf("write %s error!\n", string);
        return -1;
    }
    return 0;
}


/**
 * @fn GetTimeStr
 * @brief 获取当前时间字符串
 * @param timeStr 保存时间字符串
 * @return 成功:0 错误:-1
 */
int GetTimeStr(char *timeStr)
{
    time_t rawTime;
    struct tm *info;

    time(&rawTime);     //获取当前时间
    info = localtime(&rawTime);

    strftime(timeStr, 20, "%Y%m%d%H%M%S", info);    //时间字符串格式化

    return 0;
}

