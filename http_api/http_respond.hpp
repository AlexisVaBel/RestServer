#ifndef HTTP_RESPOND_H
#define HTTP_RESPOND_H

#include <string>
#include <map>

using namespace std;
class http_respond
{
public:
    http_respond();
private:
    map<int,string> m_map;
};

#endif // HTTP_RESPOND_H
