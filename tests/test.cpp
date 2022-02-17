#include <iostream>
#include "../sylar/log.h"
int main(){
    sylar::LogEvent event(2);
    std::cout<<"hello sylay log"<<std::endl;
    std::cout<<event.getLine()<<std::endl;
    return 0;
}