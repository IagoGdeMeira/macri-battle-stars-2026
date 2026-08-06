#ifndef jump_component_h
#define jump_component_h

struct JumpComponent
{
    bool isJumping = false;
    float force = 5000.f, maxTime = 0.3f, timer = 0.f;
    float gravityScaleAsc = 0.5f, gravityScaleDesc = 2.5f;
    float fastFallMultiplier = 2.5f;
};

#endif // jump_component_h
