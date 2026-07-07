#ifndef logger_h
#define logger_h

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

class Logger
{
public:
    enum class LogLevel { DEBUG = 0, INFO = 1, WARN = 2, ERROR = 3 };

    static void setLevel(Logger::LogLevel level) { Logger::globalLevel = level; }
    static Logger::LogLevel getLevel() { return Logger::globalLevel; }

    static void setTimestampEnabled(bool enabled) { Logger::timestampEnabled = enabled; }
    static bool isTimestampEnabled() { return Logger::timestampEnabled; }

    template<typename... Args>
    static void debug(std::string_view format, Args&&... args)
    { Logger::log(Logger::LogLevel::DEBUG, format, std::forward<Args>(args)...); }

    template<typename... Args>
    static void info(std::string_view format, Args&&... args)
    { Logger::log(Logger::LogLevel::INFO, format, std::forward<Args>(args)...); }

    template<typename... Args>
    static void warn(std::string_view format, Args&&... args)
    { Logger::log(Logger::LogLevel::WARN, format, std::forward<Args>(args)...); }

    template<typename... Args>
    static void error(std::string_view format, Args&&... args)
    { Logger::log(Logger::LogLevel::ERROR, format, std::forward<Args>(args)...); }

private:
    static Logger::LogLevel globalLevel;
    static bool timestampEnabled;

    template<typename... Args>
    static void log(Logger::LogLevel level, std::string_view format, Args&&... args)
    {
        if (level < Logger::globalLevel) return;

        std::ostream& out = (level >= Logger::LogLevel::WARN) ? std::cerr : std::cout;
        if (Logger::timestampEnabled) out << "[" << Logger::currentTimestamp() << "] ";
        
        out << "[" << Logger::levelToString(level) << "] ";
        Logger::format(out, format, std::forward<Args>(args)...);
        out << std::endl;
    }

    static std::string levelToString(Logger::LogLevel level);
    static std::string currentTimestamp();

    static void format(std::ostream& out, std::string_view format) { out << format; }
    
    template<typename T, typename... Args>
    static void format(std::ostream& out, std::string_view format, T&& value, Args&&... args);
};

#define LOG_DEBUG(...)   Logger::debug(__VA_ARGS__)
#define LOG_INFO(...)    Logger::info(__VA_ARGS__)
#define LOG_WARN(...)    Logger::warn(__VA_ARGS__)
#define LOG_ERROR(...)   Logger::error(__VA_ARGS__)

#include "Logger.inl"

#endif // logger_h
