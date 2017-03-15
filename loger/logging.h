#ifndef LOGGING_H
#define LOGGING_H


#define BOOST_LOG_DYN_LINK 1


#include <boost/log/trivial.hpp>


//#define TRACE(msg)            std::cout << msg <<std::endl


#define TRACE(msg)            BOOST_LOG_TRIVIAL(trace)<< msg
#define INFO(msg)             BOOST_LOG_TRIVIAL(info) << msg
#define ERR(msg)              BOOST_LOG_TRIVIAL(error)<< msg



#endif // LOGGING_H

