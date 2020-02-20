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


/**
 * @fn SaveCoilBits
 * @brief 保存离散线圈的数据
 * @param tabBits 需要保存的数据
 * @param modbusCoil 数据的相关信息结构体
 * @return 成功:0 错误:1
 */
int SaveCoilBits(uint8_t *tabBits, ConfigFile modbusCoil)
{
    int fileFd = 0;         //文件描述符
    ssize_t writeNum = 0;   //写入的数据长度
    int i = 0;

    fileFd = open("./CoilBits/20200220.csv", O_RDONLY | O_CREATE);
    if(fileFd < 0)
		return 1;

    writeNum = write(fileFd, "Data Name,", strlen("Data Name,"));
    writeNum = write(fileFd, modbusCoil.dataName, strlen(modbusCoil.dataName));
    writeNum = write(fileFd, ",Start Address,", strlen(",Start Address,"));
    SaveIntToFile(fileFd, modbusCoil.startAddress);
    writeNum = write(fileFd, ",Number,", strlen(",Number,"));
    SaveIntToFile(fileFd, modbusCoil.number);
    writeNum = write(fileFd, ",\n", strlen(",\n"));

    for(i = 0; i < modbusCoil.number; i++)
    {
        if(1 == tabBits[i])
        {
            writeNum = write(fileFd, "1,", strlen("1,"));
        }
        else if(0 == tabBits[i])
        {
            writeNum = write(fileFd, "0,", strlen("0,"));
        }
        else
        {
            writeNum = write(fileFd, "error,", strlen("error,"));
        }

        if((i%10) == 9)
        {
            writeNum = write(fileFd, "\n", strlen("\n"));
        }
    }

    close(fileFd);
        
    return 0;
}


/**
 * @fn Int2Char
 * @brief 整型数转字符串
 * @param number 需要转换的数
 * @param string 转换后的字符串
 * @return 成功:0 错误:1
 */
int Int2Char(int number, char *string)
{
    int i = 0;
    bool isPositive = true;
    int temp = number;

    if(string == NULL)
    {
        return 1;
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

    return 0;
}


/**
 * @fn SaveIntToFile
 * @brief 保存整型数到文件
 * @param fileFd 文件描述符
 * @param number 需要保存的数
 * @return 成功:0 错误:1
 */
int SaveIntToFile(int fileFd, int number)
{
    char string[20] = {0};  //存放数字转换后的字符串
    ssize_t writeNum = 0;   //写入的数据长度

    Int2Char(number, string);
    writeNum = write(fileFd, string, strlen(string));
    if(writeNum < 0)
    {
        printf("write error!\n");
        return 1;
    }
    return 0;
}

