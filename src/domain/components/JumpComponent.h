#ifndef jump_component_h
#define jump_component_h

struct JumpComponent
{
    bool isJumping = false;
    float force = 1500.f, maxTime = 0.2f, timer = 0.f;
    float gravityScaleAsc = 0.6f, gravityScaleDesc = 1.8f;
    float fastFallMultiplier = 2.5f;
};

#endif // jump_component_h
