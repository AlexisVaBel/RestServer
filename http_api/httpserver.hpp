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
    }    
    virtual void setAddress(string strAddr,int iPort,string strDir){
        m_iPort     =iPort;
        m_strAddr   =strAddr;
        m_strDir=strDir;        
    }
    virtual void work()=0;
    virtual void stop()=0;
protected:
    int     m_iPort;
    string  m_strAddr;
    string  m_strDir;


};

#endif // HTTPSERVER_H
