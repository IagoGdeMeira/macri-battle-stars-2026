#include "UIStyleLoader/UIStyleLoader.h"

#include "engine/include/DataNode/DataNode.h"

UIStyleLoader::StyleMap UIStyleLoader::load(const std::string& path)
{
    auto root = this->parser.parse(path);
    StyleMap styles;

    for (auto& styleNode : root->getArray("styles"))
    {
        std::string name = styleNode->getString("id");
        styles[name] = styleNode->clone();
    }
    return styles;
}
