
/***************************************************************************************
 *
 * Copyright (c) 2014 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: Reactor.h
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 08/22/2016 03:31:50 PM
 * @version: 1.0 
 *   @brief: 
 *  
 **/

#ifndef __REACTOR_H__
#define __REACTOR_H__
#include "Thread.h"
#include "Network.h"
class EventHandler{
  public:
    virtual ~EventHandler(){}
    virtual void handle_read(int fd) = 0;
    virtual void handle_write(int fd) = 0;
};

class ReactorImplementation {
  public:
    virtual ~ReactorImplementation(){}
    virtual void regist(int fd, EventHandler *handler) = 0;
    virtual void remove(int fd) = 0;
    virtual void handle_events() = 0;
};

class Reactor {
  
    public:
      void regist(int fd, EventHandler *handler);
      void handle_events();
      void remove(int fd);
      static Reactor* get_instance(){
        if(_instance == NULL) {
          _mutex->lock();
          if(_instance == NULL){
            _instance = new Reactor();
          }
          _mutex->unlock();
        }
        return _instance;
      }

      ~Reactor(){}
    private:
      Reactor();
      static Reactor *_instance;
      static Mutex *_mutex;
      ReactorImplementation *_reactor_imp;
};



class Acceptor : public EventHandler{
  public:
    Acceptor(const InetAddr &addr, Reactor *reactor);
    void handle_read(int fd);
    void handle_write(int fd);
    void handle_timeout(int fd);
    void handle_close(int fd);
    int open();
  private:
    Reactor *_reactor;
};

//ReactorImplementation* Reactor::_reactor_imp  = NULL;
#endif

