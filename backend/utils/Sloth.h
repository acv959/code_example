#ifndef SLOTH_H
#define SLOTH_H

#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <stack>
#include <map>
#include <sys/time.h>



#define UNUSED(param) (void)param



typedef std::string String;
typedef std::vector <String> vectorS;
typedef std::vector <double> vectorD;

typedef std::stack <String> stackS;

// cетевой интерфейс

static double DELTA_TIME = 0.0;

class NetworkInterface
{

    String  _host,
            _port;

public:
    NetworkInterface(const String &host,const String &port):
        _host(host),_port(port){}
    String  host()const{return _host;}
    String  port()const{return _port;}
    virtual ~NetworkInterface(){}
};


// абстрактные учётные данные
class AbstractCredentials
{

    std::string id,
                name,
                type;

public:
    AbstractCredentials() {}
    AbstractCredentials(const String &id, const String &name,const String &type):
        id(id), name(name), type(type){}
    String getId()const{return id;}
    String getName()const{return name;}
    String getType()const{return type;}
    virtual ~AbstractCredentials() {}

};typedef std::map< String, AbstractCredentials* > MapCredentials;




// список абстрактных учётных данных
class ListAbstractCredentials
{

protected:
    MapCredentials  mapCredentials;

    virtual AbstractCredentials *getCredentials(const String &nameCredentials){

        if (this->ListAbstractCredentials::checkNameCredentials(nameCredentials)) {


            MapCredentials::iterator it;
            it = mapCredentials.find(nameCredentials);
            return (*it).second;
        }

        return NULL;
    }

public:
    ListAbstractCredentials() {}
    bool checkNameCredentials(const String &nameCredentials){

        if (mapCredentials.count(nameCredentials)) {
            return true;
        }

        return false;
    }

    bool addCredentialsToList(String nameCredentials, AbstractCredentials *Credentials){

        mapCredentials.insert(std::pair<String, AbstractCredentials*>(nameCredentials, Credentials));


        return true;
    }

    bool removeFromList(String nameCredentials){

        if (mapCredentials.count(nameCredentials)) {

            MapCredentials::iterator it;
            it = mapCredentials.find(nameCredentials);
            delete (*it).second;
            mapCredentials.erase(it);

            return true;
        }

        return false;
    }

    void clean(){

        for (MapCredentials::iterator it = mapCredentials.begin(); it != mapCredentials.end(); ++it) {

            AbstractCredentials *abstractCredentials = NULL;
            abstractCredentials = (*it).second;

            delete abstractCredentials;

        }
        mapCredentials.clear();
        return;
    }

    MapCredentials  *getMapCredentials(){

        return &mapCredentials;
    }

    virtual ~ListAbstractCredentials(){

        for (MapCredentials::iterator it = mapCredentials.begin(); it != mapCredentials.end(); ++it) {

            AbstractCredentials *abstractCredentials = NULL;
            abstractCredentials = (*it).second;

            delete abstractCredentials;

        }
    }
};


inline void eraseChar(String &str, const char _char){

    if (str.find_last_of(_char)!=String::npos) {

        str.erase(str.find_last_of(_char),1);
    }

    return;
}

inline double getTime(){

    struct timeval time;
    struct timezone tzone;
    std::stringstream date_time;
    gettimeofday(&time, &tzone);
    date_time<<time.tv_sec<<"."<<time.tv_usec;

    double gettime=atof(date_time.str().c_str());

    //printf("gettime =%f",gettime);

    return gettime;
}


inline bool strToBool(const std::string &str){

    if(str == "1" || str == "T" || str == "TRUE" || str == "True" ||
            str == "true" || str == "t"){

        return true ;
    }

    return false;
}

inline  std::string doubleToString(double value){

    char buff[64000] = {0};
    sprintf(buff,"%.4f",value);
    std::string buffer="";
    buffer = std::string (buff);
//    memset(buff,0,64001);
    return buffer;
}

inline  std::string intToString(unsigned int value){

    char buff[64000] = {0};
    sprintf(buff,"%i",value);
    std::string buffer="";
    buffer = std::string (buff);
//    memset(buff,0,64001);
    return buffer;
}

inline std::string getTimeGPQ(){

    double timeGPQ = 0.0;
    if(DELTA_TIME >= 0 ){

        timeGPQ = getTime() - DELTA_TIME;

    }else{

       timeGPQ = getTime() + DELTA_TIME;
    }

    return doubleToString( timeGPQ );
}


template <typename Object>
class AutoRm
{
    Object _object;
public:
    AutoRm(Object object):_object(object) {}
    ~AutoRm() { delete _object; }
private:
    AutoRm(const AutoRm &){}
};




class ArgStr : public std::string{
public:
    ArgStr(const std::string &str):std::string(str) {


    }

    ArgStr &arg(const std::string &a){


        size_t pos = std::string::npos;
        for (size_t  i = 0; i < this->size(); ++i) {

            if(this->at(i) == '%'){


                ++i;
                if(this->at(i) == 's'){

                    pos = --i ;
                    break;
                }else{

                    --i;
                }
            }

        }

        if(pos == std::string::npos)
            return *this;

        this->replace(pos,2,a);

        return *this;
    }


    template <typename ... Args >
    ArgStr &arg(const std::string &a,Args ... args){


        size_t pos = std::string::npos;
        for (size_t  i = 0; i < this->size(); ++i) {

            if(this->at(i) == '%'){


                ++i;
                if(this->at(i) == 's'){

                    pos = --i ;
                    break;
                }else{

                    --i;
                }
            }

        }

        if(pos == std::string::npos)
            return *this;

        this->replace(pos,2,a);
        this->arg(args...);
        return *this;
    }

};



#include <vector>
#include <string>
#include <algorithm>

inline void removeChar( std::string &data, const char &rm){


    if(data.find_first_of(rm)!= std::string::npos){

        data.erase(std::remove(data.begin(),data.end(), rm), data.end());
    }

    return ;
}

#define PRINT_CONSTRUCTOR(classname)                 \
        std::cout<<"\033[32mCONSTRUCTOR\033[0m - "<<#classname<<"()\n";

#define CONSTRUCTOR std::cout<<"\033[32mCONSTRUCTOR\033[0m - "<<__FUNCTION__<<"()\n";

#define PRINT_DESTRUCTOR(classname)                  \
        std::cout<<"\033[31mDESTRUCTOR\033[0m  - ~"<<#classname<<"() \n";

#define DESTRUCTOR std::cout<<"\033[31mDESTRUCTOR\033[0m  - "<<__FUNCTION__<<"() \n";

#define MY_DEBUG(text)                  \
        std::cerr<<#text<<"\n";

#define PRINT_VAR(var)std::cout<<"\t"<<#var<<" = "<< var << "\n";\

#endif // SLOTH_H
