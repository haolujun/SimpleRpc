
/***************************************************************************************
 *
 * Copyright (c) 2014 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: Thread.h
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 08/26/2016 01:43:13 PM
 * @version: 1.0 
 *   @brief: 
 *  
 **/

#ifndef __THREAD_H__
#define __THREAD_H__
#include <pthread.h>

class Thread {
  public:
    void start();
    virtual void run() = 0;
    void join();
    pthread_t pid();
  private:
    pthread_t _pid;
    static void* work(void *param){
      Thread *t = (Thread *)param;
      t->run();
    }
};

class Mutex {
  public:
      Mutex();
      ~Mutex();
      void lock();
      void unlock();
      friend class Condition;
  private:
    pthread_mutex_t _mutex;
};

class Condition{
  public:
    Condition(Mutex *mutex);
    ~Condition();
    void wait(unsigned int nsec = 0);
    void notify();
    void notify_all();
  private:
    Mutex *_mutex;
    pthread_cond_t _cond;
};

#endif


