//
// Created by lyc8503 on 2022/9/19.
//

#ifndef DNET_DEFS_H
#define DNET_DEFS_H

#include <cstdio>
#include <cstring>
#include <cassert>

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#define DNET_DEBUG(format, args...) printf("[DEBUG %10s:%-4d] " format "\n", __FILENAME__, __LINE__, ##args)
#define DNET_ERROR(format, args...) fprintf(stderr, "[ERR %10s:%-4d] " format "\n", __FILENAME__, __LINE__, ##args)

//#define DNET_DEBUG(format, args...) void(0);
//#define DNET_ERROR(format, args...) void(0);

/**
 *  Overload for Macro functions(DNET_ASSERT): https://stackoverflow.com/questions/16683146/can-macros-be-overloaded-by-number-of-arguments
 */
#define CAT( A, B ) A ## B
#define SELECT( NAME, NUM ) CAT( NAME ## _, NUM )

#define GET_COUNT( _1, _2, _3, _4, _5, _6 /* ad nauseam */, COUNT, ... ) COUNT
#define VA_SIZE( ... ) GET_COUNT( __VA_ARGS__, 6, 5, 4, 3, 2, 1 )

#define VA_SELECT( NAME, ... ) SELECT( NAME, VA_SIZE(__VA_ARGS__) )(__VA_ARGS__)

#define DNET_ASSERT( ... ) VA_SELECT( DNET_ASSERT, __VA_ARGS__ )

#define DNET_ASSERT_2(expr, msg) do {                                                                                                                   \
    if (!(expr)) {                                                                                                                                      \
        throw std::runtime_error("Assertion failed at [" + std::string(__FILENAME__) + ":" + std::to_string(__LINE__) + "]: " + std::string(msg));      \
    }                                                                                                                                                   \
} while(0)

#define DNET_ASSERT_1(expr) DNET_ASSERT_2(expr, "No msg provided.")

#endif //DNET_DEFS_H
