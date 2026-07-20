#ifndef input_binding_loader_h
#define input_binding_loader_h

#include "DataParser/DataParser.h"
#include "InputContext/InputContext.h"

#include <string>

class InputBindingLoader
{
public:
    InputBindingLoader(DataParser& parser) : parser(parser) {};

    InputContext load(const std::string& path);

private:
    DataParser& parser;
};

#endif // input_binding_loader_h
