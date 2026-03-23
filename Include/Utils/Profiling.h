#pragma once

#if defined(TRACY_ENABLE) && defined(_DEBUG)

#include <tracy/Tracy.hpp>

#define ProfilingEndFrame FrameMark

#else

#define ProfilingEndFrame

#endif
