#pragma once
#if defined(_WIN32) || defined(_WIN64)
  #ifdef QT_TASK_FRAMEWORK_BUILD
    #define FRAMEWORK_EXPORT __declspec(dllexport)
  #else
    #define FRAMEWORK_EXPORT __declspec(dllimport)
  #endif
#else
  #define FRAMEWORK_EXPORT __attribute__((visibility("default")))
#endif
