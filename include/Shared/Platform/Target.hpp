/*
 *  Compiler macros to attempt identifying target operating system and architecture.
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
#ifndef SHARED_PLATFORM_TARGET_HPP
#define SHARED_PLATFORM_TARGET_HPP

#if defined(_WIN32) || defined(_WIN64) || defined(WIN32) || defined(WIN64) || defined(__WIN32__) || defined(__WINDOWS__)
#   define PLATFORM_OS_WIN "Microsoft Windows"
#   define PLATFORM_OS PLATFORM_OS_WIN
#elif defined(MSDOS) || defined(__MSDOS__) || defined(_MSDOS) || defined(__DOS__)
#	define PLATFORM_OS_MSDOS "MS-DOS"
#	define PLATFORM_OS PLATFORM_OS_MSDOS
#elif defined(_CYGWIN) || defined(__CYGWIN__)
#   define PLATFORM_OS_CYGWIN "Cygwin"
#   define PLATFORM_OS PLATFORM_OS_CYGWIN
#elif defined(__APPLE__)
#   include <TargetConditionals.h>
#	if defined(TARGET_IPHONE_SIMULATOR) || defined(TARGET_OS_IPHONE)
#       define PLATFORM_OS_IOS "Apple iOS"
#       define PLATFORM_OS PLATFORM_OS_IOS
#   elif defined(TARGET_OS_MAC)
#       define PLATFORM_OS_OSX "Apple OS X"
#       define PLATFORM_OS PLATFORM_OS_OSX
#   endif
#elif defined(__ANDROID__)
#   define PLATFORM_OS_ANDROID "Android"
#   define PLATFORM_OS PLATFORM_OS_ANDROID
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__)
#   define PLATFORM_OS_BSD "BSD Environment"
#   define PLATFORM_OS PLATFORM_OS_BSD
#elif defined(__SYMBIAN32__)
#   define PLATFORM_OS_SYMBIAN "Symbian OS"
#   define PLATFORM_OS PLATFORM_OS_SYMBIAN
#elif defined(__gnu_linux__) || defined(__linux__)
#   define PLATFORM_OS_LINUX "GNU/Linux"
#   define PLATFORM_OS PLATFORM_OS_LINUX
#elif defined(__sun) && defined(__SVR4)
#	define PLATFORM_OS_SOLARIS "Oracle Solaris"
#   define PLATFORM_OS PLATFORM_OS_SOLARIS
#elif defined(__hpux)
#	define PLATFORM_OS_HPUX "HP-UX"
#   define PLATFORM_OS PLATFORM_OS_HPUX
#elif defined(_AIX) || defined(__TOS_AIX__)
#	define PLATFORM_OS_AIX "IBM AIX"
#	define PLATFORM_OS PLATFORM_OS_AIX
#else
#   define PLATFORM_OS_UNK "Unknown Operating System"
#   define PLATFORM_OS PLATFORM_OS_UNK
#endif

#if defined(_M_ALPHA) || defined(__alpha__) || defined(__alpha)
#   define PLATFORM_ARCH_ALPHA "Alpha"
#   define PLATFORM_ARCH PLATFORM_ARCH_ALPHA
#elif defined(__thumb__) || defined(__TARGET_ARCH_THUMB) || defined(_M_ARMT)
#	define PLAFORM_ARCH_THUMB "ARM THUMB"
#	define PLATFORM_ARCH PLAFORM_ARCH_THUMB
#elif defined(_M_ARM64) || defined(__aarch64__)
#   define PLATFORM_ARCH_ARM64 "ARM64"
#   define PLATFORM_ARCH PLATFORM_ARCH_ARM64
#elif defined(_M_ARM) || defined(__arm__) || defined(_ARM) || defined(__arm) || defined(__TARGET_ARCH_ARM)
#   define PLATFORM_ARCH_ARM "ARM"
#   define PLATFORM_ARCH PLATFORM_ARCH_ARM
#elif defined(__hppa__) || defined(__HPPA__) || defined(__hppa)
#   define PLATFORM_ARCH_HPPA "HP/PA RISC"
#   define PLATFORM_ARCH PLATFORM_ARCH_HPPA
#elif defined(_M_X64) || defined(_M_AMD64) || defined(__x86_64__) || defined(__x86_64) || defined(__amd64__)|| defined(__amd64)
#   define PLATFORM_ARCH_X64 "Intel x86-64"
#   define PLATFORM_ARCH PLATFORM_ARCH_X64
#elif defined(_M_IX86) || defined(_X86_) || defined(__i386) || defined(__i386__) || defined(__X86__) || defined(__386) || defined(__I86__) || defined(__IA32__)
#   define PLATFORM_ARCH_X86 "Intel x86"
#   define PLATFORM_ARCH PLATFORM_ARCH_X86
#elif defined(_M_IA64) || defined(__ia64__) || defined(__ia64)
#   define PLATFORM_ARCH_IA64 "Intel IA-64"
#   define PLATFORM_ARCH PLATFORM_ARCH_IA64
#elif defined(__mips__) || defined(__MIPS__) || defined(__mips)
#   define PLATFORM_ARCH_MIPS "MIPS"
#   define PLATFORM_ARCH PLATFORM_ARCH_MIPS
#elif defined(__PPC64__) || defined(__ppc64__) || defined(__powerpc64__) || defined(_ARCH_PPC64)
#   define PLATFORM_ARCH_PPC "PowerPC64"
#   define PLATFORM_ARCH PLATFORM_ARCH_PPC64
#elif defined(_M_PPC) || defined(__ppc__) || defined(__powerpc__) || defined(_ARCH_PPC) || defined(__ppc)
#   define PLATFORM_ARCH_PPC "PowerPC"
#   define PLATFORM_ARCH PLATFORM_ARCH_PPC
#elif defined(__sparc__) || defined(__sparc)
#   define PLATFORM_ARCH_SPARC "SPARC"
#   define PLATFORM_ARCH PLATFORM_ARCH_SPARC
#elif defined(__s390__) || defined(__s390x__)
#   define PLATFORM_ARCH_S390 "IBM ESA/390"
#   define PLATFORM_ARCH PLATFORM_ARCH_S390
#else
#   define PLATFORM_ARCH_UNK "Unknown CPU Architecture"
#   define PLATFORM_ARCH PLATFORM_ARCH_UNK
#endif

#endif /* SHARED_PLATFORM_TARGET_HPP */
