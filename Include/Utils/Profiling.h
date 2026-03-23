#pragma once

// #define PROFILING_ENABLE

#if defined(PROFILING_ENABLE)

#define TRACY_ENABLE

// #include <tracy/Tracy.hpp>

// #define ProfilingEndFrame FrameMark

#else

#define ProfilingEndFrame

#endif
