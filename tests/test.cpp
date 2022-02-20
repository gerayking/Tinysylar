#include <iostream>
#include <bits/stdc++.h>
#include "../sylar/log.h"
#include "../sylar/util.h"
using namespace std;
int main() {
//    sylar::Logger::ptr logger(new sylar::Logger);
//    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));
//    sylar::FileLogAppender *errorAppender = new sylar::FileLogAppender("error.log");
//    errorAppender->setFormatter(sylar::LogFormatter::ptr(new sylar::LogFormatter("%d%T%p%T%m%n")));
//    errorAppender->setLevel(sylar::LogLevel::ERROR);
//    logger->addAppender(sylar::FileLogAppender::ptr(errorAppender));
//    SYLAR_LOG_DEBUG(logger)<<"Test DEBUG";
//
//    SYLAR_LOG_INFO(logger)<<"Test INFO";
//    SYLAR_LOG_WARN(logger)<<"Test WARN";
//    SYLAR_LOG_ERROR(logger)<<"Test ERROR";
//    SYLAR_LOG_FATAL(logger)<<"Test FATAL";
//    int a = 10;
//    SYLAR_LOG_FMT_DEBUG(logger,"test macro fmt error %d",a);
    auto l = sylar::LoggerMgr::GetInstance()->getLogger("xx");
    SYLAR_LOG_INFO(l)<<"xxx";
    return 0;
}