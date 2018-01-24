
/***************************************************************************************
 *
 * Copyright (c) 2016 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: Thread.cpp
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 08/29/2016 04:12:49 PM
 * @version: 1.0 
 *   @brief: 
 *  
 **/
#include "Thread.h"

void Thread::start(){
  pthread_create(&_pid, NULL, Thread::work, this);
}

void Thread::join(){
  pthread_join(_pid, NULL);
}

pthread_t Thread::pid(){
  return _pid;
}

Mutex::Mutex(){
  pthread_mutex_init(&_mutex, NULL);
}

Mutex::~Mutex(){
  pthread_mutex_destroy(&_mutex);
}

void Mutex::lock(){
  pthread_mutex_lock(&_mutex);
}

void Mutex::unlock(){
  pthread_mutex_unlock(&_mutex);
}

Condition::Condition(Mutex *mutex) {
  _mutex = mutex;
  pthread_cond_init(&_cond, NULL); 
}

Condition::~Condition(){
  pthread_cond_destroy(&_cond);
}

void Condition::wait(unsigned int nsec){
  if(nsec == 0){
    pthread_cond_wait(&_cond, &(_mutex->_mutex));
  } else {
    struct timespec timer;
    timer.tv_sec = time(NULL) + nsec / 1000000;
    timer.tv_nsec = nsec % 1000000;
    pthread_cond_timedwait(&_cond, &_mutex->_mutex, &timer);
  }
}

void Condition::notify(){
  pthread_cond_signal(&_cond);
}
