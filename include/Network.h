
/***************************************************************************************
 *
 * Copyright (c) 2014 Lejent, Inc. All Rights Reserved
 *
 **************************************************************************************/
/**
 *    @file: Network.h
 *  @author: zhangxingbiao(xingbiao.zhang@lejent.com)
 *    @date: 09/20/2016 11:46:20 AM
 * @version: 1.0 
 *   @brief: 
 *  
 **/
#ifndef __NET_WORK_H__
#define __NET_WORK_H__

#include <netinet/in.h>
#include <string>
#include "Types.h"

class InetAddr {
  public:
    InetAddr(){}
    InetAddr(std::string port);
    InetAddr(std::string port, std::string addr);
    ushort get_port();
    uint get_ip_addr();
    sockaddr addr() const;
    size_t size() const;
  private:
    sockaddr_in _addr;
};

class Connection {
  public:
    friend class Connector;
    Connection();
    Connection(int sock);
    ~Connection();
    //int recv(char *buf, int size);
    int send(char *buf, int size);
    int recv_n(char *buf, int size);
    int send_n(char *buf, int size);
    int sock();
    void Close();
  private:
    void set_sock(int sock);
    int _sock;
};

class Connector{
  public:
    Connector(InetAddr &addr);
    int Connect(Connection &conn, int timeout = 0);
  private:
    InetAddr _addr;
};

#endif

