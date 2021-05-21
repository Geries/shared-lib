// Licensed under the MIT License. Copyright (c) 2021 Gergely Pintér.
/*
 *  Old header which used to have some typedefs, now it just includes standard's headers and is basically obsolete.
 */
#ifndef SHARED_PLATFORM_TYPES_HPP
#define SHARED_PLATFORM_TYPES_HPP

#include <cstdint>
#include <cstddef>
#include <limits>

#if __cplusplus <= 202000L && !defined(__cpp_char8_t)
typedef char char8_t;
#endif

#endif /* SHARED_PLATFORM_TYPES_HPP */
