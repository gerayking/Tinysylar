#include "log.h"
#include <map>
#include <iostream>
#include <functional>
#include <string.h>
namespace sylar  {
    const std::vector<std::string> LogLevel::levelString = {"UNKNOWN", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
    LogEvent::LogEvent(std::shared_ptr<Logger>logger,LogLevel::Level level,const char* file,int32_t line,uint32_t elapse
            ,uint32_t thread_id,uint32_t fiber_id, uint64_t time)
            :m_logger(logger)
            ,m_level(level)
            ,m_filename(file)
            ,m_line(line)
            ,m_elapse(elapse)
            ,m_threadId(thread_id)
            ,m_fiberId(fiber_id)
            ,m_time(time){}

    void LogEvent::format(const char *fmt, ...) {
        va_list al;
        va_start(al,fmt);
        format(fmt,al);
        va_end(al);
    }

    void LogEvent::format(const char *fmt, va_list al) {
        char *buf = nullptr;
        int len = vasprintf(&buf, fmt, al);
        if(len!=-1){
            m_ss<<std::string(buf,len);
            free(buf);
        }

    }


    const char *LogLevel::ToString(LogLevel::Level level) {
        if (level > LogLevel::levelString.size())return LogLevel::levelString[0].c_str();
        return LogLevel::levelString[level].c_str();
    }
    Logger::Logger(const std::string &name) :
    m_name(name),
    m_level(LogLevel::DEBUG){
        m_formatter.reset(new LogFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n"));
    }

    void Logger::addAppender(LogAppender::ptr appender) {
        if(!appender->getFormatter()){
            appender->setFormatter(m_formatter);
        }
        m_appender.push_back(appender);
    }

    void Logger::delAppender(LogAppender::ptr appender) {
        for (auto it = m_appender.begin();
             it != m_appender.end(); it++) {
            if (*it == appender) {
                m_appender.erase(it);
            }
        }
    }

        void Logger::log(LogLevel::Level level, const LogEvent::ptr &event) {
        if (level >= m_level) {
            auto self = shared_from_this();
            for (auto &i : m_appender) {
                i->log(self,level,event);
            }
        }
    }

    void Logger::debug(LogEvent::ptr event) {
        log(LogLevel::DEBUG, event);
    }

    void Logger::info(LogEvent::ptr event) {
        log(LogLevel::INFO, event);
    }

    void Logger::warn(LogEvent::ptr event) {
        log(LogLevel::WARN, event);
    }

    void Logger::error(LogEvent::ptr event) {
        log(LogLevel::ERROR, event);
    }

    void Logger::fatal(LogEvent::ptr event) {
        log(LogLevel::FATAL, event);
    }

    FileLogAppender::FileLogAppender(const std::string &filename) : m_filename(filename) {
        m_filestream.open(filename);
    }

    void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            m_filestream << m_formatter->format(logger,level,event);
        }
    }

    bool FileLogAppender::reopen() {
        if (m_filestream) {
            m_filestream.close();
        }
        m_filestream.open(m_filename);
        return !m_filestream;
    }

    void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        if (level >= m_level) {
            std::cout << m_formatter->format(logger,level,event);
        }
    }

    LogFormatter::LogFormatter(const std::string &pattern) : m_pattern(pattern) {
        init();
    }

    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        std::stringstream ss;
        for (auto &i : m_items) {
            i->format(ss, logger, level, event);
        }
        return ss.str();
    }

    class MessageFormatItem : public LogFormatter::FormatItem {
    public:
        MessageFormatItem(const std::string &str = "") {}
        void format(std::ostream &ofs, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            ofs << event->getContent();
        }
    };

    class LevelFormatItem : public LogFormatter::FormatItem {
    public:
        LevelFormatItem(const std::string &str = "") {}
        void format(std::ostream &ofs, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            ofs << LogLevel::ToString(level);
        }
    };

    class ElapseFormatItem : public LogFormatter::FormatItem {
    public:
        ElapseFormatItem(const std::string &str = "") {}
        void format(std::ostream &ofs, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            ofs << event->getElapse();
        }
    };

    class StringFormatItem : public LogFormatter::FormatItem {
    public:
        StringFormatItem(const std::string &str = "") : m_string(str) {}
        void format(std::ostream &ofs, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            ofs << m_string;
        }
    private:
        std::string m_string;
    };

    class FiberIdFormatItem : public LogFormatter::FormatItem {
    public:
        FiberIdFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getFiberId();
        }
    };

    class ThreadNameFormatItem : public LogFormatter::FormatItem {
    public:
        ThreadNameFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getThreadId();
        }
    };

    class NameFormatItem : public LogFormatter::FormatItem {
    public:
        NameFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << logger->getName();
        }
    };

    class FileNameFormatItem : public LogFormatter::FormatItem {
    public:
        FileNameFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getFileName();
        }
    };

    class LineFormatItem : public LogFormatter::FormatItem {
    public:
        LineFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << event->getLine();
        }
    };

    class NewLineFormatItem : public LogFormatter::FormatItem {
    public:
        NewLineFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << std::endl;
        }
    };
    class TabFormatItem : public LogFormatter::FormatItem {
    public:
        TabFormatItem(const std::string &str = "") {}

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
            os << "\t";
        }
    };

    class DateTimeFormatItem : public LogFormatter::FormatItem {
    public:
        DateTimeFormatItem(const std::string &format = "%Y-%m-%d %H:%M:%S")
                : m_format(format) {
            if (m_format.empty()) {
                m_format = "%Y-%m-%d %H:%M:%S";
            }
        }

        void format(std::ostream &os, Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override {
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
    //%xxx %xxx{} %%
    void LogFormatter::init() {
        //str, format, type
        std::vector<std::tuple<std::string, std::string, int> > vec;
        std::string nstr;
        for(size_t i = 0; i < m_pattern.size(); ++i) {
            if(m_pattern[i] != '%') {
                nstr.append(1, m_pattern[i]);
                continue;
            }

            if((i + 1) < m_pattern.size()) {
                if(m_pattern[i + 1] == '%') {
                    nstr.append(1, '%');
                    continue;
                }
            }
            size_t n = i + 1;
            int fmt_status = 0;
            size_t fmt_begin = 0;

            std::string str;
            std::string fmt;
            while(n < m_pattern.size()) {
                if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                                   && m_pattern[n] != '}')) {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    break;
                }
                if(fmt_status == 0) {
                    if(m_pattern[n] == '{') {
                        str = m_pattern.substr(i + 1, n - i - 1);
                        fmt_status = 1; //????????????
                        fmt_begin = n;
                        ++n;
                        continue;
                    }
                } else if(fmt_status == 1) {
                    if(m_pattern[n] == '}') {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        fmt_status = 0;
                        ++n;
                        break;
                    }
                }
                ++n;
                if(n == m_pattern.size()) {
                    if(str.empty()) {
                        str = m_pattern.substr(i + 1);
                    }
                }
            }

            if(fmt_status == 0) {
                if(!nstr.empty()) {
                    vec.push_back(std::make_tuple(nstr, std::string(), 0));
                    nstr.clear();
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n - 1;
            } else if(fmt_status == 1) {
                std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
                m_error = true;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
        }

        if(!nstr.empty()) {
            vec.push_back(std::make_tuple(nstr, "", 0));
        }
        static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

                XX(m, MessageFormatItem),           //m:??????
                XX(p, LevelFormatItem),             //p:????????????
                XX(r, ElapseFormatItem),            //r:???????????????
                XX(c, NameFormatItem),              //c:????????????
                XX(t, ThreadNameFormatItem),          //t:??????id
                XX(n, NewLineFormatItem),           //n:??????
                XX(d, DateTimeFormatItem),          //d:??????
                XX(f, FileNameFormatItem),          //f:?????????
                XX(l, LineFormatItem),              //l:??????
                XX(T, TabFormatItem),               //T:Tab
                XX(F, FiberIdFormatItem),           //F:??????id
                XX(N, ThreadNameFormatItem),        //N:????????????
#undef XX
        };

        for(auto& i : vec) {
            if(std::get<2>(i) == 0) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
            } else {
                auto it = s_format_items.find(std::get<0>(i));
                if(it == s_format_items.end()) {
                    m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                    m_error = true;
                } else {
                    m_items.push_back(it->second(std::get<1>(i)));
                }
            }
        }
    }

    LogEventWrap::LogEventWrap(const LogEvent::ptr &mEvent) : m_event(mEvent) {}

    std::stringstream& LogEventWrap::getSS() {
        return m_event->getSS();
    }

    LogEventWrap::~LogEventWrap(){
        m_event->getLogger()->log(m_event->getLevel(), m_event);
    }

    LogEvent::ptr LogEventWrap::getEvent() {
        return m_event;
    }

    Logger::ptr LoggerManager::getLogger(const std::string &name) {
        if(m_logger.find(name)!=m_logger.end())return m_logger[name];
        return m_root;
    }
    LoggerManager::LoggerManager() {
        m_root.reset(new Logger);
        m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));
    }

    void LoggerManager::init() {}
    const std::map<std::string, Logger::ptr> &LoggerManager::getLogger()
        const {
      return m_logger;
    }
    const Logger::ptr LoggerManager::getRoot() { return m_root; }

    }

