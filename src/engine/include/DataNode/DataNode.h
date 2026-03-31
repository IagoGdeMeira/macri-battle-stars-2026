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

    virtual std::string getString(const std::string& key) const = 0;
    virtual int getInt(const std::string& key) const = 0;
    virtual float getFloat(const std::string& key) const = 0;
    
    virtual std::vector<std::unique_ptr<DataNode>> getArray(const std::string& key) const = 0;
};

#endif // data_node_h
