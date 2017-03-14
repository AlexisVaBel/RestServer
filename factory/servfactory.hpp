#ifndef SERVFACTORY_H
#define SERVFACTORY_H

#include <map>
#include <string>
#include <memory>
#include <functional>
#include "../http_api/httpserver.hpp"

using namespace::std;
class ServFactory{
public:
    static ServFactory *Instance();
    shared_ptr<HTTPServer> Create(string name);
    void    RegisterHTTPServClass(string strName, function<HTTPServer *(void)> classFactFunct);
private:

    ServFactory();

    ServFactory(ServFactory const&);
    void operator=(ServFactory const&);

    map<string, function<HTTPServer*(void)>> factoryRegistry;
};

#endif // SERVFACTORY_H
