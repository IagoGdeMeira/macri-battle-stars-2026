#ifndef window_resized_event_h
#define window_resized_event_h

#include "../../domain/value_objects/Geometry/Geometry.h"

struct WindowResizedEvent { Dimension2D newSize{0.f, 0.f}; };

#endif // window_resized_event_h
