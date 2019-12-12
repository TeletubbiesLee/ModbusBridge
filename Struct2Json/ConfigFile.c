/**
 * @file ConfigFile.c
 * @brief 程序部分参数配置文件
 * @copyright Copyright (c) 2017 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2018.6.15
 *
 * @author Zhangxiaomou
 * @version ver 1.0
 */
 

#include "ConfigFile.h"
#include <string.h>
#include <stdio.h>


/*************************************************静态变量*************************************************/
ConfigFile g_ModbusConfigFile[MODBUS_CONFIG_STRUCT_MAX];

/*************************************************END*************************************************/

/*************************************************公有函数实现*************************************************/
/**
 * @fn GetConfigFileLength
 * @brief 获取配置文件结构体数组长度
 * @return 返回数组长度
 * 
 */
inline uint16_t GetConfigFileLength(void)
{
    return (sizeof(g_ModbusConfigFile) / sizeof(g_ModbusConfigFile[0]));    //配置文件结构体数组长度;
}


void ConfigFileInit(void)
{
	uint16_t lenth = 0;
	uint8_t i = 0;
	lenth = GetConfigFileLength();
	for(i = 0; i < lenth; i++)
	{
		strcpy(g_ModbusConfigFile[i].dataName, "default");
		g_ModbusConfigFile[i].functionCode = 0;
		g_ModbusConfigFile[i].dataType = 0;
		g_ModbusConfigFile[i].serialNumber = i + 1;
		g_ModbusConfigFile[i].startAddress = 0;
		g_ModbusConfigFile[i].number = 0;
	}
}

/*************************************************END*************************************************/









