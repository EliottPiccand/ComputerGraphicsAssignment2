#pragma once

#if defined(TRACY_ENABLE)

#include <tracy/Tracy.hpp>

#define ProfilingEndFrame FrameMark
#define ProfileScope ZoneScoped

#else

#define ProfilingEndFrame
#define ProfileScope

#endif
