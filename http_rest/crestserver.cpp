#include "crestserver.hpp"

#include <string>
#include <pplx/pplx.h>
#include <cpprest/streams.h>
#include <cpprest/filestream.h>
#include <boost/filesystem.hpp>
static CHTTPServRegistrator<CRestServer> register_serv("restserver");
/**
 * \class crestserver
 *
 * \ingroup http_rest
 *
 *
 * \brief Implements ihttserver interface.
 * uses tasks and restful sdk aka 'casablanca'
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




using namespace pplx;

/// \brief api version v1.0, client should acces to server using address:port/api/v1.0
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


/// \details
/// in work method, checks if addres setted and directory exists or can be created
/// creates http_listener and binds with handlers for such http methods as put, get, post, del
/// tries to open and go in forever loop, while m_bWork is setted to false
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


/// \brief get handler, returns file if it exists
/// as file or respond can be large it uses streams
///
/// \bug client can send some request wich raise 'undefined hexadecimal value'


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

/// \brief post not implemented at all, used for stopping server
/// \bug bagdoor, needs real info if post is usefull

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

/// \brief writing file to working directory

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

/// \brief trying to delete file
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

/// \brief helper function which checks for right api and gets filename of request
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

/// \brief helper function which checks if dir exists or can be created
bool CRestServer::isDirExists(){
    INFO("checking dir "+m_strDir);
    boost::filesystem::path dir(m_strDir);
    if(boost::filesystem::is_directory(m_strDir))return true;
    INFO("Creating dir "+m_strDir);
    if(boost::filesystem::create_directory(m_strDir))return true;
    ERR("Can`t create dir"+m_strDir);
    return false;
}

/// \brief helper function which checks if file exists or can be created
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

/// \brief helper function for printing income http message
void CRestServer::printMess(http_request message){
    auto vct=uri::split_path(uri::decode(message.absolute_uri().path()));
    auto vctPnt=vct.begin();
    while(vctPnt!=vct.end()){
        TRACE(*vctPnt);
        vctPnt++;
    }    
}



