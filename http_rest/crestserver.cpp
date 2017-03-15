#include "crestserver.hpp"

#include <string>
#include <pplx/pplx.h>
#include <cpprest/streams.h>
#include <cpprest/filestream.h>
#include <boost/filesystem.hpp>



static HTTPServRegistrator<CRestServer> register_serv("restserver");

using namespace pplx;

namespace RSTSVR1{
    const string STR_APINAME="api";
    const string STR_APIVERS="v1.0";    
}


CRestServer::CRestServer():m_bWork(true){
    INFO("restserver created");
}

CRestServer::~CRestServer(){
    INFO("restserver deleted");
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
        ERR("WORK: no port setted");
        return;
    };

    if(!isDirExists()){
        ERR("WORK: no working dir, exit");
        return;
    }

    string_t str_tAddr=m_strAddr.append(":").append(std::to_string(m_iPort));
    m_listener= http_listener(str_tAddr);
    m_listener.support(methods::GET,std::bind(&CRestServer::handleGet,this,std::placeholders::_1));
    m_listener.support(methods::PUT,std::bind(&CRestServer::handlePut,this,std::placeholders::_1));
    m_listener.support(methods::DEL,std::bind(&CRestServer::handleDelete,this,std::placeholders::_1));
    m_listener.support(methods::POST,std::bind(&CRestServer::handlePost,this,std::placeholders::_1));

    TRACE("WORK: listnening");
    TRACE(m_strAddr.c_str());

    try{
        m_listener.open().wait();
    }catch (const std::system_error& e){
        ERR((e.what()));
        return;
    }

    while(m_bWork){
        usleep(20);
    }

    try{
        m_listener.close().wait();
    }catch(std::system_error &err){
        ERR(err.what());
    }
}

void CRestServer::stop(){
    m_bWork=false;
}


//!throws:
//! 1. undefined hexadecimal value
//!
void CRestServer::handleGet(http_request message){
    INFO("get handle");

    string strName=getNameFromMes(message);
    string strToGet="";

    if(strName.empty()){
        ERR("GET: no filename");
        message.reply(status_codes::BadRequest,U("no filename"));
        return;
    }else{
        strToGet=m_strDir+"/"+strName;
    }    

    if(!isFileExists(strToGet.c_str())){
        ERR("GET: no file exists "+strName);
        message.reply(status_codes::BadRequest,U("no file"));
        return;
    }


    concurrency::streams::file_stream<unsigned char>::open_istream(strToGet).then([=]
        (task<concurrency::streams::basic_istream<unsigned char>>  prevTask){
        try {            
            auto fileStream=prevTask.get(); // got ready file stream
            http_response resp;             // prepare response with OK status
            resp.set_body(fileStream);
            resp.set_status_code(status_codes::OK);
            return message.reply(resp).then([fileStream,message,strName](task<void> prevResp){
                try {
                    fileStream.close();
                    INFO("GET: "+strName+" file send to client");
                    if(prevResp.is_done())message.reply(status_codes::OK);
                } catch (const std::system_error& e) {
                    ERR("GET: file stream err");
                    ERR(e.what());
                    message.reply(status_codes::BadRequest);
                }
            });

        } catch (const std::system_error& e) {
            ERR("GET: task err");
            ERR(e.what());
            return pplx::task_from_result();
        }
    }).wait();
}

void CRestServer::handlePost(http_request message){
    INFO("got post handle");
    INFO("shutting down serv");
    //?using post as delete,
    //?using post as put method,
    //?using post as append method
    //?using post to shutdown server, buhhhahaaha-ha-ha, BIG-bug-BackDoor
    message.reply(status_codes::OK,U("Post is not used")).then([=](task<void> readyTask){
        stop();
    });

}

void CRestServer::handlePut(http_request message){
    INFO("got put handle");

    string strName=getNameFromMes(message);
    string strToPut=m_strDir+"/"+strName;
    message.extract_vector().then([=](std::vector<unsigned char> vectCh){
            ofstream outfile(strToPut.data(), ios::out | ios::binary);
            try{
                INFO("PUT: written file "+strName);
                outfile.write((const char*)&vectCh[0], vectCh.size());
                outfile.close();                
                message.reply(status_codes::OK,U("Written file "+strName));
            }catch(...){
                ERR("PUT: unable to write file "+strName);
                message.reply(status_codes::BadRequest,U("unable to write"+strName));
            }
        }).wait();
}

void CRestServer::handleDelete(http_request message){
    INFO("got del handle");

    string strToDel="";
    string strName=getNameFromMes(message);
    if(strName.empty()){
        ERR("DEL: no file name "+strName);
        message.reply(status_codes::BadRequest,U("no filename"));
        return;
    }else{
        strToDel=m_strDir+"/"+strName;
    }

    if(remove(strToDel.data())!=0){
        ERR("DEL: no such file "+strName);
        message.reply(status_codes::BadRequest,U(strName.append(" no such file")));
    }else{
        INFO("DEL: file "+strName+" deleted");
        message.reply(status_codes::OK,U(strName.append(" deleted")));
    }
}


string CRestServer::getNameFromMes(http_request message){
    auto vct=uri::split_path(uri::decode(message.absolute_uri().path()));
    auto vctPnt=vct.begin();
    bool okAPI=false;
    bool okVERS=false;
    while(vctPnt!=vct.end()){
        if(okAPI&&okVERS)return (*vctPnt);
        if((*vctPnt).compare(RSTSVR1::STR_APINAME)==0)  okAPI=true;
        if((*vctPnt).compare(RSTSVR1::STR_APIVERS)==0)  okVERS=true;
        vctPnt++;
    }
    return "";
}

bool CRestServer::isDirExists(){
    INFO("checking dir "+m_strDir);
    boost::filesystem::path dir(m_strDir);
    if(boost::filesystem::is_directory(m_strDir))return true;
    INFO("Creating dir "+m_strDir);
    if(boost::filesystem::create_directory(m_strDir))return true;
    ERR("Can`t create dir"+m_strDir);
    return false;
}

bool CRestServer::isFileExists(const char *chName){
    if(boost::filesystem::exists(chName)){
       if(boost::filesystem::is_regular_file(chName)) return true;
    };
    return false;

//    if (FILE *file = fopen(chName, "r")) {
//        fclose(file);
//        return true;
//    } else {
//        return false;
//    }
}


void CRestServer::printMess(http_request message){
    auto vct=uri::split_path(uri::decode(message.absolute_uri().path()));
    auto vctPnt=vct.begin();
    while(vctPnt!=vct.end()){
        TRACE(*vctPnt);
        vctPnt++;
    }    
}



