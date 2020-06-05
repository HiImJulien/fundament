#ifndef FUNDAMENT_API_H
#define FUNDAMENT_API_H

#if defined(__cplusplus)
    #define API_BASE extern "C"
#else 
    #define API_BASE
#endif

#if defined(FUNDAMENT_EXPORTS)
    #if defined(_WIN32)
        #define API API_BASE __declspec(dllexport)
    #else
        #define API API_BASE
    #endif
#else
    #if defined(_WIN32)
        #define API API_BASE __declspec(dllimport)
    #else
        #define API API_BASE
    #endif
#endif

#endif  // FUNDAMENT_API_H