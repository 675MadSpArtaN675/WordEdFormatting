#pragma once

#ifdef EXPORT
    #undef EXPORT
#endif

#if defined(_WIN32) || defined(_WIN64)
    #ifdef BUILD_LIB
        #define EXPORT __declspec(dllexport)
    #else
        #define EXPORT __declspec(dllimport)
    #endif
#else
    #define EXPORT __attribute__((visibility("default")))
#endif

#ifdef DEBUG_LOG
    #include <iostream>
    #define LOG(message) std::cout << message << std::endl
#else
    #define LOG(message)
#endif

#include <concepts>

#include <duckx/duckx.hpp>

template<typename T>
concept StringConvertable = std::same_as<T, std::string> || std::convertible_to<T, std::string>;
