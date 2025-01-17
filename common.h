#pragma once
#ifndef KIGU_COMMON_H
#define KIGU_COMMON_H

////////////////////////////////////////// compilers: COMPILER_CL, COMPILER_CLANG, COMPILER_GCC
//// compiler, platform, architecture //// platforms: OS_WINDOWS. OS_LINUX, OS_MAC
////////////////////////////////////////// architectures: ARCH_X64, ARCH_X86, ARCH_ARM64, ARCH_ARM32
//// CL Compiler //// (used for windows)
#if defined(_MSC_VER)
#  define COMPILER_CL 1
//#  pragma message("Compiler: cl")

#  if defined(_WIN32)
#    define OS_WINDOWS 1
#  else //_WIN32
#    error "unhandled compiler/platform combo"
#  endif //_WIN32

#  if defined(_M_AMD64)
#    define ARCH_X64 1
#  elif defined(_M_IX86) //_M_AMD64
#    define ARCH_X86 1
#  elif defined(_M_ARM64) //_M_IX86
#    define ARCH_ARM64 1
#  elif defined(_M_ARM) //_M_ARM64
#    define ARCH_ARM32 1
#  else //_M_ARM
#    error "unhandled architecture"
#  endif

//// CLANG Compiler //// (used for mac/linux)
#elif defined(__clang__) //_MSC_VER
#  define COMPILER_CLANG 1
//#  pragma message("Compiler: clang")

#  if   defined(__APPLE__) && defined(__MACH__)
#    define OS_MAC 1
#  elif defined(_WIN32)
#    define OS_WINDOWS 1
#  elif defined(__gnu_linux__)
#   undef M_PI
#   undef M_E
#	define OS_LINUX 1
#  else //__APPLE__ || __MACH__
#    error "unhandled compiler/platform combo"
#  endif //__APPLE__ || __MACH__

#  if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#    define ARCH_X64 1
#  elif defined(i386) || defined(__i386) || defined(__i386__) //__amd64__ || __amd64 || __x86_64 || __x86_64__
#    define ARCH_X86 1
#  elif defined(__aarch64__) //i386 || __i386__ || __i386__
#    define ARCH_ARM64 1
#  elif defined(__arm__) //__aarch64__
#    define ARCH_ARM32 1
#  else //__arm__
#    error "unhandled architecture"
#  endif

//// GCC Compiler //// (used for linux)
#elif defined(__GNUC__) || defined(__GNUG__) //__clang__
#  define COMPILER_GCC 1
//#  pragma message("Compiler: gcc")

#  if defined(__gnu_linux__)
#    define OS_LINUX 1
#  else //__gnu_linux__
#    error "unhandled compiler/platform combo"
#  endif

#  if defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64)
#    define ARCH_X64 1
#  elif defined(i386) || defined(__i386) || defined(__i386__) //__amd64__ || __amd64 || __x86_64 || __x86_64__
#    define ARCH_X86 1
#  elif defined(__aarch64__) //i386 || __i386__ || __i386__
#    define ARCH_ARM64 1
#  elif defined(__arm__) //__aarch64__
#    define ARCH_ARM32 1
#  else //__arm__
#    error "unhandled architecture"
#  endif

//// Unhandled Compiler ////
#else //__GNUC__ || __GNUG__
#  error "unhandled compiler"
#endif

#if !defined(COMPILER_CL)
#  define COMPILER_CL 0
#endif
#if !defined(COMPILER_CLANG)
#  define COMPILER_CLANG 0
#endif
#if !defined(COMPILER_GCC)
#  define COMPILER_GCC 0
#endif

#if !defined(OS_WINDOWS)
#  define OS_WINDOWS 0
#endif
#if !defined(OS_LINUX)
#  define OS_LINUX 0
#endif
#if !defined(OS_MAC)
#  define OS_MAC 0
#endif

#if !defined(ARCH_X64)
#  define ARCH_X64 0
#endif
#if !defined(ARCH_X86)
#  define ARCH_X86 0
#endif
#if !defined(ARCH_ARM64)
#  define ARCH_ARM64 0
#endif
#if !defined(ARCH_ARM32)
#  define ARCH_ARM32 0
#endif

//// Supported Compiler Features ////
#if COMPILER_CLANG || COMPILER_GCC
#  define COMPILER_FEATURE_TYPEOF 0
#else
#  define COMPILER_FEATURE_TYPEOF 0
#endif //#if COMPILER_CLANG || COMPILER_GCC

#if __cplusplus
#  define COMPILER_FEATURE_CPP 1
#  if   (__cplusplus >= 202002L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 20202L)
#    define COMPILER_FEATURE_CPP_20 1
#    define COMPILER_FEATURE_CPP_17 0
#    define COMPILER_FEATURE_CPP_14 0
#    define COMPILER_FEATURE_CPP_11 0
#    define COMPILER_FEATURE_CPP_98 0
#    define COMPILER_FEATURE_CPP_PRE98 0
#    define COMPILER_FEATURE_CPP_AUTO 1
#  elif (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#    define COMPILER_FEATURE_CPP_20 0
#    define COMPILER_FEATURE_CPP_17 1
#    define COMPILER_FEATURE_CPP_14 0
#    define COMPILER_FEATURE_CPP_11 0
#    define COMPILER_FEATURE_CPP_98 0
#    define COMPILER_FEATURE_CPP_PRE98 0
#    define COMPILER_FEATURE_CPP_AUTO 1
#  elif (__cplusplus >= 201402L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201402L)
#    define COMPILER_FEATURE_CPP_20 0
#    define COMPILER_FEATURE_CPP_17 0
#    define COMPILER_FEATURE_CPP_14 1
#    define COMPILER_FEATURE_CPP_11 0
#    define COMPILER_FEATURE_CPP_98 0
#    define COMPILER_FEATURE_CPP_PRE98 0
#    define COMPILER_FEATURE_CPP_AUTO 1
#  elif (__cplusplus >= 201103L)
#    define COMPILER_FEATURE_CPP_20 0
#    define COMPILER_FEATURE_CPP_17 0
#    define COMPILER_FEATURE_CPP_14 0
#    define COMPILER_FEATURE_CPP_11 1
#    define COMPILER_FEATURE_CPP_98 0
#    define COMPILER_FEATURE_CPP_PRE98 0
#    define COMPILER_FEATURE_CPP_AUTO 1
#  elif (__cplusplus >= 199711L)
#    define COMPILER_FEATURE_CPP_20 0
#    define COMPILER_FEATURE_CPP_17 0
#    define COMPILER_FEATURE_CPP_14 0
#    define COMPILER_FEATURE_CPP_11 0
#    define COMPILER_FEATURE_CPP_98 1
#    define COMPILER_FEATURE_CPP_PRE98 0
#    define COMPILER_FEATURE_CPP_AUTO 0
#  else
#    define COMPILER_FEATURE_CPP_20 0
#    define COMPILER_FEATURE_CPP_17 0
#    define COMPILER_FEATURE_CPP_14 0
#    define COMPILER_FEATURE_CPP_11 0
#    define COMPILER_FEATURE_CPP_98 0
#    define COMPILER_FEATURE_CPP_PRE98 1
#    define COMPILER_FEATURE_CPP_AUTO 0
#  endif
#else
#  define COMPILER_FEATURE_CPP 0
#  define COMPILER_FEATURE_CPP_20 0
#  define COMPILER_FEATURE_CPP_17 0
#  define COMPILER_FEATURE_CPP_14 0
#  define COMPILER_FEATURE_CPP_11 0
#  define COMPILER_FEATURE_CPP_98 0
#  define COMPILER_FEATURE_CPP_PRE98 0
#  define COMPILER_FEATURE_CPP_AUTO 0
#endif //#if __cplusplus


///////////////////////// //NOTE this file is included is almost every other file of the project, so be frugal with includes here
//// common includes ////
/////////////////////////
#include <stdio.h>
#include <stddef.h> //size_t, ptrdiff_t
#include <stdlib.h> //malloc, calloc, free
#include <stdarg.h> //va_start, va_list, va_arg, va_end
#include <string.h> //memcpy, memset, strcpy, strlen, etc
#include <math.h>   //log2


///////////////////////
//// static macros ////
///////////////////////
#define local    static //inside a .cpp
#define persist  static //inside a function
#define global   static //inside a .h
#define local_const   local const
#define persist_const persist const
#define global_const  global const


/////////////////////////////////////
//// compiler-dependent builtins ////
/////////////////////////////////////
#if COMPILER_CL
#  define FORCE_INLINE __forceinline
#  define DebugBreakpoint __debugbreak()
#  define ByteSwap16(x) _byteswap_ushort(x)
#  define ByteSwap32(x) _byteswap_ulong(x)
#  define ByteSwap64(x) _byteswap_uint64(x)
#elif COMPILER_CLANG || COMPILER_GCC
#  define FORCE_INLINE inline __attribute__((always_inline))
#  if defined(__i386__) || defined(__x86_64__)
#    define DebugBreakpoint __builtin_debugtrap()
#  else
#    include "signal.h"
#    define DebugBreakpoint raise(SIGTRAP)
#  endif
#  define ByteSwap16(x) __builtin_bswap16(x)
#  define ByteSwap32(x) __builtin_bswap32(x)
#  define ByteSwap64(x) __builtin_bswap64(x)
#else
#  error "unhandled compiler"
#endif //#if COMPILER_CL

//// stack allocation ////
#if OS_WINDOWS
#  include <malloc.h>
#  define StackAlloc(bytes) _alloca(bytes)
#elif OS_LINUX || OS_MAC
#  include <alloca.h>
#  define StackAlloc(bytes) alloca(bytes)
#else
#  error "unhandled os for stack allocation"
#endif //#if OS_WINDOWS


//////////////////////
//// common types ////
//////////////////////
typedef signed char        s8;
typedef signed short       s16;
typedef signed int         s32;
typedef signed long long   s64;
typedef ptrdiff_t          spt;   //signed pointer type
typedef unsigned char      u8;
typedef unsigned short     u16;
typedef unsigned int       u32;
typedef unsigned long long u64;
typedef size_t             upt;   //unsigned pointer type
typedef float              f32;
typedef double             f64;
typedef s8                 b8;    //sized boolean type
typedef s32                b32;   //sized boolean type
typedef wchar_t            wchar;

typedef u32 Type;
typedef u32 Flags;

struct cstring{
    char* str;
    upt   count;
    
    FORCE_INLINE explicit operator bool(){ return str && count; }
	FORCE_INLINE char& operator[](upt idx){ return str[idx]; }
	FORCE_INLINE char& operator*(){ return *str; }
	FORCE_INLINE char operator++(int){ str++; count--; return *(str-1); }
	FORCE_INLINE char* at(upt idx){ return &str[idx]; }
	FORCE_INLINE char* begin(){ return &str[0]; }
	FORCE_INLINE char* end()  { return &str[count]; }
	FORCE_INLINE const char* begin()const{ return &str[0]; }
	FORCE_INLINE const char* end()  const{ return &str[count]; }
#define cstring_lit(s) cstring{(char*)s, sizeof(s)-1}
#define cstr_lit(s) cstring{(char*)s, sizeof(s)-1}
#define cstr(s) cstring{(char*)s, sizeof(s)-1} //:)
};

template<typename T>
struct carray{
    T*  data;
    upt count;
    
    FORCE_INLINE explicit operator bool(){ return data && count; }
	FORCE_INLINE T& operator[](upt idx){ return data[idx]; }
	FORCE_INLINE T* at(upt idx){ return &data[idx]; }
	FORCE_INLINE T* begin(){ return &data[0]; }
	FORCE_INLINE T* end()  { return &data[count]; }
	FORCE_INLINE const T* begin()const{ return &data[0]; }
	FORCE_INLINE const T* end()  const{ return &data[count]; }
};

//TODO(delle) function pointer signature macro
typedef void* (*Allocator_ReserveMemory_Func)(upt size);
typedef void  (*Allocator_ChangeMemory_Func)(void* ptr, upt size);
typedef void  (*Allocator_ReleaseMemory_Func)(void* ptr);
typedef void* (*Allocator_ResizeMemory_Func)(void* ptr, upt size);

#if COMPILER_CLANG || COMPILER_GCC
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wreturn-type"
#elif COMPILER_CL
#  pragma warning(push)
#  pragma warning(default:4716)
#  pragma warning(disable:4716)
#endif //#if COMPILER_CLANG || COMPILER_GCC
#if BUILD_INTERNAL
global void* Allocator_ReserveMemory_Noop(upt size){return 0;}
global void* Allocator_ResizeMemory_Noop(void* ptr, upt size){return 0;}
#else
global void* Allocator_ReserveMemory_Noop(upt size){return 0;}
global void* Allocator_ResizeMemory_Noop(void* ptr, upt size){return 0;}
#endif //#if BUILD_INTERNAL
global void  Allocator_ChangeMemory_Noop(void* ptr, upt size){}
global void  Allocator_ReleaseMemory_Noop(void* ptr){}
#if COMPILER_CLANG || COMPILER_GCC
#  pragma clang diagnostic pop
#elif COMPILER_CL
#  pragma warning(pop)
#endif //#if COMPILER_CLANG || COMPILER_GCC

struct Allocator{
	Allocator_ReserveMemory_Func reserve;  //reserves address space from OS
	Allocator_ReleaseMemory_Func release;  //release the reserved memory back to OS
	Allocator_ResizeMemory_Func  resize;   //resizes reserved memory and moves memory if a new location is required
};

struct str8{
	u8* str;
	s64 count;
	FORCE_INLINE explicit operator bool(){ return str && count > 0; }
#define str8_lit(s) str8{(u8*)GLUE(u8,s), sizeof(GLUE(u8,s))-1}
#define str8l(s) str8{(u8*)GLUE(u8,s), sizeof(GLUE(u8,s))-1}
#define STR8(s) str8{(u8*)GLUE(u8,s), sizeof(GLUE(u8,s))-1}
#define str8null str8{0,0}
};

#ifndef KIGU_STRING_ALLOCATOR
#define KIGU_STRING_ALLOCATOR stl_allocator
#endif

struct dstr8{
	union{
		struct{u8* str; s64 count;};
		str8 fin;
	};
	s64 space;
	Allocator* allocator;
};

struct str16{
	u16* str;
	s64  count;
	FORCE_INLINE explicit operator bool(){ return str && count > 0; }
#define str16_lit(s) str16{(u16*)GLUE(u,s), (sizeof(GLUE(u,s))/sizeof(u16))-1}
};

struct str32{
	u32* str;
	s64  count;
	FORCE_INLINE explicit operator bool(){ return str && count > 0; }
#define str32_lit(s) str32{(u32*)GLUE(U,s), (sizeof(GLUE(U,s))/sizeof(u32))-1}
};


//////////////////////////
//// common constants ////
//////////////////////////
global_const u8  MAX_U8  = 0xFF;
global_const u16 MAX_U16 = 0xFFFF;
global_const u32 MAX_U32 = 0xFFFFFFFF;
global_const u64 MAX_U64 = 0xFFFFFFFFFFFFFFFF;

global_const s8  MIN_S8  = -127 - 1;
global_const s8  MAX_S8  = 127;
global_const s16 MIN_S16 = -32767 - 1;
global_const s16 MAX_S16 = 32767;
global_const s32 MIN_S32 = -2147483647 - 1;
global_const s32 MAX_S32 = 2147483647;
global_const s64 MIN_S64 = -9223372036854775807 - 1;
global_const s64 MAX_S64 = 9223372036854775807;

global_const f32 MAX_F32 = 3.402823466e+38f;
global_const f32 MIN_F32 = -MAX_F32;
global_const f64 MAX_F64 = 1.79769313486231e+308;
global_const f64 MIN_F64 = -MAX_F64;

global_const f32 M_ONETHIRD        = 0.33333333333f;
global_const f32 M_ONESIXTH        = 0.16666666667f;
global_const f32 M_EPSILON         = 0.00001f;
global_const f32 M_FOURTHPI        = 0.78539816339f;
global_const f32 M_HALFPI          = 1.57079632679f;
#undef M_PI
global_const f32 M_PI              = 3.14159265359f;
global_const f64 M_PId             = 3.14159265358979323846;
global_const f64 π                 = M_PId;
global_const f32 M_2PI             = 6.28318530718f;
global_const f32 M_TAU             = M_2PI;
#undef M_E
global_const f32 M_E               = 2.71828182846f;
global_const f32 M_SQRT_TWO        = 1.41421356237f;
global_const f32 M_SQRT_THREE      = 1.73205080757f;
global_const f32 M_HALF_SQRT_TWO   = 0.707106781187f;
global_const f32 M_HALF_SQRT_THREE = 0.866025403784f;

global_const u32 npos = -1;


///////////////////////// 
//// common var sizes////
/////////////////////////
global_const u64 s8size    = sizeof(s8);
global_const u64 s16size   = sizeof(s16);
global_const u64 s32size   = sizeof(s32);
global_const u64 s64size   = sizeof(s64);
global_const u64 sptsize   = sizeof(spt);
global_const u64 u8size    = sizeof(u8);
global_const u64 u16size   = sizeof(u16);
global_const u64 u32size   = sizeof(u32);
global_const u64 u64size   = sizeof(u64);
global_const u64 uptsize   = sizeof(upt);
global_const u64 f32size   = sizeof(f32);
global_const u64 f64size   = sizeof(f64);
global_const u64 b8size    = sizeof(b8);
global_const u64 b32size   = sizeof(b32);
global_const u64 wcharsize = sizeof(wchar);


/////////////////////// //NOTE some are two level so you can use the result of a macro expansion (STRINGIZE, GLUE, etc)
//// common macros ////
///////////////////////
#define STMNT(s) do{ s }while(0)
#define UNUSED_VAR(a) ((void)(a))
#define STRINGIZE_(a) #a
#define STRINGIZE(a) STRINGIZE_(a)
#define GLUE_(a,b) a##b
#define GLUE(a,b) GLUE_(a,b)

#define PRINTLN(x) std::wcout << x << std::endl;
#define __FILENAME__ (std::strrchr(__FILE__, '\\') ? std::strrchr(__FILE__, '\\') + 1 : __FILE__)

#define Kilobytes(a) (((u64)(a)) << 10)
#define Megabytes(a) (((u64)(a)) << 20)
#define Gigabytes(a) (((u64)(a)) << 30)
#define Terabytes(a) (((u64)(a)) << 40)

#define Thousand(a) (((u64)(a)) * 1000)
#define Million(a)  (((u64)(a)) * 1000000)
#define Billion(a)  (((u64)(a)) * 1000000000)

#define Seconds(ms) (ms          / 1000)
#define Minutes(ms) (Seconds(ms) / 60)
#define Hours(ms)   (Minutes(ms) / 60)
#define Days(ms)    (Hours(ms)   / 24)
#define SecondsToMS(n) (n * 1000)
#define MinutesToMS(n) (n * SecondsToMS(60))
#define HoursToMS(n)   (n * MinutesToMS(60))
#define DaysToMS(n)    (n * HoursToMS(24))

#define Radians(a) ((a) * (M_PI / 180.f))
#define Degrees(a) ((a) * (180.f / M_PI))

#define ToggleBool(variable) variable = !variable
#define ArrayCount(arr) (sizeof((arr)) / sizeof(((arr))[0])) //length of a static-size c-array
#define RoundUpTo(value,multiple) (((upt)((value) + (((upt)(multiple))-1)) / (upt)(multiple)) * (upt)(multiple))
#define RoundDownTo(value,multiple) ((upt)(value) - (((upt)(value)) % (upt)(multiple)))
#define AlignToPow2(value,power) (((value) + ((power)-1)) & ~((power)-1))
#define PackU32(x,y,z,w) (((u32)(x) << 24) | ((u32)(y) << 16) | ((u32)(z) << 8) | ((u32)(w) << 0))

#define PointerDifference(a,b) ((u8*)(a) - (u8*)(b))
#define PointerAsInt(a) PointerDifference(a,0)
#define OffsetOfMember(structName,memberName) PointerAsInt(&(((structName*)0)->memberName))
#define CastFromMember(structName,memberName,ptr) ((structName*)((u8*)(ptr) - OffsetOfMember(structName,memberName)))

#define StartNamespace(a) namespace a{
#define EndNamespace(a) }

#define CastToConst(type,a) const_cast<const type>(a)
#define CastFromConst(type,a) const_cast<type>(a)
#define StaticCast(type,a) static_cast<type>(a)
#define DynamicCast(type,a) dynamic_cast<type>(a)
#define Reinterpret(to_type,from_type,x) union{from_type a; to_type b;}{(x)}.b 

#define HasFlag(var,flag) ((var) & (flag))
#define HasAllFlags(var,flags) (((var) & (flags)) == (flags))
#define AddFlag(var,flag) ((var) |= (flag))
#define RemoveFlag(var,flag) ((var) &= (~(flag)))
#define ToggleFlag(var,flag) ((var) ^= (flag))

#define ReadBits64(var,start,numbits) ((var) >> (start)) & (((u64)1 << (numbits)) - 1)
#define ReadBits32(var,start,numbits) ((var) >> (start)) & (((u32)1 << (numbits)) - 1)
#define ReadBits16(var,start,numbits) ((var) >> (start)) & (((u16)1 << (numbits)) - 1)
#define ReadBits8 (var,start,numbits) ((var) >> (start)) & (((u8) 1 << (numbits)) - 1)

#define ENUM_LIST_ENUM(prefix,raw_enum) prefix##_##raw_enum
#define ENUM_LIST_STRING(prefix,raw_enum) STR8(STRINGIZE(raw_enum))
#define ENUM_LIST(list_name, list_macro)                           \
  typedef enum list_name{ list_macro( ENUM_LIST_ENUM ) }list_name; \
  str8 list_name##_strings[] = { list_macro( ENUM_LIST_STRING ) }

//// linkage macros ////
#if COMPILER_FEATURE_CPP
#  define external extern "C"
#  define StartLinkageC() extern "C"{
#  define EndLinkageC() }
#else
#  define StartLinkageC()
#  define EndLinkageC()
#endif //#if COMPILER_FEATURE_CPP


//////////////////////////
//// common functions ////
//////////////////////////
FORCE_INLINE void ZeroMemory(void* ptr, upt bytes){memset(ptr, 0, bytes);}
FORCE_INLINE void CopyMemory(void* dst, void* src, upt bytes){memcpy(dst,src,bytes);}
FORCE_INLINE void MoveMemory(void* dst, void* src, upt bytes){memmove(dst,src,bytes);}

FORCE_INLINE b32 IsPow2(u64 value){return (value != 0) && ((value & (value-1)) == 0);}
FORCE_INLINE upt roundUpToPow2(upt x){return (upt)1 << (upt)((upt)log2(f64(--x)) + 1); }

FORCE_INLINE str8 bytesUnit(upt bytes){return (bytes > Kilobytes(1) ? bytes > Megabytes(1) ? bytes > Gigabytes(1) ? bytes > Terabytes(1) ? STR8("TB") : STR8("GB") : STR8("MB") : STR8("KB") : STR8("B")); }
FORCE_INLINE f32 bytesDivisor(upt bytes){return (bytes > Kilobytes(1) ? bytes > Megabytes(1) ? bytes > Gigabytes(1) ? bytes > Terabytes(1) ? Terabytes(1) : Gigabytes(1) : Megabytes(1) : Kilobytes(1) : 1); }

template<typename T> FORCE_INLINE void Swap(T& a, T& b){T temp = a; a = b; b = temp;}

template<typename T> FORCE_INLINE T Min(T a, T b){return (a < b) ? a : b;}
template<typename T> FORCE_INLINE T Max(T a, T b){return (a > b) ? a : b;}
template<typename T,typename U> FORCE_INLINE T Min(T a, U b){return (a < b) ? a : b;}
template<typename T,typename U> FORCE_INLINE T Max(T a, U b){return (a > b) ? a : b;}

template<typename T> FORCE_INLINE T Clamp(T value, T min, T max){return (value < min) ? min : ((value > max) ? max : value);};
template<typename T,typename U> FORCE_INLINE T Clamp(T value, U min, T max){return (value < min) ? min : ((value > max) ? max : value);}
template<typename T,typename U> FORCE_INLINE T Clamp(T value, T min, U max){return (value < min) ? min : ((value > max) ? max : value);}
template<typename T,typename U> FORCE_INLINE T Clamp(T value, U min, U max){return (value < min) ? min : ((value > max) ? max : value);}
template<typename T> FORCE_INLINE T ClampMin(T value, T min){return (value < min) ? min : value;};
template<typename T> FORCE_INLINE T ClampMax(T value, T max){return (value > max) ? max : value;};
template<typename T,typename U> FORCE_INLINE T ClampMin(T value, U min){return (value < min) ? min : value;};
template<typename T,typename U> FORCE_INLINE T ClampMax(T value, U max){return (value > max) ? max : value;};

template<typename T> FORCE_INLINE T Nudge(T val, T target, T delta){ return (val != target) ? (val < target) ? ((val + delta < target) ? val + delta : target) : ((val - delta > target) ? val - delta : target) : target;}
template<typename T> FORCE_INLINE b32 EpsilonEqual(T a, T b){ return abs(a - b) < M_EPSILON;}
template<typename T> FORCE_INLINE T Remap(T val, T nu_min, T nu_max, T old_min, T old_max){return (val - old_min) / (old_max - old_min) * (nu_max - nu_min) + nu_min;}

template<typename... T, typename A> FORCE_INLINE b32 match_any(A tested, T... in){return((tested == in) || ...);}
template<typename... T, typename A> FORCE_INLINE b32 less_than_any(A tested, T... in){return((tested < in) || ...);}
template<typename... T, typename A> FORCE_INLINE b32 greater_than_any(A tested, T... in){return((tested > in) || ...);}

template<typename T> FORCE_INLINE T& deref_if_ptr(T& x){return x;}
template<typename T> FORCE_INLINE T& deref_if_ptr(T* x){return *x;}


/////////////////////// //NOTE the ... is for a programmer message at the assert; it is unused otherwise
//// assert macros //// //TODO(delle) assert message popup thru the OS
/////////////////////// 
#define AssertAlways(expression, ...) STMNT( if(!(expression)){*(volatile int*)0 = 0;} ) //works regardless of SLOW or INTERNAL
#define AssertBreakpoint(expression, ...) STMNT( if(!(expression)){ DebugBreakpoint; } )
#define StaticAssertAlways(expression, ...) char GLUE(GLUE(__ignore__, GLUE(__LINE__,__default__)),__COUNTER__)[(expression)?1:-1]

#if BUILD_INTERNAL
#  define Assert(expression, ...) AssertBreakpoint(expression)
#  define StaticAssert(expression, ...) StaticAssertAlways(expression)
#elif BUILD_SLOW
#  define Assert(expression, ...) AssertAlways(expression)
#  define StaticAssert(expression, ...) StaticAssertAlways(expression)
#else
#  define Assert(expression, ...)
#  define StaticAssert(expression, ...) 
#endif //#if BUILD_INTERNAL

#define NotImplemented AssertAlways(false, "not implemented yet")
#define InvalidPath Assert(false, "invalid path")
#define TestMe AssertBreakpoint(false, "this needs to be tested")
#define FixMe AssertBreakpoint(false, "this is broken in some way")
#define DontCompile (0=__dont_compile_this__)
#define WarnFuncNotImplemented(extra) LogW("FUNC", "Function ", __FUNCTION__, " has not been implemented or is not finished", (extra ? "\n" : ""), extra);


/////////////////////////
//// for-loop macros ////
/////////////////////////
#define forX(var_name,iterations) for(int var_name=0; var_name<(iterations); ++var_name)
#define forX_reverse(var_name,iterations) for(int var_name=(iterations)-1; var_name>=0; --var_name)
#define forI(iterations) for(int i=0; i<(iterations); ++i)
#define forI_reverse(iterations) for(int i=(iterations)-1; i>=0; --i)
#if COMPILER_FEATURE_TYPEOF
#  define For(start,count) for(typeof(*(start))* it = start; it < start+(count); ++it)
#  define ForX(var_name,start,count) for(typeof(*(start))* var_name = start; var_name < start+(count); ++var_name)
#elif COMPILER_FEATURE_CPP
#  define For(start,count) for(auto it = start; it < start+(count); ++it)
#  define ForX(var_name,start,count) for(auto var_name = start; var_name < start+(count); ++var_name)
#  define forE(iterable) for(auto it = iterable.begin(), it_begin = iterable.begin(), it_end = iterable.end(); it != it_end; ++it)
#endif //#if COMPILER_FEATURE_TYPEOF

//// stb array ////
#if COMPILER_FEATURE_TYPEOF
#  define for_stb_array(a) for(typeof(*(a))* it = a; it < a+arrlen(a); ++it)
#  define forX_stb_array(x,a) for(typeof(*(a))* x = a; x < a+arrlen(a); ++x)
#elif COMPILER_FEATURE_CPP
#  define for_stb_array(a) for(auto it = a; it < a+arrlen(a); ++it)
#  define forX_stb_array(x,a) for(auto x = a; x < a+arrlen(a); ++x)
#endif //#if COMPILER_FEATURE_TYPEOF


///////////////
//// other ////
///////////////
//// compile time sizeof() ////NOTE(delle) just for easy reference, not meant to be used in actual code 
//compiler will give an error with the size of the object
//char (*__kaboom)[sizeof( YourTypeHere )] = 1;

//// defer statement ////
//ref: https://stackoverflow.com/a/42060129 by pmttavara
//defers execution inside the block to the end of the current scope; this works by placing
//that code in a lambda specific to that line that a dummy object will call in its destructor
#ifndef defer
struct defer_dummy {};
template <class F> struct deferrer { F f; ~deferrer() { f(); } };
template <class F> deferrer<F> operator*(defer_dummy, F f) { return {f}; }
#  define DEFER_(LINE) zz_defer##LINE
#  define DEFER(LINE) DEFER_(LINE)
#  define defer auto DEFER(__LINE__) = defer_dummy{} *[&]()
#endif //#ifndef defer

//// C/C++ STL allocator //// //TODO rename this to libc allocator (STL is something different)
global void* STLAllocator_Reserve(upt size){void* a = calloc(1,size); Assert(a); return a;}
global void  STLAllocator_Release(void* ptr){free(ptr);}
global void* STLAllocator_Resize(void* ptr, upt size){void* a = realloc(ptr,size); Assert(a); return a;}
global Allocator stl_allocator_{
	STLAllocator_Reserve,
	STLAllocator_Release,
	STLAllocator_Resize
};
global Allocator* stl_allocator = &stl_allocator_;


///////////////////////////// //TODO remove/rework/rename these
//// to-be-redone macros ////
/////////////////////////////
#define cpystr(dst,src,bytes) strncpy((dst), (src), (bytes)); (dst)[(bytes)-1] = '\0' //copy c-string and null-terminate
#define dyncast(child,base) dynamic_cast<child*>(base) //dynamic cast short-hand
#define PRINTBASICTYPESIZES PRINTLN("   s8 size: " << s8size); PRINTLN("  s16 size: " << s16size); PRINTLN("  s32 size: " << s32size); PRINTLN("  s64 size: " << s64size); PRINTLN("  spt size: " << sptsize); PRINTLN("   u8 size: " << u8size); PRINTLN("  u16 size: " << u16size); PRINTLN("  u32 size: " << u32size); PRINTLN("  u64 size: " << u64size); PRINTLN("  upt size: " << uptsize); PRINTLN("  f32 size: " << f32size); PRINTLN("  f64 size: " << f64size); PRINTLN("  b32 size: " << b32size); PRINTLN("wchar size: " << wcharsize);

#endif //KIGU_COMMON_H
