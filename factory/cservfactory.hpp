#ifndef CSERVFACTORY_H
#define CSERVFACTORY_H

#include <map>
#include <string>
#include <memory>
#include <functional>
#include "../http_api/ihttpserver.hpp"

using namespace::std;
class CServFactory{
public:
    static CServFactory *Instance();
    shared_ptr<IHTTPServer> Create(string name);
    void    RegisterHTTPServClass(string strName, function<IHTTPServer *(void)> classFactFunct);
private:

    CServFactory();

    CServFactory(CServFactory const&);
    void operator=(CServFactory const&);

    map<string, function<IHTTPServer*(void)>> factoryRegistry;
};

#endif // SERVFACTORY_H
