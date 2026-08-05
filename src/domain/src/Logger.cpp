#include "Logger/Logger.h"

#include <ctime>

Logger::LogLevel Logger::globalLevel = Logger::LogLevel::DEBUG;
bool Logger::timestampEnabled = true;

std::string Logger::levelToString(Logger::LogLevel level)
{
    switch (level)
    {
        case Logger::LogLevel::DEBUG:   return "DEBUG";
        case Logger::LogLevel::INFO:    return "INFO";
        case Logger::LogLevel::WARN:    return "WARN";
        case Logger::LogLevel::ERROR:   return "ERROR";
        default: return "UNKNOWN";
    }
}

std::string Logger::currentTimestamp()
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm tm_info;
    #ifdef _WIN32
        localtime_s(&tm_info, &time_t);
    #else
        localtime_r(&time_t, &tm_info);
    #endif
    
    std::ostringstream ss;
    ss << std::put_time(&tm_info, "%H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

void Logger::parseFormatSpec(std::ostream& out, const std::string& spec)
{
    if (spec.size() >= 3 && spec[0] == ':' && spec[1] == '.')
    {
        std::string digits = spec.substr(2, spec.size() - 3);
        try
        {
            int prec = std::stoi(digits);
            out << std::fixed << std::setprecision(prec);
        }
        catch (...) { }
    }
}
