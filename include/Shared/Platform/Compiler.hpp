/*  
 *  Compiler macros to attempt identifying compiler at compile time.
 *  
 *  The list may be incomplete.
 */
/*
 *  MIT License
 *  
 *  Copyright (c) 2020 Gergely Pintér
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
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
