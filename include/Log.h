
/***************************************************************************************
 *
 * Copyright (c) 2014 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: Log.h
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 10/11/2016 01:48:47 PM
 * @version: 1.0 
 *   @brief: 
 *  
 **/
#ifndef __LOG_H__
#define __LOG_H__
#include <stdio.h>
#define DEBUG
#ifdef DEBUG
  #define LOG(...) do{printf(__VA_ARGS__); printf("\n");}while(0)
#else
  #define LOG(...) do{}while(0)
#endif
#endif
