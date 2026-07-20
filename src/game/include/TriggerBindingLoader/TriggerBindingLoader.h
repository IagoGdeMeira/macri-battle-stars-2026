#ifndef trigger_binding_loader_h
#define trigger_binding_loader_h

#include "TriggerContext/TriggerContext.h"

#include "engine/include/DataParser/DataParser.h"

class TriggerBindingLoader
{
public:
    TriggerBindingLoader(DataParser& parser) : parser(parser) {}
    TriggerContext load(const std::string& path);

private:
    DataParser& parser;
};

#endif // trigger_binding_loader_h
