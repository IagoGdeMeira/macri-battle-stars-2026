#ifndef logger_inl
#define logger_inl

#include "Logger.h"

template<typename T, typename... Args>
void Logger::format(std::ostream& out, std::string_view format, T&& value, Args&&... args)
{
    size_t pos = format.find("{}");
    if (pos != std::string::npos)
    {
        out << format.substr(0, pos);
        out << value;
        std::string_view remaining = format.substr(pos + 2);
        Logger::format(out, remaining, std::forward<Args>(args)...);
    } else out << format;
}

#endif // logger_inl
