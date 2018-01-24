
/***************************************************************************************
 *
 * Copyright (c) 2014 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: ThreadPool.h
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 08/26/2016 02:05:11 PM
 * @version: 1.0 
 *   @brief: 
 *  
 **/

#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__
#include <map>
#include "BlockQueue.h"
#include "Thread.h"
template <class EVENT>
class ThreadPool{
  public:
    EVENT get_event() {
      EVENT e = _queue.get();
      return e;
    }
    void put_event(EVENT e) {
      _queue.put(e);
    }
    void join_thread(Thread *th){
      pid_t pid = th->pid();
      if(_threads.find(pid) != _threads.end()){
        _threads[pid] = th;
      }
    }
    void remove_thread(Thread *th){
      _oper_mutex.lock();
      _threads.erase(th->pid());
      _oper_mutex.unlock();
    }
    
    void remove_thread(pthread_t tid){
      _oper_mutex.lock();
      _threads.erase(tid);
      _oper_mutex.unlock();
    }

    static ThreadPool<EVENT>* get_instance() {
      if(_instance == NULL) {
        _mutex->lock();
        if(_instance ==  NULL) {
          _instance = new ThreadPool<EVENT>();   
        }
        _mutex->unlock();
      }
      return _instance;
    }
  private:
    ThreadPool() {
    }
    BlockQueue<EVENT> _queue;
    static ThreadPool<EVENT>* _instance;
    static Mutex* _mutex;
    Mutex _oper_mutex;
    std::map<pid_t, Thread*> _threads;
};

template<class EVENT>
Mutex* ThreadPool<EVENT>::_mutex = new Mutex();

template<class EVENT>
ThreadPool<EVENT>* ThreadPool<EVENT>::_instance = 0x00;

#endif
