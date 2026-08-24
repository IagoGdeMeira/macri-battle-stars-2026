#include "TimerWidgetLoader.h"

#include "UIFactory/UIFactory.h"

#include "domain/components/RoundTimerTag.h"
#include "domain/components/UILayoutMetricsComponent.h"
#include "domain/components/UITextComponent.h"
#include "domain/include/World/World.h"
#include "domain/resources/Font/Font.h"
#include "domain/utils/Logger/Logger.h"

#include "engine/include/IFontFactory/IFontFactory.h"
#include "engine/utils/DataUtils/DataUtils.h"

TimerWidgetLoader::TimerWidgetLoader(UIFactory& factory, IFontFactory& fontFactory) :
    factory(factory), fontFactory(fontFactory) {}

Entity TimerWidgetLoader::load(const DataNode& node, const ParamMap& params)
{
    auto& comp = this->factory.world().components();

    Rectangle rect = DataUtils::parseRect(node, {{0.f, 0.f}, {100.f, 40.f}});
    Entity panel = this->factory.createBox(rect);

    float initialTime = node.getFloat("initialTime", 99.f);
    if (params.contains("initialTime")) initialTime = std::stof(params.at("initialTime"));

    std::string text = std::to_string(static_cast<int>(initialTime));
    float fontSize = node.getFloat("fontSize", 32.f);

    Color color = Color::WHITE();
    if (node.has("color"))
    {
        auto colorNode = node.getObject("color");
        if (colorNode) color = DataUtils::parseColor(*colorNode, Color::WHITE());
    }

    std::string fontPath = node.getString("fontPath", "assets/fonts/default.ttf");
    std::shared_ptr<Font> font = nullptr;

    try { font = this->fontFactory.createFont(fontPath); }
    catch (const std::exception&) { LOG_ERROR("Failed to load font: " + fontPath); }

    UIFactory::TextParams paramsText{text, fontSize, color, rect.position};
    Entity textEntity = this->factory.createText(paramsText, font);

    this->factory.attachChild(panel, textEntity, {0.f, 0.f});

    comp.add<RoundTimerTag>(panel, RoundTimerTag{});
    comp.get<UILayoutMetricsComponent>(textEntity).size = rect.size;

    return panel;
}
