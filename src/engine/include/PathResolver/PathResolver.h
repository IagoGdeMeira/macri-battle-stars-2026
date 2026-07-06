#ifndef path_resolver_h
#define path_resolver_h

#include <filesystem>
#include <string>

class PathResolver
{
public:
    static std::filesystem::path resolve(const std::string& path)
    {
        std::filesystem::path input(path);
        if (input.is_absolute()) return input;

        std::filesystem::path current = std::filesystem::current_path();
        while (true)
        {
            std::filesystem::path candidate = current / input;
            if (std::filesystem::exists(candidate)) return candidate;

            if (!current.has_parent_path() || current.parent_path() == current) break;
            current = current.parent_path();
        }

        return input;
    }
};

#endif // path_resolver_h
