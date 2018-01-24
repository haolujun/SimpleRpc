
/***************************************************************************************
 *
 * Copyright (c) 2014 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: BlockQueue.h
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 08/26/2016 02:48:00 PM
 * @version: 1.0 
 *   @brief: 
 *  
 **/
#ifndef __BLOCK_QUEUE__
#define __BLOCK_QUEUE__
#include <queue>
#include "Thread.h"

template <class EVENT>
class BlockQueue {
  public:
    BlockQueue(){
      _mutex = new Mutex();
      _cond = new Condition(_mutex);
    }
    
    ~BlockQueue(){
      delete _mutex;
      delete _cond;
    }

    bool empty(){
      return _qu.size() == 0 ? true : false;
    }

    EVENT get() {
      _mutex->lock();
      while(_qu.size() == 0) {
        _cond->wait();
      } 
      EVENT e = _qu.front();
      _qu.pop();
      _cond->notify();
      _mutex->unlock();
      return e;
    }

    void put(EVENT e) {
      _mutex->lock();
      while(_qu.size() >= 100){
        _cond->wait();
      } 
      _qu.push(e);
      _cond->notify();
      _mutex->unlock();
    }
  private:
    std::queue<EVENT> _qu;
    Condition *_cond;
    Mutex *_mutex;
};

#endif


