
/***************************************************************************************
 *
 * Copyright (c) 2016 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: Test.cpp
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 10/13/2016 04:50:42 PM
 * @version: 1.0 
 *   @brief: 
 *  
 **/
#include <stdio.h>
#include <string>
#include "calculate.pb.h"
using namespace std;

int main(){
  CalcRequest req;
  req.set_a(1000);
  req.set_b(1000);
  char buf[10];
  string data;
  req.SerializeToArray(buf, 10);
  req.SerializeToString(&data);
  int size = req.ByteSize();
  CalcRequest req_1;
  req_1.ParseFromArray(buf, size);
  printf("%d %d\n", req_1.a(), req_1.b());
  CalcRequest req_2;
  req_2.ParseFromString(data);
  printf("%d %d\n", req_2.a(), req_2.b());
  return 0;
}

