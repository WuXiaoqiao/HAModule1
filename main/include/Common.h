/*
 * Common.cpp
 *
 *  Created on: 25.10.2017
 *      Author: Xiaoqiao
 */
#ifndef _COMMON_H_
#define _COMMON_H_

#include "stdint.h"

#define SSID_MACRO "Your_SSID"
#define PASS_MACRO "YourNetWorkpassword"
#define UDP_LOGGING_MAX_PAYLOAD_LEN 2048


void connectUDP();

void obtain_time();

void LogCrashDump();

int64_t hmMillis();

#endif

//git update-index --assume-unchanged Common.h
