/*
* This source file is part of ARK
* For the latest info, see https://github.com/QuadHex
*
* Copyright (c) 2013-2018 QuadHex authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <limits>
#include <algorithm>
#include <utility>
#include <functional>
#include <cctype>
#include <iterator>
#include <unordered_map>
#include <stdint.h>
#include <functional>
#include <memory>
#include <signal.h>
#include <chrono>
#include <sstream>
#include <random>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <algorithm>
#include <future>
#include <ctime>
#include <bitset>
#include <regex>

#define NOMINMAX
#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(__WIN64__) || defined(WIN64) || defined(_WIN64)
// only windows include
#include <io.h>
#include <time.h>
#ifndef WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <MSWSock.h>
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#include <windows.h>
#define _SCL_SECURE_NO_WARNINGS

#define SIGHUP  1
#define SIGINT  2
#define SIGQUIT 3
#define SIGUSR1 10
#define SIGPIPE 13
#define SIGCHLD 17
#define SIGSYS  32

#include <Dbghelp.h>
#pragma comment(lib, "DbgHelp")

#else
// only other Unix/Linux include
#include <sys/socket.h>
#include <sys/time.h>
#include <pthread.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/prctl.h>
#include <dlfcn.h>

#endif

#define ARK_LITTLE_ENDIAN
#define ARK_BIG_ENDIAN

#if !defined(ARK_ENDIAN)
#  if defined(USE_BIG_ENDIAN)
#    define ARK_ENDIAN ARK_BIG_ENDIAN
#  else
#     define ARK_ENDIAN ARK_LITTLE_ENDIAN
#  endif
#endif // !defined(ARK_ENDIAN)

#define PLATFORM_WIN    0
#define PLATFORM_UNIX   1
#define PLATFORM_APPLE  2

#define UNIX_FLAVOUR_LINUX  1
#define UNIX_FLAVOUR_BSD    2
#define UNIX_FLAVOUR_OTHER  3
#define UNIX_FLAVOUR_OSX    4

#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(__WIN64__) || defined(WIN64) || defined(_WIN64)
#  define ARK_PLATFORM PLATFORM_WIN
#elif defined(__APPLE_CC__)
#  define ARK_PLATFORM PLATFORM_APPLE
#else
#  define ARK_PLATFORM PLATFORM_UNIX
#endif

#define COMPILER_MICROSOFT  0
#define COMPILER_GNU        1
#define COMPILER_BORLAND    2
#define COMPILER_INTEL      3
#define COMPILER_CLANG      4

#ifdef _MSC_VER
#  define ARK_COMPILER COMPILER_MICROSOFT
#elif defined(__INTEL_COMPILER)
#  define ARK_COMPILER COMPILER_INTEL
#elif defined(__BORLANDC__)
#  define ARK_COMPILER COMPILER_BORLAND
#elif defined(__GNUC__)
#  define ARK_COMPILER COMPILER_GNU
#elif defined( __clang__ )
#  define ARK_COMPILER COMPILER_CLANG
#else
#  pragma error "FATAL ERROR: Unknown compiler."
#endif

#if ARK_PLATFORM == PLATFORM_UNIX || ARK_PLATFORM == PLATFORM_APPLE
#  if defined(HAVE_DARWIN)
#    define ARK_PLATFORM_NAME "MacOSX"
#    define UNIX_FLAVOUR UNIX_FLAVOUR_OSX
#  elif defined(USE_KQUEUE)
#    define ARK_PLATFORM_NAME "FreeBSD"
#    define UNIX_FLAVOUR UNIX_FLAVOUR_BSD
#  elif defined(USE_KQUEUE_DFLY)
#    define ARK_PLATFORM_NAME "DragonFlyBSD"
#    define UNIX_FLAVOUR UNIX_FLAVOUR_BSD
#  else
#    define ARK_PLATFORM_NAME "Linux"
#    define UNIX_FLAVOUR UNIX_FLAVOUR_LINUX
#  endif
#elif ARK_PLATFORM == PLATFORM_WIN
#  define ARK_PLATFORM_NAME "Windows"
#else
#  pragma error "FATAL ERROR: Unknown platform."
#endif

#define ARK_RUN_MODE_DEBUG      0
#define ARK_RUN_MODE_RELEASE    1

#ifndef ARK_RUN_MODE
#  if defined(DEBUG) || defined(_DEBUG)
#    define ARK_RUN_MODE ARK_RUN_MODE_DEBUG
#    define ARK_RUN_MODE_NAME "Debug"
#  else
#    define ARK_RUN_MODE ARK_RUN_MODE_RELEASE
#    define ARK_RUN_MODE_NAME "Release"
#  endif // DEBUG
#endif

#ifndef X64
#  if defined(_WIN64) || defined(__x86_64__) || defined(__amd64) || defined(__LP64__)
#    define X64
#  endif
#endif

#ifdef X64
#  define ARK_ARCH_NAME "X64"
#else
#  define ARK_ARCH_NAME "X86"
#endif // X64

#define ARK_LITTLE_ENDIAN

#if ARK_PLATFORM == PLATFORM_WIN
#pragma warning(disable:4251)
#pragma warning(disable:4267)
#pragma warning(disable:4305)
#pragma warning(disable:4800)
#pragma warning(disable:4996)
#pragma warning(disable:)
#endif

namespace ark
{
    using AFGUID = int64_t;
}