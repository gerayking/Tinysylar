#include <iostream>
#include "../sylar/log.h"
#include "../sylar/util.h"
int main() {
    sylar::Logger::ptr logger(new sylar::Logger);
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));
    SYLAR_LOG_DEBUG(logger)<<"Test DEBUG";
    SYLAR_LOG_INFO(logger)<<"Test INFO";
    SYLAR_LOG_WARN(logger)<<"Test WARN";
    SYLAR_LOG_ERROR(logger)<<"Test ERROR";
    SYLAR_LOG_FATAL(logger)<<"Test FATAL";
    return 0;
}