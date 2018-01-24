
/***************************************************************************************
 *
 * Copyright (c) 2016 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: ScarchService.cpp
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 10/14/2016 05:36:23 PM
 * @version: 1.0 
 *   @brief: 
 *  
 **/
#include <cstring>
#include "Log.h"
#include "SimpleRpc.h"

Mutex* SimpleRpcClient::_mutex = new Mutex();
SimpleRpcClient* SimpleRpcClient::_instance = 0x00;
SimpleRpcClient::ReactorThread* SimpleRpcClient::_reactor_thread = 0x00;

void DownstreamHandler::process_response(int fd) {
  char head[4];
  Connection conn(fd);
  conn.recv_n(head, 4);
  int size = *((int *)head);
  char *buf = new char[size];
  conn.recv_n(buf, size);
  close(fd);  
  //下游响应
  _response->deserialize(buf, size);
  //如果有result_handler,则调用data_comeback钩子函数
  if(_result_handler != NULL) {
    _result_handler->data_comeback();
  }
  
  delete[] buf;
  //自杀
  delete this;
}

void DownstreamHandler::handle_read(int fd) {
  if(fd != _sock_fd){
    return;
  } 
  _reactor->remove(fd);
  //StreamEvent event;
  //event.fd = fd;
  //event.type = 1;
  //event.response_handler = this;
  //ThreadPool<StreamEvent>::get_instance()->put_event(event); 
  //printf("DownstreamHandler handler_read\n");
  process_response(fd);
}

void DownstreamHandler::handle_write(int fd) {

}

void UpstreamHandler::handle_read(int fd) {
  if(fd != _sock_fd){
    return;
  }

  StreamEvent e;
  e.fd = _sock_fd;
  e.type = 0;
  _reactor->remove(fd);
  //由每个工作线程自己去读fd,客户端请求事件
  ThreadPool<StreamEvent>::get_instance()->put_event(e); 
  delete this;
}

void UpstreamHandler::handle_write(int fd) {
  
}

int SimpleRpcClient::async_request(
  Server &server, Request *request, Response *response, ResultHandler *handler) {
  Connection conn;
  InetAddr addr(server.get_port_str(), server.get_ip_str());
  Connector conntor(addr);
  int ret = conntor.Connect(conn, 100);
  
  if(ret == -1){
    LOG("connect error\n");
    return -1;
  }

  int size = request->bytes();
  char *buf = new char[size + 4];

  if(buf == NULL) {
    LOG("request oom, request need %d bytes\n", size + 4);
    conn.Close();
    return -1;
  }
    
  int payload = request->serialize(buf + 4, size);
  memcpy(buf, &payload, sizeof(int));
  
  ret = conn.send_n(buf, payload + 4);
  if(ret != 0) {
    LOG("connection send error\n");
    return -1;
  }
  DownstreamHandler *down_handler = 
    new DownstreamHandler(conn.sock(), response, Reactor::get_instance(), handler);
  Reactor::get_instance()->regist(conn.sock(), down_handler);
  
  delete[] buf;
  return 0;
}

int SimpleRpcClient::sync_request(Server &server, Request *request, Response *response) {
  Mutex mutex;
  Connection conn;
  Condition cond(&mutex);
  InetAddr addr(server.get_port_str(), server.get_ip_str());
  Connector conntor(addr);
  int ret = conntor.Connect(conn);
  if(ret == -1){
    LOG("connect error\n");
    return -1;
  }
  int size = request->bytes();
  char *buf = new char[size + 4];
  if(buf == NULL) {
    LOG("request oom, request need %d bytes\n", size + 4);
    conn.Close();
    return -1;
  }
  int payload = request->serialize(buf + 4, size);
  memcpy(buf, &payload, sizeof(int));
  ret = conn.send_n(buf, payload + 4);
  if(ret != 0) {
    LOG("connection send error\n");
    return -1;
  }
  DefaultResultHandler *handler = new DefaultResultHandler(&cond, &mutex);
  DownstreamHandler *down_handler = 
    new DownstreamHandler(conn.sock(), response, Reactor::get_instance(), handler);
  Reactor::get_instance()->regist(conn.sock(), down_handler);
  handler->finish();
  delete[] buf;
  delete handler;
  return 0;
}

