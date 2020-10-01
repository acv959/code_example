#include <iostream>
#include <signal.h>
#include <algorithm>
#include <fstream>


#include "InitApp.h"
#include "./utils/Loop.h"

extern bool GLOBAL_NEED_QUIT;


#define HELP "Usage: [ -c <config_file> ] [-d] [ -h ] \n\n\
\t-c - path to config file\n\
\t-d - for run this tool in daemon mode\n\
\t-h - for show this message\n"

#define SHOW_HELP printf("\n\t%s\n\n",HELP);

#define PACKAGE "dcd"
#define localedir_full "/usr/share/locale"

 char* getCmdOption(char **begin, char** end, const std::string &option){
     char **itr = std::find(begin,end,option);

     if (itr != end && ++itr !=end) {
         return *itr;
     }
     return nullptr;
 }

 bool isCmdOptExists(char ** begin, char **end, const std::string &option){

     return std::find(begin,end, option) != end;
 }



int main(int argc, char *argv[]){

    setlocale(LC_ALL,"");

    bindtextdomain(PACKAGE, localedir_full);
    textdomain(PACKAGE);

    std::string config_path = nii_rubin::CONF_PATH+"/"+"da"+"/"+APP_NAME+".xml";

    // do demonize!
    if ( isCmdOptExists(argv, argv+argc, "-d") ) {

        chdir("/");

        pid_t curPID=fork();

        switch(curPID) {
        case 0:
            break;  // in children proccess. ok
        case -1:    // err
            printf("initial fork failed: %s\n",strerror(errno));
            return 1;
            break;
        default:   // in parent proccess. kill him!
            exit(0);
            break;
        }

        // start new session
        if( setsid() < 0 )
              return 1;

        // close file descriptors
        long numFiles = sysconf(_SC_OPEN_MAX);
        for(long i = numFiles-1; i >= 0; --i) {
              close(i);
        }

        // reopen stdio(err)
        int stdioFD = open("/dev/null", O_RDWR);
        dup(stdioFD);
        dup(stdioFD);
    }


    // run with '-h' option
    if ( isCmdOptExists(argv, argv+argc, "-h") ) {
        SHOW_HELP;
        exit(0);
    }

    // run with '-c' option
    if ( isCmdOptExists(argv, argv+argc, "-c") ) {
        if (getCmdOption(argv, argv+argc, "-c")) config_path = std::string( getCmdOption(argv, argv+argc, "-c") );
        else {
            printf(_("config path not set!\n"));
            SHOW_HELP;
            exit(1);
        }
    }


    InitApp initApp;

    if ( ! initApp.initApp(config_path)) {

        return 1;
    }

    Loop().loop();
    return 0;
}
