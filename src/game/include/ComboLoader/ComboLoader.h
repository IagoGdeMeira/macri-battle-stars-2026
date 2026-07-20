#ifndef combo_loader_h
#define combo_loader_h

#include "Combo/Combo.h"

#include "engine/include/DataParser/DataParser.h"

#include <string>
#include <vector>

class ComboLoader
{
public:
    ComboLoader(DataParser& parser) : parser(parser) {}
    std::vector<Combo> load(const std::string& path);

private:
    DataParser& parser;
};

#endif // combo_loader_h
