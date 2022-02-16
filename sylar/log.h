#ifndef __SYLAR_LOG_H
#include <string>
#include <cstdint>
#include <memory>
#include <list>
#include <map>
#include <functional>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#define __SYLAR_LOG_H

#define size_t unsigned int

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
        static const char* ToString(LogLevel::Level level);
        static const std::vector<std::string> levelString;
    };
    const std::vector<std::string> LogLevel::levelString = {"UNKNOWN","DEBUG","INFO","WARN","ERROR","FATAL"};
    // 日志事件
    class LogEvent {
    public:
        size_t t;
        typedef std::shared_ptr <LogEvent> ptr;
        LogEvent(const char* file,int32_t line,uint32_t elapse
        ,uint32_t thread_id,uint32_t fiber_id, uint64_t time);
        const char* getFileName() const {return m_filename;}
        int32_t getLine()const{return m_line;}
        uint32_t  getElapse()const{return m_elapse;}
        uint32_t  getThreadId()const{return m_threadId;}
        uint32_t  getFiberId()const{return m_fiberId;}
        uint64_t getTime()const{return m_time;}
        std::string getContent()const{return m_ss.str();}
        std::stringstream& getSS(){return m_ss;}
    private:
        const char* m_filename = nullptr;   // 文件名
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
        typedef std::shared_ptr <LogFormatter> ptr;
        LogFormatter(const std::string& pattern);//根据pattern来解析出信息
        std::string format(std::shared_ptr<Logger> logger,LogLevel::Level level,LogEvent::ptr event);
        class  FormatItem{
        public:
            typedef std::shared_ptr <FormatItem> ptr;
            virtual ~FormatItem(){}
            virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)=0;
        };

    private:
        void init();
    private:
        std::string m_pattern;
        std::vector<int>v;
        std::vector<FormatItem::ptr> m_items;
    };
    //日志输出器
    class LogAppender {
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        virtual ~LogAppender() {}
        virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) = 0;
        void setFormatter(LogFormatter::ptr val){m_formatter=val;}
        LogFormatter::ptr getFormatter()const {return m_formatter;}
    protected:
        LogLevel::Level m_level;
        LogFormatter::ptr m_formatter;
    };

    //日志器
class Logger : public std::enable_shared_from_this<Logger>{
    public:
        typedef std::shared_ptr <Logger> ptr;

        Logger(const std::string &name = "root");

        void log(LogLevel::Level level, const LogEvent::ptr &event);
        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void error(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);
        void addAppender(LogAppender::ptr appender);
        void delAppender(LogAppender::ptr appender);
        LogLevel::Level getLevel()const{return m_level;}
        void setLevel(LogLevel::Level val){m_level = val;}
        std::string getName(){return m_name;}
    private:
        std::string m_name;         // 日志名称
        LogLevel::Level m_level;    // 日志级别
        std::list<LogAppender::ptr> m_appender;// Appender集合
        LogFormatter::ptr m_formatter; //
    };
    //输出到控制台的append
    class StdoutLogAppender: public LogAppender{
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event) override;

    private:
    };
    class FileLogAppender: public LogAppender{
    public:
        typedef std::shared_ptr <FileLogAppender> ptr;
        FileLogAppender(const std::string& filename);
        virtual void log(std::shared_ptr<Logger> logger,LogLevel::Level level, LogEvent::ptr event)override;
        // reopen file true : success false:fail
        bool reopen();
    private:
        std::string m_filename;
        std::ofstream m_filestream;
    };
    class MessageFormatItem :public LogFormatter::FormatItem{
    public:
        MessageFormatItem(const std::string& str = "") {}
        void format(std::ostream &ofs,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event)override{
            ofs<<event->getContent();
        }
    };
    class LevelFormatItem : public LogFormatter::FormatItem{
    public:
        LevelFormatItem(const std::string& str = "") {}
        void format(std::ostream &ofs,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event)override{
            ofs<<LogLevel::ToString(logger->getLevel());
        } class MessageFormatItem :public LogFormatter::FormatItem{
        public:
            void format(std::ostream &ofs,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event)override{
                ofs<<event->getContent();
            }
        };
    };
    class ElapseFormatItem : public LogFormatter::FormatItem{
    public:
        ElapseFormatItem(const std::string& str = "") {}
        void format(std::ostream &ofs,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event)override{
            ofs<<event->getElapse();
        }
    };
    class StringFormatItem : public LogFormatter::FormatItem{
    public:
        StringFormatItem(const std::string& str = ""):m_string(str){}
        void format(std::ostream &ofs,Logger::ptr logger,LogLevel::Level level,LogEvent::ptr event)override{
            ofs<<m_string;
        }

    private:
        std::string m_string;
    };
    class FiberIdFormatItem : public LogFormatter::FormatItem {
    public:
        FiberIdFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getFiberId();
        }
    };
    class ThreadNameFormatItem : public LogFormatter::FormatItem {
    public:
        ThreadNameFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getThreadId();
        }
    };
    class NameFormatItem : public LogFormatter::FormatItem {
    public:
        NameFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << logger->getName();
        }
    };
    class FileNameFormatItem : public LogFormatter::FormatItem {
    public:
        FileNameFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getFileName();
        }
    };
    class LineFormatItem : public LogFormatter::FormatItem {
    public:
        LineFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getLine();
        }
    };
    class NewLineFormatItem : public LogFormatter::FormatItem {
    public:
        NewLineFormatItem(const std::string& str = "") {}
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << std::endl;
        }
    };
    class DateTimeFormatItem : public LogFormatter::FormatItem {
    public:
        DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
                :m_format(format) {
            if(m_format.empty()) {
                m_format = "%Y-%m-%d %H:%M:%S";
            }
        }
        void format(std::ostream& os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            struct tm tm;
            time_t time = event->getTime();
            localtime_r(&time, &tm);
            char buf[64];
            strftime(buf, sizeof(buf), m_format.c_str(), &tm);
            os << buf;
        }
    private:
        std::string m_format;
    };
};

#endif
