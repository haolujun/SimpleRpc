
/***************************************************************************************
 *
 * Copyright (c) 2014 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: Worker.h
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 08/26/2016 01:54:08 PM
 * @version: 1.0 
 *   @brief: 
 *  
 **/

#ifndef __WORKER_H__
#define __WORKER_H__
#include "Thread.h"
#include "ThreadPool.h"
template <class EVENT>
class Worker : public Thread{
  public:
    Worker(){
      _pool = NULL;
    }

    Worker(ThreadPool<EVENT> *pool) {
       _pool = pool;
      if(_pool != NULL){
        join_pool(_pool);
      }
    }
    void join_pool(ThreadPool<EVENT> *pool){
      _pool = pool; 
    }
  protected:
    EVENT get_event(){
      return _pool->get_event();
    }
  private:
    ThreadPool<EVENT> *_pool;
};
#endif
