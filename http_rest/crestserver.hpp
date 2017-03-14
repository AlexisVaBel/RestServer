#ifndef CRESTSERVER_H
#define CRESTSERVER_H

#include <string.h>

#include <cpprest/http_listener.h>
#include <cpprest/uri.h>
#include "../http_api/httpserver.hpp"
#include "../factory/httpservregistrator.hpp"



using namespace std;
using namespace utility;
using namespace web::http;
using namespace web::http::experimental::listener;

class CRestServer: public HTTPServer{
public:
    CRestServer();
    ~CRestServer();
    virtual void work();
    virtual void stop();
private:
    bool            m_bWork;
    http_listener   m_listener;
    void handle_get(http_request message);
    void handle_put(http_request message);
    void handle_post(http_request message);
    void handle_delete(http_request message);
    void handle_error(pplx::task<void> &t);

    string getNameFromMes(http_request message);
    void print_Mess(http_request message);
};

#endif // RESTSERVER_H
