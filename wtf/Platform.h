/*
 * Copyright (C) 2006, 2007, 2008, 2009 Apple Inc.  All rights reserved.
 * Copyright (C) 2007-2009 Torch Mobile, Inc.
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WTF_Platform_h
#define WTF_Platform_h

#include <stdint.h>

typedef uint16_t UChar;
typedef uint32_t UChar32;

/* ==== Platform adaptation macros: these describe properties of the target environment. ==== */

/* COMPILER() - the compiler being used to build the project */
#define COMPILER(WTF_FEATURE) (defined WTF_COMPILER_##WTF_FEATURE  && WTF_COMPILER_##WTF_FEATURE)
/* CPU() - the target CPU architecture */
#define CPU(WTF_FEATURE) (defined WTF_CPU_##WTF_FEATURE  && WTF_CPU_##WTF_FEATURE)

/* ==== COMPILER() - the compiler being used to build the project ==== */

/* COMPILER(MSVC) Microsoft Visual C++ */
/* COMPILER(MSVC7_OR_LOWER) Microsoft Visual C++ 2003 or lower*/
/* COMPILER(MSVC9_OR_LOWER) Microsoft Visual C++ 2008 or lower*/
#if defined(_MSC_VER)
#define WTF_COMPILER_MSVC 1
#if _MSC_VER < 1400
#define WTF_COMPILER_MSVC7_OR_LOWER 1
#elif _MSC_VER < 1600
#define WTF_COMPILER_MSVC9_OR_LOWER 1
#endif
#endif

/* COMPILER(RVCT)  - ARM RealView Compilation Tools */
#if defined(__CC_ARM) || defined(__ARMCC__)
#define WTF_COMPILER_RVCT 1
#endif

/* COMPILER(GCC) - GNU Compiler Collection */
/* --gnu option of the RVCT compiler also defines __GNUC__ */
#if defined(__GNUC__) && !COMPILER(RVCT)
#define WTF_COMPILER_GCC 1
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#define GCC_VERSION_AT_LEAST(major, minor, patch) (GCC_VERSION >= (major * 10000 + minor * 100 + patch))
#else
/* define this for !GCC compilers, just so we can write things like COMPILER(GCC) && GCC_VERSION_AT_LEAST(4,1,0) */
#define GCC_VERSION_AT_LEAST(major, minor, patch) 0
#endif

/* COMPILER(MINGW) - MinGW GCC */
/* COMPILER(MINGW64) - mingw-w64 GCC - only used as additional check to exclude mingw.org specific functions */
#if defined(__MINGW32__)
#define WTF_COMPILER_MINGW 1
#include <_mingw.h> /* private MinGW header */
    #if defined(__MINGW64_VERSION_MAJOR) /* best way to check for mingw-w64 vs mingw.org */
        #define WTF_COMPILER_MINGW64 1
    #endif /* __MINGW64_VERSION_MAJOR */
#endif /* __MINGW32__ */

/* COMPILER(WINSCW) - CodeWarrior for Symbian emulator */
#if defined(__WINSCW__)
#define WTF_COMPILER_WINSCW 1
/* cross-compiling, it is not really windows */
#undef WIN32
#undef _WIN32
#endif

/* COMPILER(INTEL) - Intel C++ Compiler */
#if defined(__INTEL_COMPILER)
#define WTF_COMPILER_INTEL 1
#endif

/* ==== CPU() - the target CPU architecture ==== */

/* This also defines CPU(BIG_ENDIAN) or CPU(MIDDLE_ENDIAN) or neither, as appropriate. */

/* CPU(MIPS) - MIPS 32-bit */
#if (defined(mips) || defined(__mips__)) \
    && defined(_ABIO32)
#define WTF_CPU_MIPS 1
#if defined(__MIPSEB__)
#define WTF_CPU_BIG_ENDIAN 1
#endif
#endif /* MIPS */

/* CPU(PPC) - PowerPC 32-bit */
#if   defined(__ppc__)     \
    || defined(__PPC__)     \
    || defined(__powerpc__) \
    || defined(__powerpc)   \
    || defined(__POWERPC__) \
    || defined(_M_PPC)      \
    || defined(__PPC)
#define WTF_CPU_PPC 1
#define WTF_CPU_BIG_ENDIAN 1
#endif

/* CPU(PPC64) - PowerPC 64-bit */
#if   defined(__ppc64__) \
    || defined(__PPC64__)
#define WTF_CPU_PPC64 1
#define WTF_CPU_BIG_ENDIAN 1
#endif


/* CPU(X86) - i386 / x86 32-bit */
#if   defined(__i386__) \
    || defined(i386)     \
    || defined(_M_IX86)  \
    || defined(_X86_)    \
    || defined(__THW_INTEL)
#define WTF_CPU_X86 1
#endif

/* CPU(X86_64) - AMD64 / Intel64 / x86_64 64-bit */
#if   defined(__x86_64__) \
    || defined(_M_X64)
#define WTF_CPU_X86_64 1
#endif

/* CPU(ARM) - ARM, any version*/
#if   defined(arm) \
    || defined(__arm__) \
    || defined(ARM) \
    || defined(_ARM_)
#define WTF_CPU_ARM 1

#if defined(__ARMEB__)
#define WTF_CPU_BIG_ENDIAN 1

#elif !defined(__ARM_EABI__) \
    && !defined(__EABI__) \
    && !defined(__VFP_FP__) \
    && !defined(_WIN32_WCE) \
    && !defined(ANDROID)
#define WTF_CPU_MIDDLE_ENDIAN 1

#endif

#endif


#endif /* WTF_Platform_h */
