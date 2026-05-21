#ifndef ui_action_component_h
#define ui_action_component_h

#include <functional>

struct UIActionComponent { std::function<void()> execute; };

#endif // ui_action_component_h
