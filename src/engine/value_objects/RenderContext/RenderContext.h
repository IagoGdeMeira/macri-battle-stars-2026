#ifndef render_context_h
#define render_context_h

#include "domain/include/World/World.h"

class EventBus;

struct RenderContext { World& world; EventBus& eventBus; };

#endif // render_context_h
