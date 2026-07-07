#pragma once

#ifdef BUILD_LIB
    #ifdef _WIN32
        #define EXPORT __declspec(dllexport)
    #else
        #define EXPORT __attribute__(visibility("default"))
    #endif
#else
    #ifdef _WIN32
        #define EXPORT __declspec(dllimport)
    #else
        #define EXPORT
    #endif
#endif

#include <duckx/duckx.hpp>