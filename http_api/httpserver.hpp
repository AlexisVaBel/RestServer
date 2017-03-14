#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <string>
#include <iostream>
#include <loger/logging.h>

using namespace std;
class HTTPServer
{
public:
    HTTPServer(){
        TRACE("http ancestor created");
    }    
    virtual void setAddress(string strAddr,int iPort){
        m_iPort=iPort;
        m_strAddr=strAddr;
        TRACE("address to listen ");
        TRACE(strAddr);
    }
    virtual void work()=0;
    virtual void stop()=0;
protected:
    int     m_iPort;
    string  m_strAddr;


};

#endif // HTTPSERVER_H
