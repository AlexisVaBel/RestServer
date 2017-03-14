#ifndef HTTPSERVREGISTRATOR_H
#define HTTPSERVREGISTRATOR_H

#include <string>
#include "servfactory.hpp"
#include "../http_api/httpserver.hpp"

using namespace std;
template<class T>
class HTTPServRegistrator
{
public:
    HTTPServRegistrator(string strNameServ){
        ServFactory::Instance()->RegisterHTTPServClass(strNameServ,[] (void) ->HTTPServer * {return new T();});
    }
};

#endif // HTTPSERVREGISTRATOR_H
