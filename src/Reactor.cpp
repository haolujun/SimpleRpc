
/***************************************************************************************
 *
 * Copyright (c) 2016 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: Reactor.cpp
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 08/29/2016 03:04:01 PM
 * @version: 1.0 
 *   @brief: 
 *  
 **/
#include <stdio.h>
#include "Log.h"
#include "Reactor.h"
#include "SimpleRpc.h"
#include "EpollReactorImplementation.h"

Reactor* Reactor::_instance = 0x00;
Mutex* Reactor::_mutex = new Mutex();

Reactor::Reactor(){
  _reactor_imp = new EpollReactorImplementation();
}

void Reactor::regist(int fd, EventHandler *handler){
  _reactor_imp->regist(fd, handler);
}

void Reactor::remove(int fd){
  _reactor_imp->remove(fd);
}

void Reactor::handle_events(){
  _reactor_imp->handle_events();
}

Acceptor::Acceptor(const InetAddr &addr, Reactor *reactor){
  _reactor = reactor;
  int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr sock_addr = addr.addr();
  int ret = bind(sock_fd, &sock_addr, sizeof(sockaddr));
  if(ret != 0){
    LOG("bind error");
    exit(0);
  }
  ret = listen(sock_fd, 1000);
  if(ret != 0) {
    LOG("listen error");
    exit(0);
  }
  _reactor->regist(sock_fd, this);
}

void Acceptor::handle_read(int sock_fd) {
  struct sockaddr addr;
  socklen_t size = sizeof(struct sockaddr_in);
  int fd = accept(sock_fd, &addr, &size);
  printf("Accept new fd:%d\n", fd);
  _reactor->regist(fd, new UpstreamHandler(fd, _reactor));
}

void Acceptor::handle_write(int fd) {
  //do nothing
}

void Acceptor::handle_timeout(int fd) {
}

void Acceptor::handle_close(int fd) {
}

