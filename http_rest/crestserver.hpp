#ifndef CRESTSERVER_H
#define CRESTSERVER_H

#include <string.h>
#include <cpprest/uri.h>
#include <cpprest/http_listener.h>


#include "../http_api/ihttpserver.hpp"
#include "../factory/chttpservregistrator.hpp"


using namespace std;
using namespace utility;
using namespace web::http;
using namespace web::http::experimental::listener;

class CRestServer: public IHTTPServer{
public:
    CRestServer();
    ~CRestServer();
    virtual void work();
    virtual void stop();
private:
    bool            m_bWork;
    http_listener   m_listener;

    void handleGet(http_request message);
    void handlePut(http_request message);
    void handlePost(http_request message);
    void handleDelete(http_request message);
//    void handleError(pplx::task<void> &t);

    string  getNameFromMes(http_request message);
    bool    isDirExists();
    bool    isFileExists(const char* chName);
    void    printMess(http_request message);
};

#endif // RESTSERVER_H
