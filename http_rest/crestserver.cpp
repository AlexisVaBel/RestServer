#include "crestserver.hpp"
#include <fstream>
#include <functional>
#include <pplx/pplx.h>

#include <cpprest/streams.h>
#include <cpprest/filestream.h>
#include <string>


static HTTPServRegistrator<CRestServer> register_serv("restserver");

using namespace pplx;

namespace RESTSERV{    
    const string STR_APINAME="api";
    const string STR_APIVERS="v1.0";
    const string FILE_DIR="./playground/";
}


CRestServer::CRestServer():m_bWork(true){
    TRACE("restserver created");
}

CRestServer::~CRestServer(){
    TRACE("restserver deleted");
}


// from article "bringing  restfull service" <bel>
//!Instead of continuously checking for is_done, it’s better to use the then function.
//It relies on attaching a handler function to the task.
//It should be easily identifiable for developers using the PPL tasks used for programming Windows Runtime (WinRT)
//asynchronous operations.
//!The handler for the then function is invoked only after the task is completed
//The handler function passed to the then function should take an argument either of type T or task<T>.

// this matters, i haven`t it <bel>
//Using the argument of type task<T> bestows an additional benefit:
//It’s the only way to catch exceptions raised by the task operation itself!

void CRestServer::work(){
    if (this->m_strAddr.compare("")==0){
        TRACE("no port setted");
        return;
    };

    string_t str_tAddr=m_strAddr.append(":").append(std::to_string(m_iPort));
    m_listener= http_listener(str_tAddr);
    m_listener.support(methods::GET,std::bind(&CRestServer::handle_get,this,std::placeholders::_1));
    m_listener.support(methods::PUT,std::bind(&CRestServer::handle_put,this,std::placeholders::_1));
    m_listener.support(methods::DEL,std::bind(&CRestServer::handle_delete,this,std::placeholders::_1));
    m_listener.support(methods::POST,std::bind(&CRestServer::handle_post,this,std::placeholders::_1));

    TRACE("listnening");
    TRACE(m_strAddr.c_str());

    try{
        m_listener.open().wait();
    }catch (const std::system_error& e){
        TRACE(e.what());
        return;
    }

    while(m_bWork){

    }

    try{
        m_listener.close().wait();
    }catch(std::system_error &err){
        TRACE(err.what());
    }
}

void CRestServer::stop(){
    m_bWork=false;
}


//!throws:
//! 1. undefined hexadecimal value
//!
void CRestServer::handle_get(http_request message){
    TRACE("get handle");

    string strName=getNameFromMes(message);
    string strToGet="";

    if(strName.empty()){
        message.reply(status_codes::BadRequest,U("no filename"));
        return;
    }else{
        strToGet=RESTSERV::FILE_DIR+strName;
    }


    // to stand alone class - file procer or smth else <bel>
    if (FILE *file = fopen(strToGet.c_str(), "r")) {
        fclose(file);
    } else {
        message.reply(status_codes::BadRequest,U("no file"));
        return;
    }
    // to stand alone class - file procer

    concurrency::streams::file_stream<unsigned char>::open_istream(strToGet).then([=]
        (task<concurrency::streams::basic_istream<unsigned char>>  prevTask){
        try {
            auto fileStream=prevTask.get(); // got ready file stream

            http_response resp;             // prepare response with OK status
            resp.set_body(fileStream);
            resp.set_status_code(status_codes::OK);
            return message.reply(resp).then([fileStream,message](task<void> prevResp){
                fileStream.close();
                try {
                    if(prevResp.is_done())message.reply(status_codes::OK);
                } catch (...) {
                    message.reply(status_codes::BadRequest);
                }
            });

        } catch (...) {
            return pplx::task_from_result();
        }
    }).wait();
}

void CRestServer::handle_post(http_request message){
    TRACE("got post handle");

    print_Mess(message);    
    //using post as delete,
    //using post as put method,
    //using post as append method
    //using post to shutdown server, ha-ha-ha - that bug was in our java-plc
    message.reply(status_codes::OK,U("responded,Post"));
}

void CRestServer::handle_put(http_request message){
    TRACE("got put handle");

    string strName=getNameFromMes(message);
    string strToPut=RESTSERV::FILE_DIR+strName;
    message.extract_vector().then([=](std::vector<unsigned char> vectCh){
            ofstream outfile(strToPut.data(), ios::out | ios::binary);
            try{
                outfile.write((const char*)&vectCh[0], vectCh.size());
                outfile.close();
                message.reply(status_codes::OK,U("Written file "+strName));
            }catch(...){
                message.reply(status_codes::BadRequest,U("unable to write"+strName));
            }
        }).wait();
}

void CRestServer::handle_delete(http_request message){
    TRACE("got del handle");    

    string strToDel="";
    string strName=getNameFromMes(message);
    if(strName.empty()){
        message.reply(status_codes::BadRequest,U("no filename"));
        return;
    }else{
        strToDel=RESTSERV::FILE_DIR+strName;
    }

    if(remove(strToDel.data())!=0){
        message.reply(status_codes::BadRequest,U(strName.append(" no such file")));
    }else
        message.reply(status_codes::OK,U(strName.append(" deleted")));
}


string CRestServer::getNameFromMes(http_request message){
    auto vct=uri::split_path(uri::decode(message.absolute_uri().path()));
    auto vctPnt=vct.begin();
    bool okAPI=false;
    bool okVERS=false;
    while(vctPnt!=vct.end()){
        TRACE(*vctPnt);
        if(okAPI&&okVERS)return (*vctPnt);
        if((*vctPnt).compare(RESTSERV::STR_APINAME)==0)  okAPI=true;
        if((*vctPnt).compare(RESTSERV::STR_APIVERS)==0)  okVERS=true;
        vctPnt++;
    }
    return "";
}


void CRestServer::print_Mess(http_request message){
    auto vct=uri::split_path(uri::decode(message.absolute_uri().path()));
    auto vctPnt=vct.begin();
    while(vctPnt!=vct.end()){
        TRACE(*vctPnt);
        vctPnt++;
    }    
}



void CRestServer::handle_error(pplx::task<void> &t){
    try{
        t.get();
    }
    catch(...){

    }
}

