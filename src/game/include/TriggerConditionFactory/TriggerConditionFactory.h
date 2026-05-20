#ifndef trigger_condition_factory_h
#define trigger_condition_factory_h

#include "../ITriggerCondition/ITriggerCondition.h"

#include "../../engine/include/DataNode/DataNode.h"

#include <memory>

class TriggerConditionFactory
{
public:
    static std::unique_ptr<ITriggerCondition> create(const DataNode& node);
};

#endif // trigger_condition_factory_h
