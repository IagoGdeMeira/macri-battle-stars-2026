#ifndef data_node_h
#define data_node_h

#include <memory>
#include <string>
#include <vector>

class DataNode
{
public:
    virtual ~DataNode() = default;

    virtual bool has(const std::string& key) const = 0;

    inline static const std::string defaultStringFallback{};
    inline static constexpr int defaultIntFallback = 0;
    inline static constexpr float defaultFloatFallback = 0.f;
    inline static constexpr bool defaultBoolFallback = false;

    virtual std::string getString(const std::string& key, const std::string& fallback = defaultStringFallback) const = 0;
    virtual int getInt(const std::string& key, const int& fallback = defaultIntFallback) const = 0;
    virtual float getFloat(const std::string& key, const float& fallback = defaultFloatFallback) const = 0;
    virtual bool getBool(const std::string& key, const bool& fallback = defaultBoolFallback) const = 0;
    virtual std::vector<std::unique_ptr<DataNode>> getArray(const std::string& key) const = 0;
    virtual std::unique_ptr<DataNode> getObject(const std::string& key) const { (void)key; return nullptr; }

    virtual void setString(const std::string& key, const std::string& value) { (void)key; (void)value; }
    virtual void setInt(const std::string& key, int value) { (void)key; (void)value; }
    virtual void setFloat(const std::string& key, float value) { (void)key; (void)value; }
    virtual void setBool(const std::string& key, bool value) { (void)key; (void)value; }
    virtual void setArray(const std::string& key, std::vector<std::unique_ptr<DataNode>> value) { (void)key; (void)value; }
    virtual void setObject(const std::string& key, std::unique_ptr<DataNode> value) { (void)key; (void)value; }

    virtual bool isString() const = 0;
    virtual bool isInt() const = 0;
    virtual bool isFloat() const = 0;
    virtual bool isBool() const = 0;
    virtual bool isArray() const = 0;
    virtual bool isObject() const = 0;

    virtual std::unique_ptr<DataNode> clone() const = 0;
};

#endif // data_node_h
