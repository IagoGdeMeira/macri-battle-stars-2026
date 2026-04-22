#ifndef state_mapping_component_h
#define state_mapping_component_h

#include <memory>

class StateIdMapper;

struct StateMappingComponent { std::shared_ptr<StateIdMapper> mapper; };

#endif // state_mapping_component_h
