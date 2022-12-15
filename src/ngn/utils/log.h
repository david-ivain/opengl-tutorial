#pragma once

#include <iostream>

#define LOGERRF(format_string, args...)              \
    fprintf(stderr, "(%s:%d) ", __FILE__, __LINE__); \
    fprintf(stderr, format_string, args);            \
    fprintf(stderr, "\n")
#define LOGERR(string)                               \
    fprintf(stderr, "(%s:%d) ", __FILE__, __LINE__); \
    fprintf(stderr, string);                         \
    fprintf(stderr, "\n")

#ifndef NDEBUG
#define LOGF(format_string, args...)        \
    printf("(%s:%d) ", __FILE__, __LINE__); \
    printf(format_string, args);            \
    printf("\n")
#define LOG(string)                         \
    printf("(%s:%d) ", __FILE__, __LINE__); \
    printf(string);                         \
    printf("\n")
#else
#define LOG(args...)
#define LOGF(args...)
#endif
