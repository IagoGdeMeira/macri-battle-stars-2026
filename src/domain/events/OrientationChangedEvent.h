#ifndef orientation_changed_event_h
#define orientation_changed_event_h

#include "../include/Entity/Entity.h"
#include "../include/Orientation/Orientation.h"

struct OrientationChangedEvent { Entity entity; Orientation previous, current; };

#endif // orientation_changed_event_h
