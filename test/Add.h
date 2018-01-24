
/***************************************************************************************
 *
 * Copyright (c) 2014 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: Add.h
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 10/14/2016 05:52:47 PM
 * @version: 1.0 
 *   @brief: 
 *  
 **/

#ifndef __AS_H__
#define __AS_H__
#include "SimpleRpc.h"
#include "Thread.h"
#include "calculate.pb.h"

class AddRequest : public Request {
  public:
    AddRequest(){
    
    }
    
    int serialize(char *buf, int size) {
       _req.SerializeToArray(buf, size);
       return _req.ByteSize();
    }

    int deserialize(char *buf, int size) {
      _req.ParseFromArray(buf, size); 
      return 0;
    }

    int bytes() {
      return _req.ByteSize();
    }

    CalcRequest get_req() {
      return _req;
    }

    void set_req(CalcRequest req) {
      _req = req;
    }

  private:
    CalcRequest _req;
};

class AddResponse : public Response {
  public:
    AddResponse(){
    
    }

    int serialize(char *buf, int size) {
      _resp.SerializeToArray(buf, size);    
      return _resp.ByteSize();
    }
    
    int deserialize(char *buf, int size) {
      _resp.ParseFromArray(buf, size);
      return 0;
    }
    
    int bytes() {
      return _resp.ByteSize();
    }

    CalcResponse get_response() {
      return _resp;
    }

    void set_response(CalcResponse response) {
      _resp = response;
    }

  private:
    CalcResponse _resp;
};

class AddProcessor : public Processor<AddRequest, AddResponse>{
  public:
    int process(AddRequest &request, AddResponse &response) {
      CalcRequest req = request.get_req();
      CalcResponse resp;
      resp.set_result(req.a() + req.b());
      response.set_response(resp);
      return 0;
    }
};

class AddResultHandler : public ResultHandler {
  public:
    
    AddResultHandler(Condition *cond, Mutex *mutex){
      _cond = cond;
      _mutex = mutex;
      _finish = 0;
    }

    void data_comeback(){
      _finish++;
      _mutex->lock();
      _cond->notify();
      _mutex->unlock();
    }

    void finish(){
      _mutex->lock();
      while(_finish != 1) {
        _cond->wait(0);
      }
      _mutex->unlock();
    }

  private:
    Mutex *_mutex;
    Condition *_cond;
    volatile int _finish;
};

class AddProcessorFactory : public ProcessorFactory<AddRequest, AddResponse> {
  public:
    Processor<AddRequest, AddResponse>* make_processor(){
      AddProcessor* processor = new AddProcessor();
      return processor;
    }
};

#endif
