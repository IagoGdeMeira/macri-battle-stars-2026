#ifndef condition_factory_h
#define condition_factory_h

#include "../ICondition/ICondition.h"

#include "../../engine/include/DataNode/DataNode.h"

#include <memory>

class ConditionFactory
{
public:
    static std::unique_ptr<ICondition> create(const DataNode& node);
};

#endif // condition_factory_h
