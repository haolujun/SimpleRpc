
/***************************************************************************************
 *
 * Copyright (c) 2016 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: Main.cpp
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 08/26/2016 04:16:36 PM
 * @version: 1.0 
 *   @brief: 
 *  
 **/
#include "Add.h"
#include "SimpleRpc.h"

int main(){
  //服务先启动
  std::string service_name("add");
  SimpleRpcService<AddRequest, AddResponse, AddProcessorFactory> *service_add;
  service_add = new SimpleRpcService<AddRequest, AddResponse, AddProcessorFactory>(
    std::string("27008"), service_name);
  service_add->start();
  return 0;
}
