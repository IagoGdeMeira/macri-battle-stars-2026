#include "HUDScene.h"

#include "ComponentRegistry/HUDComponentRegistry.h"
#include "HealthBarSystem/HealthBarSystem.h"
#include "HUDVisibilitySystem/HUDVisibilitySystem.h"
#include "PlayerSpawnedEvent.h"
#include "RoundTimerDisplaySystem/RoundTimerDisplaySystem.h"
#include "UIFactory/UIFactory.h"
#include "UIDrawer/UIDrawer.h"
#include "UILayoutSystem/UILayoutSystem.h"

#include "domain/components/ActiveComponent.h"
#include "domain/components/ChildrenComponent.h"
#include "domain/components/FlexContainer.h"
#include "domain/components/HealthBarTag.h"
#include "domain/components/HealthBarSegmentComponent.h"
#include "domain/components/HUDEntityTag.h"
#include "domain/components/ParentComponent.h"
#include "domain/components/RenderComponent.h"
#include "domain/components/RoundTimerTag.h"
#include "domain/components/UISpriteComponent.h"
#include "domain/components/UITextComponent.h"
#include "domain/components/UITransform.h"
#include "domain/utils/Logger/Logger.h"

#include "engine/value_objects/RenderContext/RenderContext.h"

HUDScene::HUDScene(Config&& cfg) :
    Scene(*cfg.eventBus),
    initialRoundTime(cfg.initialRoundTime),
    renderer(*cfg.renderer),
    settings(*cfg.settings),
    fontFactory(*cfg.fontFactory),
    textureFactory(*cfg.textureFactory) {}

HUDScene::~HUDScene() = default;

void HUDScene::init()
{
    this->registerComponents();
    this->addSystems();

    this->uiFactory = std::make_unique<UIFactory>(this->world(), this->fontFactory, this->textureFactory);
    this->uiDrawer = std::make_unique<UIDrawer>(this->eventBus, this->renderer, this->settings);

    this->eventBus.subscribe<PlayerSpawnedEvent>([this](const PlayerSpawnedEvent& e)
    {
        LOG_WARN("PlayerSpawnedEvent received for playerId: {}, characterName: {}, maxHealth: {}, currentHealth: {}",
            e.playerId, e.characterName, e.maxHealth, e.currentHealth);
        this->createHealthBar(HealthBarParams{e.playerId, e.characterName, e.maxHealth, e.currentHealth });
    });

    this->createTimer(this->initialRoundTime);
}

void HUDScene::registerComponents() { HUDComponentRegistry::registerAll(this->world().components()); }

void HUDScene::addSystems()
{
    this->addSystem<HealthBarSystem>(this->eventBus);
    this->addSystem<RoundTimerDisplaySystem>(this->eventBus);
    this->addSystem<UILayoutSystem>();

    auto& visSystem = this->addSystem<HUDVisibilitySystem>();
    this->visibilitySystem = &visSystem;
}

void HUDScene::onPause() { if (this->visibilitySystem) this->visibilitySystem->setVisible(false); }

void HUDScene::onResume() { if (this->visibilitySystem) this->visibilitySystem->setVisible(true); }

void HUDScene::render()
{
    if (!this->uiDrawer) return;
    RenderContext ctx{ this->world(), this->eventBus };
    this->uiDrawer->draw(ctx);
}

void HUDScene::createHealthBar(const HealthBarParams& params)
{
    auto& world = this->world();
    auto& comp = world.components();

    const float totalWidth = 300.f;
    const float barHeight = 30.f;
    const float maxSegmentHP = 100.f;
    const int maxHealth = params.maxHealth;
    const int currentHealth = params.currentHealth;
    const int numSegments = (maxHealth + static_cast<int>(maxSegmentHP) - 1) / static_cast<int>(maxSegmentHP);
    const float segmentMaxWidth = totalWidth / numSegments;

    Entity container = world.entities().create();
    comp.add<UITransform>(container, UITransform{
        Rectangle{Position{10.f, 10.f + 50.f * params.playerId}, Dimension2D{totalWidth, barHeight}},
        UITransform::UIAnchor::TopLeft
    });
    comp.add<ActiveComponent>(container, ActiveComponent{true});
    comp.add<HUDEntityTag>(container, HUDEntityTag{});
    comp.add<HealthBarTag>(container, HealthBarTag{params.playerId, maxHealth, currentHealth});
    comp.add<ChildrenComponent>(container, ChildrenComponent{});
    comp.add<RenderComponent>(container, RenderComponent{0, 0});

    Entity background = world.entities().create();
    comp.add<UITransform>(background, UITransform{Rectangle{Position{0.f, 0.f}, Dimension2D{totalWidth, barHeight}}});
    comp.add<ActiveComponent>(background, ActiveComponent{true});
    comp.add<ParentComponent>(background, ParentComponent{container});
    comp.add<UISpriteComponent>(background, UISpriteComponent{nullptr, Color{60, 60, 60, 255}});
    comp.add<RenderComponent>(background, RenderComponent{0, 0});
    comp.get<ChildrenComponent>(container).children.push_back(background);

    Entity border = world.entities().create();
    comp.add<UITransform>(border, UITransform{Rectangle{Position{0.f, 0.f}, Dimension2D{totalWidth, barHeight}}});
    comp.add<ActiveComponent>(border, ActiveComponent{true});
    comp.add<ParentComponent>(border, ParentComponent{container});
    comp.add<UISpriteComponent>(border, UISpriteComponent{nullptr, Color::TRANSPARENT()});
    comp.add<RenderComponent>(border, RenderComponent{0, 2});
    comp.get<ChildrenComponent>(container).children.push_back(border);

    float remainingHP = static_cast<float>(currentHealth);
    for (int i = 0; i < numSegments; ++i)
    {
        float segmentHP = std::min(remainingHP, maxSegmentHP);
        remainingHP -= segmentHP;

        bool isLast = (i == numSegments - 1);
        Color fillColor = isLast ? Color{255, 200, 0, 255} : Color{0, 200, 0, 255};

        Entity segment = world.entities().create();
        comp.add<UITransform>(segment, UITransform{
            Rectangle{Position{i * segmentMaxWidth, 0.f},
            Dimension2D{(segmentHP / maxSegmentHP) * segmentMaxWidth, barHeight}}});
        comp.add<ActiveComponent>(segment, ActiveComponent{true});
        comp.add<ParentComponent>(segment, ParentComponent{container});
        comp.add<HealthBarSegmentComponent>(segment, HealthBarSegmentComponent{maxSegmentHP, segmentMaxWidth});
        comp.add<UISpriteComponent>(segment, UISpriteComponent{nullptr, fillColor});
        comp.add<RenderComponent>(segment, RenderComponent{0, 1});
        comp.get<ChildrenComponent>(container).children.push_back(segment);
    }
}

void HUDScene::createTimer(float initialTime)
{
    auto& world = this->world();
    auto& comp = world.components();

    Entity timerPanel = this->uiFactory->createPanel(Rectangle{Position{400.f - 50.f, 10.f}, Dimension2D{100.f, 40.f}});
    comp.add<ActiveComponent>(timerPanel, ActiveComponent{true});
    comp.add<HUDEntityTag>(timerPanel, HUDEntityTag{});

    auto& flex = comp.get<FlexContainer>(timerPanel);
    flex.direction = FlexContainer::FlexDirection::Row;
    flex.justify = FlexContainer::JustifyContent::Center;
    flex.align = FlexContainer::AlignItems::Center;

    Entity timerText = this->uiFactory->createText(
        std::to_string(static_cast<int>(initialTime)),
        32.f, Color::WHITE(), Position{0.f, 0.f});
    comp.add<ActiveComponent>(timerText, ActiveComponent{true});
    comp.add<HUDEntityTag>(timerText, HUDEntityTag{});
    comp.add<RoundTimerTag>(timerText, RoundTimerTag{});
    comp.add<ParentComponent>(timerText, ParentComponent{timerPanel});
}
