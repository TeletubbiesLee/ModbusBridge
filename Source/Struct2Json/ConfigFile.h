/**
 * @file ConfigFile.h
 * @brief 程序部分参数配置文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company SOJO
 * @date 2020.02.18
 *
 * @author Lei.L
 * @version ver 1.0
 */
 

#ifndef __CONFIG_FILE_H
#define	__CONFIG_FILE_H

#include "../Config.h"

/****************************宏定义****************************/
#define DIS_ENABLE  0               //禁止
#define ENABLE      !DIS_ENABLE     //使能

#define PARAM_ERROR  -1   //参数错误

#define CHECK_PARAM(parameter) {parameter > 1 ? PARAM_ERROR : 1}    //检测参数


/****************************结构体定义****************************/
typedef struct tagConfiguiation
{
    char dataName[DATA_NAME_LENTH_MAX];				//名称
    int functionCode;		//功能码
    int dataType;			//数据类型
    int serialNumber;		//序号
    int startAddress;      //起始地址
    int number;			//大小
}ConfigFile;


/****************************全局变量定义****************************/
extern ConfigFile g_ModbusConfigFile[MODBUS_CONFIG_STRUCT_MAX];
extern ConfigFile *g_JsonFile;

/****************************函数定义****************************/
extern int GetConfigFileLength(void);
extern void ConfigFileInit(void);

/**********************************END**********************************/


#endif


