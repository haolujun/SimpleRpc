
/***************************************************************************************
 *
 * Copyright (c) 2014 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: ScarchService.h
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 08/26/2016 03:22:50 PM
 * @version: 1.0 
 *   @brief: 
 *  
 **/

#ifndef __SCARCH_SERVICE_H__
#define __SCARCH_SERVICE_H__
#include <stdio.h>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "Reactor.h"
#include "Worker.h"
#include "Thread.h"
#include "ThreadPool.h"

class Server {
  public:
    Server(){}
    Server(std::string server_name, std::string service_name, std::string ip, std::string port) {
      _server_name = server_name;
      _service_name = service_name;
      _ip = ip;
      _port = port;
    }
    std::string get_ip_str(){
      return _ip;
    }

    std::string get_port_str(){
      return _port;
    }

    uint get_ip_int(){
      return inet_addr(_ip.c_str()); 
    }

    ushort get_port_int(){
      return atoi(_port.c_str());
    }

  private:
    std::string _server_name;
    std::string _service_name;
    std::string _ip;
    std::string _port;
};

class Service {
  public:
    Service(){}
    ~Service(){}
    void set_service_name(std::string &service_name){
      _service_name = service_name;
    }
    void set_servers(std::vector<Server> &servers){
      for(int i = 0; i < servers.size(); ++i) {
        add_server(servers[i]);
      }
    }
    void add_server(Server &server){
      _servers.push_back(server); 
    }
  private:
    std::string _service_name;
    std::vector<Server> _servers;
};

class Lookup {
  public:
    static Lookup* get_instance(){
      if(_instance == NULL){
        _mutex->lock();
        if(_instance == NULL){
          _instance = new Lookup();
        }
        _mutex->unlock();
      }
      return _instance;
    }
    void lookup(std::string service_name, std::vector<Server> &servers) {
    
    }
  private:
    static Lookup* _instance;
    static Mutex* _mutex;
};



class ResultHandler{
  public:
    virtual void before_connect() {}
    virtual void data_comeback() {}
};

class Serializable{
  public:
    virtual int serialize(char *, int size) = 0;
    virtual int deserialize(char *, int size) = 0; 
    virtual int bytes() = 0;
};

class Request : public Serializable{
  public:
    Request() {}
    Request(Server &server) : _server(server){}
    virtual ~Request(){}
    void set_server(Server server){
      _server = server;
    }
    
    Server get_server() {
      return _server;
    }

  private:
    Server _server;
};

class Response : public Serializable {
    public:
      Response(){}
      virtual ~Response(){}
      virtual void handle_result(){}
};

class DownstreamHandler : public EventHandler{
  public:
    DownstreamHandler(int sock_fd, 
                      Response *response, 
                      Reactor *reactor, 
                      ResultHandler *result_handler = NULL){
      _sock_fd = sock_fd;
      _response = response;
      _reactor = reactor;
      _result_handler = result_handler;
    }
    
    void handle_read(int fd);
    void handle_write(int fd);
    void handle_close(int fd);
    void handle_timeout(int fd);
    void process_response(int fd);

  private:
    int _sock_fd;
    Response *_response;
    Reactor *_reactor;
    ResultHandler *_result_handler;
};


class UpstreamHandler : public EventHandler{
  public:
    UpstreamHandler(int sock_fd, Reactor *reactor) {
      _sock_fd = sock_fd;
      _reactor = reactor;
    }
    void handle_read(int fd);
    void handle_write(int fd);
    void handle_timeout(int fd);
    void handle_close(int fd);
  private:
    int _alloc(char **buf, int old_size, int new_size) {
      if(*buf == NULL) {
        *buf = new char[new_size];
        return new_size;
      } else {
        if(new_size > old_size) {
          delete[] *buf;
          *buf = new char[new_size];
          return new_size;
        }
      }
      return old_size;
    }

    Reactor *_reactor;
    int _sock_fd;
};

typedef struct {
  int fd;
  int type; // 0:upstream 1:downstream
  DownstreamHandler *response_handler;
  UpstreamHandler *request_handler;
} StreamEvent;

template<class REQUEST, class RESPONSE>
class Processor : public Worker<StreamEvent> {
  public:
    virtual int process(REQUEST &request, RESPONSE &response) = 0;
    
    void run() {
      while(true){
        StreamEvent e = get_event();
        if (e.type == 0) { //上游请求
          char head[4];
          Connection conn(e.fd);
          int payload = conn.recv_n(head, 4);
          if(payload == -1) {
            close(e.fd);
            printf("Error Processor close fd:%d\n", e.fd);
            return;
          }
          
          REQUEST request;
          RESPONSE response;
  
          int size = *((int *)head);
          char *recv_buf = new char[size];
          conn.recv_n(recv_buf, size);
          request.deserialize(recv_buf, size);
          //服务端进行请求处理
          process(request, response);
          size = response.bytes();
          char *send_buf = new char[size + 4];
          memcpy(send_buf, &size, sizeof(int));
          payload = response.serialize(send_buf + 4, size);
          //给客户端返回响应数据
          conn.send_n(send_buf, size + 4);
          //为了正常关闭该链接，需要重新注册回reactor
          UpstreamHandler *upHandler = new UpstreamHandler(e.fd, Reactor::get_instance());
          Reactor::get_instance()->regist(e.fd, upHandler);
          delete recv_buf;
          delete send_buf;
        } else if(e.type == 1) { //下游响应
          //e.response_handler->process_response(e.fd); 
        }
      }
    }

    virtual ~Processor(){}
  private:
};

template <class REQUEST, class RESPONSE>
class ProcessorFactory {
  public:
    virtual Processor<REQUEST, RESPONSE>* make_processor() = 0;
};

template <class REQUEST, class RESPONSE, class PROCESSOR_FACTORY>
class SimpleRpcService {
  public:
    SimpleRpcService(std::string port, std::string &service_name, int thread_num = 10){
      _port = port;
      _service_name = service_name;
      _thread_num = thread_num;
    }
    
    void start(){
      //初始化线程池，处理上游发送的请求
      ThreadPool<StreamEvent> *pool = 
        ThreadPool<StreamEvent>::get_instance();
      PROCESSOR_FACTORY factory;
      
      for(int i = 0; i < _thread_num; ++i) {
        Processor<REQUEST, RESPONSE> *p = factory.make_processor();
        p->join_pool(pool);
        p->start();
      }

      InetAddr addr(_port);
      Acceptor acceptor(addr, Reactor::get_instance());
      
      while(1){
        Reactor::get_instance()->handle_events();
      }
    } 

  private:
    std::string _port;
    std::string _service_name;
    int _thread_num;
};


class DefaultResultHandler : public ResultHandler {
  public:
    DefaultResultHandler (Condition *cond, Mutex *mutex) { 
      _cond = cond;
      _mutex = mutex;
      _finish = false;
    }

    ~DefaultResultHandler () {}
    
    void data_comeback() {
      _finish = true;
      _mutex->lock();
      _cond->notify();
      _mutex->unlock();
    }
    
    void finish() {
      _mutex->lock();
      while(!_finish) {
        _cond->wait(0);
      } 
      _mutex->unlock();
    }
  private:
    Condition *_cond;
    Mutex *_mutex;
    volatile bool _finish;
};

class SimpleRpcClient {
  public:
    static SimpleRpcClient *get_instance(){
      if(_instance == NULL){
        _mutex->lock();
        if(_instance == NULL){
          _instance = new SimpleRpcClient();
          _reactor_thread = new ReactorThread();
          _reactor_thread->start();
          /*
          ThreadPool<StreamEvent> *pool = 
          ThreadPool<StreamEvent>::get_instance();
          PROCESSOR_FACTORY factory;
      
          for(int i = 0; i < 10; ++i) {
            Processor<REQUEST, RESPONSE> *p = factory.make_processor();
            p->join_pool(pool);
            p->start();
          }*/
        }
        _mutex->unlock();
      }
      return _instance;
    }
    
    //异步请求
    int async_request(Server &server, Request *request, Response *response, ResultHandler *handler);
    //同步请求
    int sync_request(Server &server, Request *request, Response *response);     
  private:

    class ReactorThread : public Thread {
      public:
        void run(){
          Reactor::get_instance();
          for(;;){
            Reactor::get_instance()->handle_events();
          }
        }
    };

    SimpleRpcClient() {
    }

    static SimpleRpcClient *_instance;
    static Mutex* _mutex;
    static ReactorThread *_reactor_thread;
};

#endif


