#include "TimerWidgetLoader.h"

#include "UIFactory/UIFactory.h"

#include "domain/components/RoundTimerTag.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/TransformComponent.h"
#include "domain/components/UIRectComponent.h"
#include "domain/components/UITextComponent.h"
#include "domain/include/World/World.h"

#include "engine/utils/DataUtils/DataUtils.h"

Entity TimerWidgetLoader::load(const DataNode& node, const UILoader::ParamMap& params)
{
    auto& comp = this->factory.world().components();

    Rectangle rect = DataUtils::parseRect(node, {{0.f, 0.f}, {100.f, 40.f}});
    Entity panel = this->factory.createPanel(rect);
    comp.add<RoundTimerTag>(panel, RoundTimerTag{});

    float initialTime = node.getFloat("initialTime", 99.f);
    if (params.contains("initialTime")) initialTime = std::stof(params.at("initialTime"));

    std::string initialText = std::to_string(static_cast<int>(initialTime));
    float fontSize = node.getFloat("fontSize", 32.f);
    Color color = Color::WHITE();
    if (node.has("color"))
    {
        auto colorNode = node.getObject("color");
        if (colorNode) color = DataUtils::parseColor(*colorNode, Color::WHITE());
    }

    Entity textEntity = this->factory.createText(initialText, fontSize, color, rect.position);
    comp.add<ParentComponent>(textEntity, ParentComponent{panel});
    comp.get<UIRectComponent>(textEntity).size = rect.size;
    comp.get<TransformComponent>(textEntity).position = rect.position;

    return panel;
}
