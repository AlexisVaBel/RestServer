#include "http_respond.hpp"

http_respond::http_respond(){    
    m_map.insert(pair<int,string>(200, "OK /r/n"));
    m_map.insert(pair<int,string>(200,"OK/r/n"));
    m_map.insert(pair<int,string>(201,"created"));
    m_map.insert(pair<int,string>(202,"accepted"));
    m_map.insert(pair<int,string>(203,"not authorised"));
    m_map.insert(pair<int,string>(204,"no content"));
    m_map.insert(pair<int,string>(205,"reset content"));
    m_map.insert(pair<int,string>(206,"partial content"));
    m_map.insert(pair<int,string>(400,"bad request"));
    m_map.insert(pair<int,string>(401,"unauthorised"));
    m_map.insert(pair<int,string>(402,"payment required"));
    m_map.insert(pair<int,string>(403,"forbidden"));
    m_map.insert(pair<int,string>(404,"not found"));
    m_map.insert(pair<int,string>(405,"method not allowed"));
    m_map.insert(pair<int,string>(406,"not acceptable"));
    m_map.insert(pair<int,string>(407,"proxy auth required"));
    m_map.insert(pair<int,string>(408,"timeout"));
    m_map.insert(pair<int,string>(500,"server error"));
    m_map.insert(pair<int,string>(501,"not implemented"));
    m_map.insert(pair<int,string>(502,"bad gateway"));
}

