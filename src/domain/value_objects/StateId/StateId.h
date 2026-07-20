#ifndef state_id_h
#define state_id_h

#include <cstddef>
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

class StateId
{
public:
    enum class BaseState : std::int32_t
    {
        Unknown = -1,
        Idle = 0,
        Running,
        Walking,
        Jumping,
        Falling,
        Punching,
        Kicking,
        Blocking,
        Dying,
        BeingHit,
        ChargingSpecial,
        SpecialAttack,
        Crouching,
        CustomStart = 1000
    };

    struct Hash { std::size_t operator()(const StateId& stateId) const { return std::hash<std::int32_t>{}(stateId.value()); } };

    StateId() : id(static_cast<std::int32_t>(BaseState::Unknown)) {}
    explicit StateId(BaseState base) : id(static_cast<std::int32_t>(base)) {}

    static StateId createCustom(std::int32_t id);

    bool operator==(const StateId& other) const { return this->id == other.id; }
    bool operator!=(const StateId& other) const { return !(*this == other); }

    std::int32_t value() const { return this->id; }
    bool isCustom() const { return this->id >= static_cast<std::int32_t>(BaseState::CustomStart); }
    bool isUnknown() const { return this->id == static_cast<std::int32_t>(BaseState::Unknown); }

    static bool isBaseName(const std::string& name) { return StateId::nameToId.contains(name); }
    static StateId fromBaseName(const std::string& name);
    static std::string toBaseName(StateId id);

    static const StateId Unknown;
    static const StateId Idle;
    static const StateId Running;
    static const StateId Walking;
    static const StateId Jumping;
    static const StateId Falling;
    static const StateId Punching;
    static const StateId Kicking;
    static const StateId Blocking;
    static const StateId Dying;
    static const StateId BeingHit;
    static const StateId ChargingSpecial;
    static const StateId SpecialAttack;
    static const StateId Crouching;

private:
    explicit StateId(std::int32_t value) : id(value) {}

    std::int32_t id;

    static const std::unordered_map<std::string, StateId::BaseState> nameToId;
    static const std::unordered_map<std::int32_t, std::string> idToName;
};

#endif // state_id_h
