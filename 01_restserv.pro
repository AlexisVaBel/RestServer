TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG+=c++11


INCLUDEPATH += /opt/casablanca/Release/include/

LIBS+=-L//opt/casablanca/Release/build.Release/Binaries -lcpprest
LIBS+=-lboost_log -lboost_thread -lboost_date_time -lboost_system -lboost_filesystem -lpthread -lcrypto  -lssl


QMAKE_CXXFLAGS += -std=c++11


SOURCES += \
    main.cpp \
    http_api/http_respond.cpp \
    factory/servfactory.cpp \
    http_rest/crestserver.cpp

HEADERS += \
    http_api/http_respond.hpp \
    http_api/httpserver.hpp \
    factory/servfactory.hpp \
    factory/httpservregistrator.hpp \
    loger/logging.h \
    http_rest/crestserver.hpp
