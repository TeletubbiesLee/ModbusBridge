/**
 * @file Config.h
 * @brief 配置相关的头文件
 * @copyright Copyright (c) 2020 Beijing SOJO Electric CO., LTD.
 * @company  SOJO
 * @date 2020.03.24
 *
 * @author Lei.L
 * @version ver 1.0
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#define DEBUG_PRINTF
#ifdef DEBUG_PRINTF
#define printf_debug(fmt,...) do{printf("%s: %s[line %d]: "fmt, __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__);}while(0)
#endif


#define JSON_FILENAME					"ModbusConfigFile.json"		//获取或保存json配置文件的文件路径名
#define MODBUS_CONFIG_STRUCT_MAX		30							//modbus配置结构体数组的最大数量
#define DATA_NAME_LENTH_MAX				20							//modbus配置结构体中数据名字字符串长度最大值

#define CSV_DIR_NAME					"CSV_Data/"				//存放csv数据的文件夹名
#define FILENAME_STRING_MAX				60							//csv数据文件名字符串长度最大值
#define CSV_LINE_STRING_MAX				1024						//csv文件一行最大的字符串长度

#define RS485_MODE						1
#define SERVER_ID           			1
#define UART_DEVICE_NAME    			"/dev/ttymxc5"
#define UART_BANDRATE       			115200
#define UART_PARITY         			'N'
#define UART_DATA_BIT       			8
#define UART_STOP_BIT       			1

#define UT_BITS_NUMBER 					256
#define UT_INPUT_BITS_NUMBER			256
#define UT_REGISTERS_NUMBER				512
#define UT_INPUT_REGISTERS_NUMBER		512


/* 程序错误码 */
typedef enum {
	FUNCTION_FAIL = -1,		//函数执行错误
	NO_ERROR = 0,			//没有错误
	POINT_NULL,				//指针为空
	OPEN_FILE_FAIL,			//打开文件失败
	MALLOC_FAIL,			//分配堆内存失败
	FEW_ARGUMENTS,			//参数太少
	ERROR_ARGUMENTS,		//参数错误


}ErrorCode;

#endif
