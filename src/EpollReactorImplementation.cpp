
/***************************************************************************************
 *
 * Copyright (c) 2016 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: EpollReactorImplementation.cpp
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 09/03/2016 04:26:20 PM
 * @version: 1.0 
 *   @brief: 
 *  
 **/
#include <sys/socket.h> 
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "EpollReactorImplementation.h"
#include "Log.h"

EpollReactorImplementation::EpollReactorImplementation() {
  _epoll_fd = epoll_create(1000);
  _events = new epoll_event[1000];
}

void EpollReactorImplementation::regist(int fd, EventHandler *handler) {
  _handlers[fd] = handler;
  _mutex.lock();
  struct epoll_event e;
  e.data.fd = fd;
  e.events = EPOLLIN | EPOLLET | EPOLLERR;
  int ret = epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &e);
  _mutex.unlock();
  if(ret == -1) {
    LOG("epoll regist fd=%d failed", fd);
  } else {
    LOG("epoll regist fd=%d succeed", fd);
  }
}

void EpollReactorImplementation::remove(int fd) {
  _mutex.lock();
  struct epoll_event e;
  epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, fd, &e);
  _handlers.erase(fd);
  _mutex.unlock();
}

void EpollReactorImplementation::handle_events(){
  int n = epoll_wait(_epoll_fd, _events, 64, -1);
  LOG("Epool reach %d event", n);
  for(int i = 0; i < n; ++i){
    if(_events[i].events & EPOLLERR ||
       _events[i].events & EPOLLHUP)
    {
      LOG("epoll fd=%d ERR HUP", _events[i].data.fd);
      continue;
    } 

    if(_events[i].events & EPOLLRDHUP){
      epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, _events[i].data.fd, &_events[i]);
      close(_events[i].data.fd);
      LOG("epoll fd=%d close", _events[i].data.fd);
      continue;
    }

    if(_events[i].events & EPOLLIN){
      LOG("epoll in data fd=%d", _events[i].data.fd);
      int fd = _events[i].data.fd;
      EventHandler *handler = _handlers[fd];
      if(handler != NULL) {
        handler->handle_read(fd);
      }
    }
    
    if(_events[i].events & EPOLLOUT){
      LOG("epoll out data fd=%d", _events[i].data.fd);
      int fd = _events[i].data.fd;
      EventHandler *handler = _handlers[fd];
      if(handler != NULL) {
        handler->handle_write(fd);
      }
    }
  }
}

