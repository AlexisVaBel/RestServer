#include "cservfactory.hpp"

/**
 * \class cservfactory
 *
 * \ingroup factory
 *
 *
 * \brief Factory for generating httpserver object
 *
 * Singleton itself, for best work needs helper class chttpservregistrator
 * holds servers in map, returning by it`s name
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


CServFactory *CServFactory::Instance(){
    static CServFactory servfactory;
    return &servfactory;
}

CServFactory::CServFactory(){
}

shared_ptr<IHTTPServer> CServFactory::Create(string name){
    IHTTPServer *instance=nullptr;
    auto it=factoryRegistry.find(name);
    if(it!=factoryRegistry.end()){
        instance=it->second();
    }
    if(instance!=nullptr){
        return std::shared_ptr<IHTTPServer>(instance);
    }
    return nullptr;
}

void CServFactory::RegisterHTTPServClass(string strName, function<IHTTPServer *()> classFactFunct){
    factoryRegistry[strName]=classFactFunct;
}
