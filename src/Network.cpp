
/***************************************************************************************
 *
 * Copyright (c) 2016 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: Network.cpp
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 10/14/2016 04:27:23 PM
 * @version: 1.0 
 *   @brief: 
 *  
 **/

#include <stdlib.h>
#include <cstring>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include "Network.h"

InetAddr::InetAddr(std::string port) {
  memset(&_addr, 0x00, sizeof(_addr));
  _addr.sin_family = PF_INET;
  _addr.sin_port = htons(atoi(port.c_str()));
  _addr.sin_addr.s_addr = INADDR_ANY;
}

InetAddr::InetAddr(std::string port, std::string ip) {
  memset(&_addr, 0x00, sizeof(_addr));
  _addr.sin_family = PF_INET;
  _addr.sin_port = htons(atoi(port.c_str()));
  _addr.sin_addr.s_addr = inet_addr(ip.c_str());
}

ushort InetAddr::get_port() {
  return ntohs(_addr.sin_port);
}

uint InetAddr::get_ip_addr() {
  return ntohl(_addr.sin_addr.s_addr);
}

sockaddr InetAddr::addr() const {
  return *(sockaddr *)(&_addr);
}

size_t InetAddr::size() const {
  return sizeof(_addr);
}

Connection::Connection() {
  _sock = -1;
}

Connection::Connection(int sock) {
  _sock = sock;
}

Connection::~Connection() {
}

int Connection::recv_n(char *buf, const int size) {
  if(_sock == -1) {
    return -1;
  }

  int recv_size = 0;
  while(recv_size < size) {
    int n = recv(_sock, buf + recv_size, size - recv_size, MSG_WAITALL);
    if(n == 0) {
      return 1;
    }

    if(n != -1) {
      recv_size += n;
    } else {
      return -1;
    }
  }
  return 0;
}

/*  
int Connection::recv(char *buf, const int size) {
  if(_sock == -1) {
    return -1;
  }
  return read(_sock, buf, size);
}*/

int Connection::send_n(char *buf, const int size) {
  if(_sock == -1) {
    return -1;
  }
  
  int tot = 0;
  while(tot < size) {
    int n = write(_sock, buf + tot, size - tot);
    if(n != -1) {
      tot = tot + n;
    } else {
      return -1;
    }
  }
  return 0;
}

int Connection::send(char *buf, const int size) {
  if(_sock == -1) {
    return -1;
  }
  return write(_sock, buf, size);
}

int Connection::sock() {
  return _sock;
}

void Connection::set_sock(int sock) {
  _sock = sock;
}

void Connection::Close() {
  close(_sock);
}

Connector::Connector(InetAddr &addr) {
  _addr = addr;
}

int Connector::Connect(Connection &conn, int timeout) {
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0){
    return -1;
  }

  //如果timeout=0，默认操作系统超时75s
  if(timeout == 0) {
    struct sockaddr server_addr = _addr.addr();
    if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
      return -1;
    }
    conn.set_sock(sockfd);
    return 0;
  }
  
  fd_set set;
  int error = 0;
  unsigned long ul = 1;
  struct timeval tm;
  int len = sizeof(struct sockaddr);
  struct sockaddr server_addr = _addr.addr();

  ioctl(sockfd, FIONBIO, &ul);
  if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
    tm.tv_usec = timeout * 1000;
    tm.tv_sec = 0;
    FD_ZERO(&set);   
    FD_SET(sockfd, &set);
    if(select(sockfd + 1, NULL, &set, NULL, &tm) == 1) {
      getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, (socklen_t*)&len);
      if(error != 0) {
        return -1;
      }
    }
  }
  ioctl(sockfd, FIONBIO, &ul);
  conn.set_sock(sockfd);
  return 0;
}

