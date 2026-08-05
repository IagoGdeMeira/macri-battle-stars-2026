#ifndef logger_inl
#define logger_inl

#include "Logger.h"

#include <sstream>
#include <iomanip>

template<typename... Args>
void Logger::log(Logger::LogLevel level, std::string_view format, Args&&... args)
{
    if (level < Logger::globalLevel) return;

    std::ostream& out = (level >= Logger::LogLevel::WARN) ? std::cerr : std::cout;
    if (Logger::timestampEnabled) out << "[" << Logger::currentTimestamp() << "] ";

    out << "[" << Logger::levelToString(level) << "] ";
    Logger::format(out, format, std::forward<Args>(args)...);
    out << std::endl;
}

template<typename T, typename... Args>
void Logger::format(std::ostream& out, std::string_view format, T&& value, Args&&... args)
{
    size_t pos = format.find('{');
    if (pos == std::string::npos) { out << format; return; }

    out << format.substr(0, pos);

    size_t end = format.find('}', pos);
    if (end == std::string::npos) { out << format.substr(pos); return; }

    std::string spec(format.substr(pos + 1, end - pos - 1));

    std::ostringstream formatted;
    if (!spec.empty()) Logger::parseFormatSpec(formatted, spec);
    formatted << std::forward<T>(value);
    
    out << formatted.str();

    std::string_view remaining = format.substr(end + 1);
    Logger::format(out, remaining, std::forward<Args>(args)...);
}

#endif // logger_inl
