#ifndef HTTPSERVREGISTRATOR_H
#define HTTPSERVREGISTRATOR_H

#include <string>
#include "cservfactory.hpp"
#include "../http_api/ihttpserver.hpp"

/**
 * \class сhttpservregistrator
 *
 * \ingroup factory
 *
 *
 * \brief Helper function for factory. In a new implemented server class * use static СHTTPServRegistrator<T> register_serv("string name"),
 *  where T stands for Class implemented on IHTTPServer interface
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
template<class T>
class CHTTPServRegistrator
{
public:
    CHTTPServRegistrator(string strNameServ){
        CServFactory::Instance()->RegisterHTTPServClass(strNameServ,[] (void) ->IHTTPServer * {return new T();});
    }
};

#endif // HTTPSERVREGISTRATOR_H
