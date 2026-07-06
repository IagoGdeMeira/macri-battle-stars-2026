#ifndef trigger_id_h
#define trigger_id_h

enum class TriggerId
{
    Unknown = -1,
    Jumped,
    Landed,
    Kicked,
    Punched,
    Blocked,
    StartedChargingSpecial,
    ReleasedSpecial,
    TookDamage,
    Died,
    TouchedWall,
    TouchedCeiling,
    LookedUp,
    LookedDown,
    LookedLeft,
    LookedRight,
    Crouched,
    CrouchReleased
};

#endif // trigger_id_h
