
/***************************************************************************************
 *
 * Copyright (c) 2016 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: TestClent.cpp
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 10/10/2016 05:01:51 PM
 * @version: 1.0 
 *   @brief: 
 *  
 **/

#include <string>
#include "Add.h"
#include "SimpleRpc.h"

int main(){
  Server server(
    std::string("add"), std::string("add"), std::string("127.0.0.1"), std::string("27008"));
  CalcRequest req;
  req.set_a(100);
  req.set_b(100);
  AddRequest add_request;
  AddResponse add_response;
  add_request.set_req(req);

  for(int i = 0; i < 10; ++i) {
    SimpleRpcClient::get_instance()->sync_request(
      server, (Request*)&add_request, (Response*)&add_response);
    printf("result = %d\n", add_response.get_response().result());
  }

  return 0;
}
