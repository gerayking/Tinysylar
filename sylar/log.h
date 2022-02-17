#ifndef __SYLAR_LOG_H
#define __SYLAR_LOG_H

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <map>

namespace sylar {

    class Logger;

    // 日志级别
    class LogLevel {
    public:
        enum Level {
            UNKNOWN = 0,
            DEBUG = 1,
            INFO,
            WARN,
            ERROR,
            FATAL
        };

        static const char *ToString(LogLevel::Level level);

        static const std::vector<std::string> levelString;
    };


    // 日志事件
    class LogEvent {
    public:
        ~LogEvent(){std::cout<<"destory LogEvent"<<std::endl;}
        typedef std::shared_ptr<LogEvent> ptr;

        LogEvent(int32_t line);
        LogEvent(const char *file, int32_t line, uint32_t elapse, uint32_t thread_id, uint32_t fiber_id, uint64_t time);

        const char *getFileName() const { return m_filename; }

        int32_t getLine() const { return m_line; }

        uint32_t getElapse() const { return m_elapse; }

        uint32_t getThreadId() const { return m_threadId; }

        uint32_t getFiberId() const { return m_fiberId; }

        uint64_t getTime() const { return m_time; }

        std::string getContent() const { return m_ss.str(); }

        std::stringstream &getSS() { return m_ss; }

    private:
        const char *m_filename = nullptr;   // 文件名
        int32_t m_line = 0;             // 行号
        uint32_t m_elapse = 0;         // 程序启动到现在的毫秒数
        uint32_t m_threadId = 0;        // 线程id
        uint32_t m_fiberId = 0;         // 协程id
        uint64_t m_time = 0;            // 时间戳
        std::stringstream m_ss;          // 日志内容
    };

    // 日志格式器
    class LogFormatter {
    public:
        typedef std::shared_ptr<LogFormatter> ptr;

        LogFormatter(const std::string &pattern);//根据pattern来解析出信息
        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

        class FormatItem {
        public:
            typedef std::shared_ptr<FormatItem> ptr;

            virtual ~FormatItem() {}

            virtual void
            format(std::ostream &os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
        };

    private:
        void init();

    private:
        bool m_error = false;
        std::string m_pattern;
        std::vector<FormatItem::ptr> m_items;
    };

    //日志输出器
    class LogAppender {
    public:
        typedef std::shared_ptr<LogAppender> ptr;

        virtual ~LogAppender() {}

        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

        void setFormatter(LogFormatter::ptr val) { m_formatter = val; }

        LogFormatter::ptr getFormatter() const { return m_formatter; }

    protected:
        LogLevel::Level m_level = LogLevel::DEBUG;
        LogFormatter::ptr m_formatter;
    };

    //日志器
    class Logger : public std::enable_shared_from_this<Logger> {
    public:
        typedef std::shared_ptr<Logger> ptr;

        Logger(const std::string &name = "root");

        void log(LogLevel::Level level, const LogEvent::ptr &event);

        void debug(LogEvent::ptr event);

        void info(LogEvent::ptr event);

        void warn(LogEvent::ptr event);

        void error(LogEvent::ptr event);

        void fatal(LogEvent::ptr event);

        void addAppender(LogAppender::ptr appender);

        void delAppender(LogAppender::ptr appender);

        LogLevel::Level getLevel() const { return m_level; }

        void setLevel(LogLevel::Level val) { m_level = val; }

        std::string getName() { return m_name; }

    private:
        std::string m_name;         // 日志名称
        LogLevel::Level m_level = LogLevel::DEBUG; // 日志级别
        std::list<LogAppender::ptr> m_appender;// Appender集合
        LogFormatter::ptr m_formatter; //
    };

    //输出到控制台的append
    class StdoutLogAppender : public LogAppender {
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
    private:
    };
    class FileLogAppender : public LogAppender {
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        FileLogAppender(const std::string &filename);
        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
        // reopen file true : success false:fail
        bool reopen();
    private:
        std::string m_filename;
        std::ofstream m_filestream;
    };

};
#endif
