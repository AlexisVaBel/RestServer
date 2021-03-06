#include "factory/cservfactory.hpp"

//
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>

//Реализовать клиент-серверное приложение с использованием библиотеки Casablanca REST API https://casablanca.codeplex.com/.
//Сервер получает команды от клиента и выполняет содержимое этих команд. Требования по выполнению задания:
//1. Возможность сервера запускаться в режиме демона или в режиме обычного приложения.
//2. Авторизация на сервере не требуется.
//3. В сообщении должны содержаться команды следующего характера: копирование, перемещение и удаление файлов.
//4. Клиент принимает команду на вход в виде параметров командной строки.
//5. Сервер, получив команду, выполняет её и возвращает оповещает клиента об успешном (или не очень) выполнении.
//   Результаты выполнения и/или иная отладочная информация выводятся в консоль
//  (или лог файл, если сервер запущен в режиме демона)


/**
 * \class main
 *
 * \ingroup base project file
 *
 *
 * \brief Starts sever by default or using command line arguments
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

//// \brief Structure for getopt
struct globalArgs_t{
    char *name;
    char *host;
    int     port;
    char *dir;
    char isProcess;
} globalArgs;
/// \details
///  n - name
///  h - host
///  p - port
///  d - dir to work
///  t - thread is working as daemon
///  usage ./01_restserv -p 3669 -d "hell" (01_restserv - app name)
///  means start serv on port 3669,  sett(nd)ing working dir to "hell"
///
static const char *optString="n:h:p:d:t:";


//// \brief  filing Structure with getopt, than it will be used to start server
void fillArgs(int opt){
    switch(opt){
    case 'n':
    case 'N':
        globalArgs.name=optarg;
    case 'h':
    case 'H':
        globalArgs.host=optarg;
        break;
    case 'p':
    case 'P':
        globalArgs.port=stoi(optarg);
        break;
    case 'd':
    case 'D':
        globalArgs.dir=optarg;
        break;
    case 't':
    case 'T':
        globalArgs.isProcess=stoi(optarg);
        break;
    }
}

//// \brief  default Structure with getopt, than it will be used to start server
void initArgs(){
    globalArgs.name="restserver";
    globalArgs.host="http://localhost";
    globalArgs.port=30000;
    globalArgs.dir="playground";
    globalArgs.isProcess=0;
}

//// \brief  if app started as daemon, logginh info to file
void setLoggingForDaemon(){
    boost::log::add_file_log("./resterv.log");
    boost::log::core::get()->set_filter(
                boost::log::trivial::severity >=boost::log::trivial::info
            );
}

int main(int argc,char **argv){
    initArgs();
    int opt=0;
    do{
        opt=getopt(argc,argv,optString);
        fillArgs(opt);
    }while(opt!=-1);

//// \brief creating serv with factory
    auto serv=CServFactory::Instance()->Create(globalArgs.name);
    if(serv==nullptr)    return EXIT_FAILURE;
    serv->setAddress(globalArgs.host,globalArgs.port,globalArgs.dir);

//// \brief unix way creating daemon
    if(globalArgs.isProcess==1){

        setLoggingForDaemon();
        pid_t pid,sid;
        pid=fork();
        if(pid<0){exit(EXIT_FAILURE);}
        if(pid>0){exit (EXIT_SUCCESS);}

        umask(0);
        sid=setsid();
        if(sid<0) {exit (EXIT_FAILURE);}
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    };

    serv->work();
    return EXIT_SUCCESS;
}
