
/***************************************************************************************
 *
 * Copyright (c) 2014 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: EpollReactorImplementation.h
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 09/01/2016 04:40:57 PM
 * @version: 1.0 
 *   @brief: 
 *  
 **/

#ifndef __EPOLL_REACTOR_IMPLEMENTATION_H__
#define __EPOLL_REACTOR_IMPLEMENTATION_H__
#include <sys/epoll.h>
#include <map>
#include "Reactor.h"
class EpollReactorImplementation : public ReactorImplementation{
  public:
    EpollReactorImplementation();
    void regist(int fd, EventHandler *handler); 
    void remove(int fd);
    void handle_events();

  private:
    Mutex _mutex;
    int _epoll_fd;
    struct epoll_event *_events;
    std::map<int, EventHandler*> _handlers;
};

#endif

