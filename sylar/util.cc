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
}
