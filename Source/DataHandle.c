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
#include <fcntl.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "./ParserCSV/csv.h"
#include "Config.h"


static int SaveDataInfo(int filefd, ConfigFile *dataInfo);
static int Int2String(int number, char *string);
static int SaveIntToFile(int fileFd, int number);
static int SaveStringToFile(int fileFd, char *string, int stringLenth);
static int GetTimeStr(char *timeStr);


/**
 * @fn SaveBitsFile
 * @brief 保存离散线圈的数据到文件
 * @param tabBits 需要保存的数据
 * @param modbusCoil 数据的相关信息结构体
 * @return 成功:0 错误:其他
 */
int SaveBitsFile(uint8_t *tabBits, ConfigFile *modbusCoil)
{
    int fileFd = 0;         //文件描述符
    int i = 0;
    char fileName[FILENAME_STRING_MAX] = {0};    //文件名
    char temp[20] = {0};

    GetTimeStr(temp);
	strncat(fileName, CSV_DIR_NAME, strlen(CSV_DIR_NAME));
	strncat(fileName, temp, strlen(temp));
    strncat(fileName, "Bits.csv", strlen("Bits.csv"));
    fileFd = open(fileName, O_RDWR | O_CREAT);
    if(fileFd < 0)
		return OPEN_FILE_FAIL;

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
        
    return NO_ERROR;
}


/**
 * @fn SaveRegistersFile
 * @brief 保存寄存器的数据到文件
 * @param tabRegisters 需要保存的数据
 * @param modbusRegister 数据的相关信息结构体
 * @return 成功:0 错误:其他
 */
int SaveRegistersFile(uint16_t *tabRegisters, ConfigFile *modbusRegister)
{
    int fileFd = 0;         //文件描述符
    int i = 0;
    char fileName[FILENAME_STRING_MAX] = {0};    //文件名
    char temp[20] = {0};

    GetTimeStr(temp);
    strncat(fileName, CSV_DIR_NAME, strlen(CSV_DIR_NAME));
    strncat(fileName, temp, strlen(temp));
    strncat(fileName, "Regiters.csv", strlen("Regiters.csv"));
    fileFd = open(fileName, O_RDWR | O_CREAT);
    if(fileFd < 0)
		return OPEN_FILE_FAIL;

    SaveDataInfo(fileFd, modbusRegister);

    for(i = 0; i < modbusRegister->number; i++)      //保存数据
    {
        SaveIntToFile(fileFd, (int)tabRegisters[i]);
        SaveStringToFile(fileFd, ",", strlen(","));

        if((i%10) == 9)
        {
            SaveStringToFile(fileFd, "\n", strlen("\n"));
        }
    }

    close(fileFd);
    return NO_ERROR;
}


/**
 * @fn SaveDataInfo
 * @brief 保存数据的说明信息
 * @param filefd 文件描述符
 * @param dataInfo 数据的相关信息结构体
 * @return 成功:0 错误:其他
 */
static int SaveDataInfo(int fileFd, ConfigFile *dataInfo)
{
    /* 保存信息的格式：Data Name,数据名称,Start Address,起始地址,Number,数量, */
    SaveStringToFile(fileFd, "Data Name,", strlen("Data Name,"));
    SaveStringToFile(fileFd, dataInfo->dataName, strlen(dataInfo->dataName));
    SaveStringToFile(fileFd, ",Start Address,", strlen(",Start Address,"));
    SaveIntToFile(fileFd, dataInfo->startAddress);
    SaveStringToFile(fileFd, ",Number,", strlen(",Number,"));
    SaveIntToFile(fileFd, dataInfo->number);
    SaveStringToFile(fileFd, ",\n", strlen(",\n"));

    return NO_ERROR;
}


/**
 * @fn Int2String
 * @brief 整型数转字符串
 * @param number 需要转换的数
 * @param string 转换后的字符串
 * @return 成功:转换后字符串长度 错误:其他
 */
static int Int2String(int number, char *string)
{
    int i = 0, j = 0;
    int tempNum = number;
    char tempString[20] = {0};

    if(string == NULL)
    {
        return FUNCTION_FAIL;
    }

    if(tempNum == 0)
	{
		string[0] = '0';
		string[1] = '\0';
		return 1;
	}

    if(tempNum < 0)        //判断正负数
    {
    	tempNum = -tempNum;
    	string[j++] = '-';
    }

    for(; tempNum > 0; i++)        //数字转换为字符
    {
    	tempString[i] = tempNum % 10 + '0';
        tempNum /= 10;
    }

    for(; i > 0; j++)
    {
    	string[j] = tempString[--i];
    }
    string[j] = '\0';

    return j;
}


/**
 * @fn SaveIntToFile
 * @brief 保存整型数到文件
 * @param fileFd 文件描述符
 * @param number 需要保存的数
 * @return 成功:0 错误:其他
 */
static int SaveIntToFile(int fileFd, int number)
{
    char string[20] = {0};  //存放数字转换后的字符串
    int stringLenth = 0;

    stringLenth = Int2String(number, string);
    if(stringLenth == -1)
        return FUNCTION_FAIL;
    return SaveStringToFile(fileFd, string, stringLenth);
}


/**
 * @fn SaveStringToFile
 * @brief 保存字符串到文件
 * @param fileFd 文件描述符
 * @param string 需要保存的字符串
 * @param stringLenth 字符串长度
 * @return 成功:0 错误:其他
 */
static int SaveStringToFile(int fileFd, char *string, int stringLenth)
{
    ssize_t writeNum = 0;   //写入的数据长度
    writeNum = write(fileFd, string, stringLenth);
    if(writeNum < 0)
    {
    	printf_debug("write %s error!\n", string);
        return FUNCTION_FAIL;
    }
    return NO_ERROR;
}


/**
 * @fn GetTimeStr
 * @brief 获取当前时间字符串
 * @param timeStr 保存时间字符串
 * @return 成功:0 错误:-1
 */
static int GetTimeStr(char *timeStr)
{
    time_t rawTime;
    struct tm *info;

    time(&rawTime);     //获取当前时间
    info = localtime(&rawTime);

    strftime(timeStr, 20, "%Y%m%d%H%M%S", info);    //时间字符串格式化

    return NO_ERROR;
}


/**
 * @fn ParseCSVDataFile
 * @brief 解析数据文件。解析bit，则registerData赋值NULL；解析register，则bitData赋值NULL
 * @param fileName 文件名
 * @param bitData 需要保存的bit数据
 * @param registerData 需要保存的寄存器数据
 * @param arrayNumber 数据数组大小
 * @return 成功:0 错误:其他
 */
int ParseCSVDataFile(char *fileName, uint8_t *bitData, uint16_t *registerData, int arrayNumber)
{
    FILE *fp = NULL;
    int err, done = 0;
    char *csvLineString = NULL;     //一行csv文件字符串
    char **parsed = NULL;           //一个csv文件中一个字符串元素
    int i = 0, num = 0;
    bool bitOrRegister = IS_BIT;

    if(NULL != bitData && NULL == registerData)
    {
        bitOrRegister = IS_BIT;
        memset(bitData, 0, arrayNumber);
    }
    else if(NULL == bitData && NULL != registerData)
    {
        bitOrRegister = IS_REGISTER;
        memset(registerData, 0, arrayNumber);
    }
    else
    {
        return ERROR_ARGUMENTS;
    }

    fp = fopen(fileName, "r");

    /* 保存信息的格式：Data Name,数据名称,Start Address,起始地址,Number,数量, */
    csvLineString = fread_csv_line(fp, CSV_LINE_STRING_MAX, &done, &err);
    parsed = parse_csv(csvLineString);

    while(num < arrayNumber)
    {
        csvLineString = fread_csv_line(fp, CSV_LINE_STRING_MAX, &done, &err);
        parsed = parse_csv(csvLineString);
        for(i = 0; i < 10 && num < arrayNumber; i++)
        {
            switch (bitOrRegister)
            {
                case IS_BIT:
                    bitData[num] = String2Int(parsed[i], strlen(parsed[i]));
                    break;
                case IS_REGISTER:
                    registerData[num] = String2Int(parsed[i], strlen(parsed[i]));
                    break;
                default:
                    break;
            }
            num++;
        }
    }

    fclose(fp);
    fp = NULL;
    
    return NO_ERROR;
}


/**
 * @fn String2Int
 * @brief 字符串转整型
 * @param string 需要转换的字符串
 * @param stringLenth 字符串长度
 * @return 转换后的整数
 */
int String2Int(char *string, int stringLenth)
{
    int number = 0;         //保存转换后的数
    bool isPositive = true;     //正数标志位
    int i = 0;

    for(i = 0; i < stringLenth && string[i] != '\0'; i++)
    {
        if(i == 0 && string[i] == '-')
        {
            isPositive = false;
            continue;
        }
        if('0' <= string[i] && string[i] <= '9')
        {
			number *= 10;
			number += string[i] - '0';
        }
        else
        {
        	printf_debug("\'String2Int(%s, %d)\' error!\n", string, stringLenth);
        	return 0xFFFFFFFF;
        }
    }
    if(isPositive == false)
    {
        number = -number;
    }

    return number;
}


