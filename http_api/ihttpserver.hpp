#ifndef IHTTPSERVER_H
#define IHTTPSERVER_H

#include <string>
#include <iostream>
#include "../loger/logging.h"

/**
 * \class ihttpserver
 *
 * \ingroup http_api
 *
 *
 * \brief Interface class for httpservers. Basic parameters - m_iPort -port for server, m_strAddr -address for server, m_strDir-working dir
 *
 * \note
 *
 * \author A.Beljaev
 *
 * \version $Revision: 1.0 $
 *
 * \date $Date: 2017/04/18 10:00 $
 *
 * Contact: alexisvabel@gmail.com
 *
 *
 */


using namespace std;
class IHTTPServer
{
public:
    IHTTPServer(){
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
