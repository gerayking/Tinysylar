#include "log.h"

namespace sylar {
    LogEvent::LogEvent(const char* file,int32_t line,uint32_t elapse
            ,uint32_t thread_id,uint32_t fiber_id, uint64_t time)
            :m_filename(file),m_line(line),m_elapse(elapse),m_threadId(thread_id)
            ,m_fiberId(fiber_id),m_time(time){}

    const char *LogLevel::ToString(LogLevel::Level level) {
        if (level > LogLevel::levelString.size())return LogLevel::levelString[0].c_str();
        return LogLevel::levelString[level].c_str();
    }

    Logger::Logger(const std::string &name) : m_name(name),m_level(LogLevel::DEBUG){
        m_formatter.reset(new LogFormatter("%d [%p] %f %l %m %n"));
    };

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
            for (auto &i : m_appender) {
                i->log(std::shared_ptr<Logger>(this),level,event);
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

    FileLogAppender::FileLogAppender(const std::string &filename) : m_filename(filename) {}

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

    LogFormatter::LogFormatter(const std::string &pattern) : m_pattern(pattern) {}

    std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
        std::stringstream ss;
        for (auto &i : m_items) {
            i->format(ss, logger, level, event);
        }
        return ss.str();
    }

    //%xxx %xxx{} %%
    //
    void LogFormatter::init() {
        //str format type
        std::vector<std::tuple<std::string, std::string, int> > vec;
        std::string nstr;
        for (size_t i = 0; i < m_pattern.size(); ++i) {
            if (m_pattern[i] != '%') {
                nstr.append(1, m_pattern[i]);
                continue;
            }
            if (i + 1 < m_pattern.size()) {
                if (m_pattern[i + 1] == '%') {
                    nstr.append(1, '%');
                    continue;
                }
            }
            size_t n = i + 1;
            int fmt_status = 0;
            size_t fmt_begin = 0;
            std::string str;
            std::string fmt;
            while (n < m_pattern.size()) {
                if (isspace(m_pattern[n])) {
                    break;
                }
                if (fmt_status == 0) {
                    if (m_pattern[n] == '{') {
                        str = m_pattern.substr(i + 1, n - i);
                        fmt_status = 1;
                        fmt_begin = n;
                        ++n;
                    }
                }
                if (fmt_status == 1) {
                    if (m_pattern[n] == '}') {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin);
                        fmt_status = 2;
                        ++n;
                        continue;
                    }
                }
            }
            if (fmt_status == 0) {
                str = m_pattern.substr(i + 1, n - i - 1);
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n;
            } else if (fmt_status == 1) {
                std::cout << "pattern parse error" << m_pattern << " - " << m_pattern.substr(i) << std::endl;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            } else if (fmt_status == 2) {
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n;
            }
            if (!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, "", 0));
            }
            /* %m -- 消息体
             * %p -- level
             * %r -- 启动后的时间       -- time is used by start
             * %c -- 日志名称          -- log name
             * %n -- 回车换行          --
             * %d -- 时间             -- time
             * %f -- 文件名称          -- filename
             * %l -- 行号             -- number of row
             * */
            static std::map<std::string, std::function<FormatItem::ptr(const std::string &str)> > s_format_item = {
#define XX(str, C) \
{#str,[](const std::string fmt){return FormatItem::ptr(new C(fmt));}}
                    XX(m, MessageFormatItem),
                    XX(P, LevelFormatItem),
                    XX(r, ElapseFormatItem),
                    XX(c, NameFormatItem),
                    XX(t, ThreadNameFormatItem),
                    XX(n, NewLineFormatItem),
                    XX(d, DateTimeFormatItem),
                    XX(f, FileNameFormatItem),
                    XX(l, LineFormatItem)
#undef XX
            };
            for(auto &item : vec){
                if(std::get<2>(item) == 0){
                    m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(item))));
                }else{
                    auto it = s_format_item.find(std::get<0>(item));
                    if(it == s_format_item.end()){
                        m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(item) + ">>")));
                    }else{
                        m_items.push_back(it->second(std::get<1>(item)));
                    }
                }
                std::cout << std::get<0>(item) << " - " << std::get<1>(item) << std::get<2>(item) << std::endl;
            }

        }
    }


}