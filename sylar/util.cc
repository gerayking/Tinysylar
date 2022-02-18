//
// Created by geray on 2022/2/18.
//
#include "util.h"
#include <pthread.h>
#include <sys/syscall.h>
#include <zconf.h>

namespace sylar{
    pid_t GetThreadId() {
        return syscall(SYS_gettid);
    }

    pid_t GetFiberId() {
        return 0;
    }
//    char* GetThreadName(){
//        char* buffer = new char[32];
//        pthread_getname_np(GetThreadId(),buffer,32);
//        return buffer;
//    }
}
