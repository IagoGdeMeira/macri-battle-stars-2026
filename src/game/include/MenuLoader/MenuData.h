#ifndef menu_data_h
#define menu_data_h

#include "UIElement/UIElement.h"

#include <memory>
#include <string>
#include <vector>

struct MenuData
{
    std::string id;
    std::vector<std::unique_ptr<UIElement>> elements;
};

#endif // menu_data_h
