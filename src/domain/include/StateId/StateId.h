#ifndef state_id_h
#define state_id_h

enum class StateId
{
    Unknown = -1,
    Idle,
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
};

#endif // state_id_h
