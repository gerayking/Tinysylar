#ifndef __SYLAR_LOG_H
#define __SYLAR_LOG_H

#include <stdarg.h>
#include <stdint.h>

#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include "util.h"
#include "singleton.h"

#define SYLAR_LOG_LEVEL(logger, level)                                \
  if (logger->getLevel() <= level)                                    \
  sylar::LogEventWrap(                                                \
      sylar::LogEvent::ptr(new sylar::LogEvent(                       \
          logger, level, __FILE__, __LINE__, 0, sylar::GetThreadId(), \
          sylar::GetFiberId(), time(0))))                             \
      .getSS()
#define SYLAR_LOG_DEBUG(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::DEBUG)
#define SYLAR_LOG_INFO(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::INFO)
#define SYLAR_LOG_WARN(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::WARN)
#define SYLAR_LOG_ERROR(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::ERROR)
#define SYLAR_LOG_FATAL(logger) SYLAR_LOG_LEVEL(logger, sylar::LogLevel::FATAL)

#define SYLAR_LOG_FMT_LEVEL(logger, level, fmt, ...)                  \
  if (logger->getLevel() <= level)                                    \
  sylar::LogEventWrap(                                                \
      sylar::LogEvent::ptr(new sylar::LogEvent(                       \
          logger, level, __FILE__, __LINE__, 0, sylar::GetThreadId(), \
          sylar::GetFiberId(), time(0))))                             \
      .getEvent()                                                     \
      ->format(fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_DEBUG(logger, fmt, ...) \
  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_INFO(logger, fmt, ...) \
  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::INFO, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_WARN(logger, fmt, ...) \
  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::WARN, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_ERROR(logger, fmt, ...) \
  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::ERROR, fmt, __VA_ARGS__)
#define SYLAR_LOG_FMT_FATAL(logger, fmt, ...) \
  SYLAR_LOG_FMT_LEVEL(logger, sylar::LogLevel::FATAL, fmt, __VA_ARGS__)

#define SYLAR_LOG_ROOT  sylar::LoggerMgr::GetInstance()->getRoot()

namespace sylar {

class Logger;

// ????????????
class LogLevel {
 public:
  enum Level { UNKNOWN = 0, DEBUG = 1, INFO, WARN, ERROR, FATAL };

  static const char *ToString(LogLevel::Level level);

  static const std::vector<std::string> levelString;
};

// ????????????
class LogEvent {
 public:
  typedef std::shared_ptr<LogEvent> ptr;

  LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level,
           const char *file, int32_t line, uint32_t elapse, uint32_t thread_id,
           uint32_t fiber_id, uint64_t time);

  const char *getFileName() const { return m_filename; }

  int32_t getLine() const { return m_line; }

  uint32_t getElapse() const { return m_elapse; }

  uint32_t getThreadId() const { return m_threadId; }

  uint32_t getFiberId() const { return m_fiberId; }

  uint64_t getTime() const { return m_time; }

  std::string getContent() const { return m_ss.str(); }

  std::stringstream &getSS() { return m_ss; }

  std::shared_ptr<Logger> getLogger() const { return m_logger; }

  LogLevel::Level getLevel() { return m_level; }

  void format(const char *fmt, ...);

  void format(const char *fmt, va_list al);

 private:
  const char *m_filename = nullptr;  // ?????????
  int32_t m_line = 0;                // ??????
  uint32_t m_elapse = 0;             // ?????????????????????????????????
  uint32_t m_threadId = 0;           // ??????id
  uint32_t m_fiberId = 0;            // ??????id
  uint64_t m_time = 0;               // ?????????
  std::string m_thread_name;         // ????????????
  std::stringstream m_ss;            // ????????????
  std::shared_ptr<Logger> m_logger;  //
  LogLevel::Level m_level;
};

class LogEventWrap {
 public:
  LogEventWrap(const LogEvent::ptr &mEvent);

  ~LogEventWrap();

  std::stringstream &getSS();

  LogEvent::ptr getEvent();

 private:
  LogEvent::ptr m_event;
};

// ???????????????
class LogFormatter {
 public:
  typedef std::shared_ptr<LogFormatter> ptr;

  LogFormatter(const std::string &pattern);  //??????pattern??????????????????
  std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level,
                     LogEvent::ptr event);

  class FormatItem {
   public:
    typedef std::shared_ptr<FormatItem> ptr;

    virtual ~FormatItem() {}

    virtual void format(std::ostream &os, std::shared_ptr<Logger> logger,
                        LogLevel::Level level, LogEvent::ptr event) = 0;
  };

 private:
  void init();

 private:
  bool m_error = false;
  std::string m_pattern;
  std::vector<FormatItem::ptr> m_items;
};

//???????????????
class LogAppender {
 public:
  typedef std::shared_ptr<LogAppender> ptr;

  virtual ~LogAppender() {}

  virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level,
                   LogEvent::ptr event) = 0;

  void setFormatter(LogFormatter::ptr val) { m_formatter = val; }

  void setLevel(LogLevel::Level level) { m_level = level; }

  void setMLevel(LogLevel::Level mLevel) { m_level = mLevel; }

  LogFormatter::ptr getFormatter() const { return m_formatter; }

 protected:
  LogLevel::Level m_level = LogLevel::DEBUG;
  LogFormatter::ptr m_formatter;
};

//?????????
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
  std::string m_name;                         // ????????????
  LogLevel::Level m_level = LogLevel::DEBUG;  // ????????????
  std::list<LogAppender::ptr> m_appender;     // Appender??????
  LogFormatter::ptr m_formatter;              //
};

class LoggerManager {
 public:
  LoggerManager();

  Logger::ptr getLogger(const std::string &name);
  const std::map<std::string, Logger::ptr> &getLogger() const;
  const Logger::ptr getRoot();
  void init();

 private:
  std::map<std::string, Logger::ptr> m_logger;
  Logger::ptr m_root;
};

//?????????????????????append
class StdoutLogAppender : public LogAppender {
 public:
  typedef std::shared_ptr<StdoutLogAppender> ptr;

  virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level,
                   LogEvent::ptr event) override;

 private:
};

class FileLogAppender : public LogAppender {
 public:
  typedef std::shared_ptr<FileLogAppender> ptr;

  FileLogAppender(const std::string &filename);

  virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level,
                   LogEvent::ptr event) override;

  // reopen file true : success false:fail
  bool reopen();

 private:
  std::string m_filename;
  std::ofstream m_filestream;
};

typedef sylar::Singleton<LoggerManager> LoggerMgr;
};  // namespace sylar
#endif
