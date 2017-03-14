#include "servfactory.hpp"


ServFactory *ServFactory::Instance(){
    static ServFactory servfactory;
    return &servfactory;
}

ServFactory::ServFactory(){
}

shared_ptr<HTTPServer> ServFactory::Create(string name){
    HTTPServer *instance=nullptr;
    auto it=factoryRegistry.find(name);
    if(it!=factoryRegistry.end()){
        instance=it->second();
    }
    if(instance!=nullptr){
        return std::shared_ptr<HTTPServer>(instance);
    }
    return nullptr;
}

void ServFactory::RegisterHTTPServClass(string strName, function<HTTPServer *()> classFactFunct){
    factoryRegistry[strName]=classFactFunct;
}
