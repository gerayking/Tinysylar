#include <iostream>
#include "../sylar/log.h"

int main(int agc, char** argv){
    sylar::Logger::ptr logger(new sylar::Logger);
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));
    sylar::LogEvent::ptr event(new sylar::LogEvent(__FILE__,__LINE__,0,1,2,time(0)));
    logger->debug(event);
    std::cout<<"Hello sylar";
    return 0;
}