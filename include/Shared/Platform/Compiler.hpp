// Licensed under the MIT License. Copyright (c) 2020 Gergely Pintér.
/*  
 *  Compiler macros to attempt identifying compiler at compile time.
 *  
 *  The list may be incomplete.
 */
#ifndef SHARED_PLATFORM_COMPILER_HPP
#define SHARED_PLATFORM_COMPILER_HPP

#if defined(__CC_ARM)
#   define PLATFORM_COMPILER_ARMCC "ARM Compiler"
#   define PLATFORM_COMPILER PLATFORM_COMPILER_ARMCC
#elif defined(__clang__)
#   define PLATFORM_COMPILER_CLANG "Clang"
#   define PLATFORM_COMPILER PLATFORM_COMPILER_CLANG
#elif defined(__GNUC__)
#   define PLATFORM_COMPILER_GCC "GNU Compiler Chain"
#   define PLATFORM_COMPILER PLATFORM_COMPILER_GCC
#elif defined(__INTEL_COMPILER)
#   define PLATFORM_COMPILER_ICC "Intel C++ Compiler"
#   define PLATFORM_COMPILER PLATFORM_COMPILER_ICC
#elif defined(_MSC_VER)
#   define PLATFORM_COMPILER_MSC "Microsoft Visual C++ Compiler"
#   define PLATFORM_COMPILER PLATFORM_COMPILER_MSC
#elif defined(__MINGW32__) || defined(__MINGW64__)
#   define PLATFORM_COMPILER_MINGW "MinGW"
#   define PLATFORM_COMPILER PLATFORM_COMPILER_MINGW
#elif defined(__BORLANDC__)
#	define PLATFORM_COMPILER_BORLAND "Borland C++"
#	define PLATFORM_COMPILER PLATFORM_COMPILER_BORLAND
#elif defined(__TI_COMPILER_VERSION__)
#	define PLATFORM_COMPILER_TICC "Texas Instruments C/C++ Compiler"
#	define PLATFORM_COMPILER PLATFORM_COMPILER_TICC
#elif defined(__WATCOMC__)
#	define PLATFORM_COMPILER_WATCOM "Watcom C++ Compiler"
#	define PLATFORM_COMPILER PLATFORM_COMPILER_WATCOM
#else
#   define PLATFORM_COMPILER_UNK "Unknown Compiler"
#   define PLATFORM_COMPILER PLATFORM_COMPILER_UNK
#endif

#endif /* SHARED_PLATFORM_COMPILER_HPP */
