#pragma once
#ifndef KIGU_STRING_UTILS_H
#define KIGU_STRING_UTILS_H

#include "arrayT.h"
#include "string.h"
#include "cstring.h"
#include "color.h"
#include "common.h"
#include "unicode.h"

#include <cstdarg>
#include <string>

global string 
stringf(const char* fmt, ...){
	va_list argptr;
	va_start(argptr, fmt);
	string s;
	s.count = vsnprintf(nullptr, 0, fmt, argptr);
	s.str   = (char*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
	s.allocator->commit(s.str, s.count+1);
	s.space = s.count+1;
	vsnprintf(s.str, s.count+1, fmt, argptr);
	va_end(argptr);
	return s;
}

global string 
stringf(Allocator* a, const char* fmt, ...){
	va_list argptr;
	va_start(argptr, fmt);
	string s(a);
	s.count = vsnprintf(nullptr, 0, fmt, argptr);
	s.str   = (char*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
	s.allocator->commit(s.str, s.count+1);
	s.space = s.count+1;
	vsnprintf(s.str, s.count+1, fmt, argptr);
	va_end(argptr);
	return s;
}

///////////////
//// @stox ////
///////////////
global s32
stoi(char* s){
	s32 x;
	(void)sscanf(s, "%d", &x);
	return x;
}
FORCE_INLINE s32 stoi(cstring s){ return stoi(s.str); }
FORCE_INLINE s32 stoi(const string& s){ return stoi(s.str); }
FORCE_INLINE s32 stoi(str8 s){return stoi((char*)s.str);}

global s64
stolli(char* s){
	s64 x;
	(void)sscanf(s, "%lli", &x);
	return x;
}
FORCE_INLINE s64 stolli(cstring s){ return stolli(s.str); }
FORCE_INLINE s64 stolli(const string& s){ return stolli(s.str); }
FORCE_INLINE s64 stolli(str8 s) { return stolli((char*)s.str); }


global f64
stod(char* s) {
	return strtod(s, 0);
}
FORCE_INLINE f64 stod(cstring s){ return stod(s.str); }
FORCE_INLINE f64 stod(const string& s){ return stod(s.str); }
FORCE_INLINE f64 stod(str8 s){ return stod((char*)s.str); }

////////////////////
//// @to_string ////
////////////////////
global string 
to_string(cstring x, Allocator* a = KIGU_STRING_ALLOCATOR){
	return string(x.str, x.count, a);
}

global string 
to_string(char* str, Allocator* a = KIGU_STRING_ALLOCATOR){ 
	return string(str, a);
}

global string 
to_string(const char* str, Allocator* a = KIGU_STRING_ALLOCATOR){ 
	return string(str, a);
}

global string 
to_string(const string& str, Allocator* a = KIGU_STRING_ALLOCATOR){ 
	return string(str, a);
}

global string 
to_string(const std::string& str, Allocator* a = KIGU_STRING_ALLOCATOR){ 
	return string(str.c_str(), str.size(), a); 
}

global string 
to_string(char x, Allocator* a = KIGU_STRING_ALLOCATOR){
	return string(&x, 1, a);
}

global string 
to_string(s32 x, Allocator* a = KIGU_STRING_ALLOCATOR){
	string s(a);
	s.count = snprintf(nullptr, 0, "%d", x);
	s.str   = (char*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
	s.allocator->commit(s.str, s.count+1);
	s.space = s.count+1;
	snprintf(s.str, s.count+1, "%d", x);
	return s;
}

global string 
to_string(s64 x, Allocator* a = KIGU_STRING_ALLOCATOR){
	string s(a);
	s.count = snprintf(nullptr, 0, "%lld", x);
	s.str   = (char*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
	s.allocator->commit(s.str, s.count+1);
	s.space = s.count+1;
	snprintf(s.str, s.count+1, "%lld", x);
	return s;
}

global string 
to_string(u32 x, Allocator* a = KIGU_STRING_ALLOCATOR){
	string s(a);
	s.count = snprintf(nullptr, 0, "%u", x);
	s.str   = (char*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
	s.allocator->commit(s.str, s.count+1);
	s.space = s.count+1;
	snprintf(s.str, s.count+1, "%u", x);
	return s;
}

global string 
to_string(u64 x, Allocator* a = KIGU_STRING_ALLOCATOR){
	string s(a);
	s.count = snprintf(nullptr, 0, "%llu", x);
	s.str   = (char*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
	s.allocator->commit(s.str, s.count+1);
	s.space = s.count+1;
	snprintf(s.str, s.count+1, "%llu", x);
	return s;
}

global string 
to_string(long x, Allocator* a = KIGU_STRING_ALLOCATOR){
	string s(a);
	s.count = snprintf(nullptr, 0, "%ld", x);
	s.str   = (char*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
	s.allocator->commit(s.str, s.count+1);
	s.space = s.count+1;
	snprintf(s.str, s.count+1, "%ld", x);
	return s;
}

global string 
to_string(f32 x, bool trunc = true, Allocator* a = KIGU_STRING_ALLOCATOR){
	string s(a);
	if(trunc){
		s.count = snprintf(nullptr, 0, "%g", x);
		s.str   = (char*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
		s.allocator->commit(s.str, s.count+1);
		s.space = s.count+1;
		snprintf(s.str, s.count+1, "%g", x);
	}else{
		s.count = snprintf(nullptr, 0, "%f", x);
		s.str   = (char*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
		s.allocator->commit(s.str, s.count+1);
		s.space = s.count+1;
		snprintf(s.str, s.count+1, "%f", x);
	}
	return s;
}

global string 
to_string(f64 x, bool trunc = true, Allocator* a = KIGU_STRING_ALLOCATOR){
	string s(a);
	if(trunc){
		s.count = snprintf(nullptr, 0, "%g", x);
		s.str   = (char*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
		s.allocator->commit(s.str, s.count+1);
		s.space = s.count+1;
		snprintf(s.str, s.count+1, "%g", x);
	}else{
		s.count = snprintf(nullptr, 0, "%f", x);
		s.str   = (char*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
		s.allocator->commit(s.str, s.count+1);
		s.space = s.count+1;
		snprintf(s.str, s.count+1, "%f", x);
	}
	return s;
}

global string 
to_string(upt x, Allocator* a = KIGU_STRING_ALLOCATOR){
	string s(a);
	s.count = snprintf(nullptr, 0, "%zu", x);
	s.str   = (char*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
	s.allocator->commit(s.str, s.count+1);
	s.space = s.count+1;
	snprintf(s.str, s.count+1, "%zu", x);
	return s;
}

global string
to_string(void* ptr, Allocator* a = KIGU_STRING_ALLOCATOR) {
	string s(a);
	s.count = snprintf(nullptr, 0, "%p", ptr);
	s.str = (char*)s.allocator->reserve(s.count + 1); Assert(s.str, "Failed to allocate memory");
	s.allocator->commit(s.str, s.count + 1);
	s.space = s.count + 1;
	snprintf(s.str, s.count + 1, "%p", ptr);
	return s;
}

global string 
to_string(const color& x, Allocator* a = KIGU_STRING_ALLOCATOR){
	string s(a);
	s.count = snprintf(nullptr, 0, "{R:%u, G:%u, B:%u, A:%u}", x.r, x.g, x.b, x.a);
	s.str   = (char*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
	s.allocator->commit(s.str, s.count+1);
	s.space = s.count+1;
	snprintf(s.str, s.count+1, "{R:%u, G:%u, B:%u, A:%u}", x.r, x.g, x.b, x.a);
	return s;
}

global string
to_string(str8 s, Allocator* a = KIGU_STRING_ALLOCATOR){
	return string((char*)s.str, s.count, a);
}

#define toStr(...) (ToString(__VA_ARGS__))
template<class... T> global string 
ToString(T... args){
	string str;
	constexpr auto arg_count{sizeof...(T)};
	string arr[arg_count] = {to_string(args)...};
	forI(arg_count) str += arr[i];
	return str;
}

/////////////////////
//// @to_string8 ////
/////////////////////


global str8
to_str8(const char* str, Allocator* a = KIGU_STRING_ALLOCATOR){ DPZoneScoped;
	str8b out; str8_builder_init(&out, {(u8*)str, (s32)strlen(str)}, a);
	return out.fin;
}


global str8
to_str8(const std::string& str, Allocator* a = KIGU_STRING_ALLOCATOR){ DPZoneScoped;
	str8b out; str8_builder_init(&out, {(u8*)str.c_str(), (s32)str.size()}, a);
	return out.fin;
}

global str8
to_str8(char x, Allocator* a = KIGU_STRING_ALLOCATOR){DPZoneScoped;
	str8b out; str8_builder_init(&out, {(u8*)&x, 1}, a);
	return out.fin;
}

global str8
to_str8(s32 x, Allocator* a = KIGU_STRING_ALLOCATOR){DPZoneScoped;
	str8b s; s.allocator = a;
	s.count = snprintf(nullptr, 0, "%d", x);
	s.str   = (u8*)a->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
	a->commit(s.str, s.count+1);
	s.space = s.count+1;
	snprintf((char*)s.str, s.count+1, "%d", x);
	return s.fin;
}

global str8
to_str8(s64 x, Allocator* a = KIGU_STRING_ALLOCATOR){DPZoneScoped;
	str8b s; s.allocator = a;
	s.count = snprintf(nullptr, 0, "%lld", x);
	s.str   = (u8*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
	s.allocator->commit(s.str, s.count+1);
	s.space = s.count+1;
	snprintf((char*)s.str, s.count+1, "%lld", x);
	return s.fin;
}

global str8
to_str8(u32 x, Allocator* a = KIGU_STRING_ALLOCATOR){DPZoneScoped;
	str8b s; s.allocator = a;
	s.count = snprintf(nullptr, 0, "%u", x);
	s.str   = (u8*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
	s.allocator->commit(s.str, s.count+1);
	s.space = s.count+1;
	snprintf((char*)s.str, s.count+1, "%u", x);
	return s.fin;
}

global str8
to_str8(u64 x, Allocator* a = KIGU_STRING_ALLOCATOR){DPZoneScoped;
	str8b s; s.allocator = a;
	s.count = snprintf(nullptr, 0, "%llu", x);
	s.str   = (u8*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
	s.allocator->commit(s.str, s.count+1);
	s.space = s.count+1;
	snprintf((char*)s.str, s.count+1, "%llu", x);
	return s.fin;
}

// NOTE(sushi) required for clang because it finds it to be ambiguous with 
global str8
to_str8(long x, Allocator* a = KIGU_STRING_ALLOCATOR){DPZoneScoped;
	str8b s; s.allocator = a;
	s.count = snprintf(nullptr, 0, "%ld", x);
	s.str   = (u8*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
	s.allocator->commit(s.str, s.count+1);
	s.space = s.count+1;
	snprintf((char*)s.str, s.count+1, "%ld", x);
	return s.fin;
}

global str8
to_str8(f32 x, bool trunc = true, Allocator* a = KIGU_STRING_ALLOCATOR){DPZoneScoped;
	str8b s; s.allocator = a;
	if(trunc){
		s.count = snprintf(nullptr, 0, "%g", x);
		s.str   = (u8*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
		s.allocator->commit(s.str, s.count+1);
		s.space = s.count+1;
		snprintf((char*)s.str, s.count+1, "%g", x);
	}else{
		s.count = snprintf(nullptr, 0, "%f", x);
		s.str   = (u8*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
		s.allocator->commit(s.str, s.count+1);
		s.space = s.count+1;
		snprintf((char*)s.str, s.count+1, "%f", x);
	}
	return s.fin;
}

global str8
to_str8(f32 x, Allocator* a = KIGU_STRING_ALLOCATOR){DPZoneScoped;
	return to_str8(x, 1, a);
}

global str8
to_str8(f64 x, bool trunc = true, Allocator* a = KIGU_STRING_ALLOCATOR){DPZoneScoped;
	str8b s; s.allocator = a;
	if(trunc){
		s.count = snprintf(nullptr, 0, "%g", x);
		s.str   = (u8*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
		s.allocator->commit(s.str, s.count+1);
		s.space = s.count+1;
		snprintf((char*)s.str, s.count+1, "%g", x);
	}else{
		s.count = snprintf(nullptr, 0, "%f", x);
		s.str   = (u8*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
		s.allocator->commit(s.str, s.count+1);
		s.space = s.count+1;
		snprintf((char*)s.str, s.count+1, "%f", x);
	}
	return s.fin;
}

global str8
to_str8(f64 x, Allocator* a = KIGU_STRING_ALLOCATOR){DPZoneScoped;
	return to_str8(x, 1, a);
}

global str8
to_str8(upt x, Allocator* a = KIGU_STRING_ALLOCATOR){DPZoneScoped;
	str8b s; s.allocator = a;
	s.count = snprintf(nullptr, 0, "%zu", x);
	s.str   = (u8*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
	s.allocator->commit(s.str, s.count+1);
	s.space = s.count+1;
	snprintf((char*)s.str, s.count+1, "%zu", x);
	return s.fin;
}

global str8
to_str8(void* ptr, Allocator* a = KIGU_STRING_ALLOCATOR) {DPZoneScoped;
	str8b s; s.allocator = a;
	s.count = snprintf(nullptr, 0, "%p", ptr);
	s.str = (u8*)s.allocator->reserve(s.count + 1); Assert(s.str, "Failed to allocate memory");
	s.allocator->commit(s.str, s.count + 1);
	s.space = s.count + 1;
	snprintf((char*)s.str, s.count + 1, "%p", ptr);
	return s.fin;
}

global str8
to_str8(const color& x, Allocator* a = KIGU_STRING_ALLOCATOR){DPZoneScoped;
	str8b s; s.allocator = a;
	s.count = snprintf(nullptr, 0, "{R:%u, G:%u, B:%u, A:%u}", x.r, x.g, x.b, x.a);
	s.str   = (u8*)s.allocator->reserve(s.count+1); Assert(s.str, "Failed to allocate memory");
	s.allocator->commit(s.str, s.count+1);
	s.space = s.count+1;
	snprintf((char*)s.str, s.count+1, "{R:%u, G:%u, B:%u, A:%u}", x.r, x.g, x.b, x.a);
	return s.fin;
}

global str8
to_str8(const str8& x, Allocator* a = KIGU_STRING_ALLOCATOR){DPZoneScoped;
	str8b s; str8_builder_init(&s, x, a);
	return s.fin;
}

template<class... T> global str8
ToString8(Allocator* allocator, T... args){DPZoneScoped;
	str8b str; str8_builder_init(&str, {0}, allocator);
	constexpr auto arg_count{sizeof...(T)};
	str8 arr[arg_count] = {to_str8(args, allocator)...};
	forI(arg_count) str8_builder_append(&str, arr[i]);
	return str.fin;
}

///////////////
//// @find ////
///////////////
//NOTE these functions do not do any safety/bounds checking
//TODO decide if we should do that here
global u32 
find_first_char(const char* str, u32 strsize, char c, u32 offset = 0) {
	for (u32 i = offset; i < strsize; ++i)
		if (str[i] == c) return i;
	return npos;
}
global u32 find_first_char(const char* str, char c, u32 offset = 0)    { return find_first_char(str, strlen(str), c, offset); } 
global u32 find_first_char(const cstring& str, char c, u32 offset = 0) { return find_first_char(str.str, str.count, c, offset); } 
global u32 find_first_char(const string& str, char c, u32 offset = 0)  { return find_first_char(str.str, str.count, c, offset); }

global u32 
find_first_char_not(const char* str, u32 strsize, char c, u32 offset = 0) {
	for (u32 i = 0; i < strsize; ++i)
		if (str[i] != c) return i;
	return npos;
}
global u32 find_first_char_not(const char*    str, char c, u32 offset = 0) { return find_first_char_not(str, strlen(str), c, offset); }
global u32 find_first_char_not(const cstring& str, char c, u32 offset = 0) { return find_first_char_not(str.str, str.count, c, offset); }
global u32 find_first_char_not(const string&  str, char c, u32 offset = 0) { return find_first_char_not(str.str, str.count, c, offset); }

global u32 
find_last_char(const char* str, u32 strsize, char c, u32 offset = 0) {
	for (u32 i = (offset != 0 ? offset : strsize - 1); i != 0; --i)
		if (str[i] == c) return i;
	return npos;
}
global u32 find_last_char(const char*    str, char c, u32 offset = 0) { return find_last_char(str, strlen(str), c, offset); } 
global u32 find_last_char(const cstring& str, char c, u32 offset = 0) { return find_last_char(str.str, str.count, c, offset); } 
global u32 find_last_char(const string&  str, char c, u32 offset = 0) { return find_last_char(str.str, str.count, c, offset); }

global u32 
find_last_char_not(const char* str, u32 strsize, char c, u32 offset = 0) {
	for (u32 i = strsize - 1; i != 0; --i)
		if (str[i] != c) return i;
	return npos;
}
global u32 find_last_char_not(const char*    str, char c, u32 offset = 0) { return find_last_char_not(str, strlen(str), c, offset); }
global u32 find_last_char_not(const cstring& str, char c, u32 offset = 0) { return find_last_char_not(str.str, str.count, c, offset); }
global u32 find_last_char_not(const string&  str, char c, u32 offset = 0) { return find_last_char_not(str.str, str.count, c, offset); }

global u32 
find_first_string(const char* str, u32 strsize, const char* str2, u32 str2size, u32 offset = 0) {
	if (str2size > strsize) return npos;
	for (u32 i = offset; i < strsize - str2size; i++)
		if (!memcmp(str + i, str2, str2size)) return i;
	return npos;
}
global u32 find_first_string(const string&  buf1, const string&  buf2, u32 offset = 0) { return find_first_string(buf1.str, buf1.count, buf2.str, buf2.count, offset); }
global u32 find_first_string(const cstring& buf1, const cstring& buf2, u32 offset = 0) { return find_first_string(buf1.str, buf1.count, buf2.str, buf2.count, offset); }
global u32 find_first_string(const string&  buf1, const cstring& buf2, u32 offset = 0) { return find_first_string(buf1.str, buf1.count, buf2.str, buf2.count, offset); }
global u32 find_first_string(const cstring& buf1, const string&  buf2, u32 offset = 0) { return find_first_string(buf1.str, buf1.count, buf2.str, buf2.count, offset); }
global u32 find_first_string(const char*    buf1, const string&  buf2, u32 offset = 0) { return find_first_string(buf1, strlen(buf1), buf2.str, buf2.count, offset); }
global u32 find_first_string(const string&  buf1, const char*    buf2, u32 offset = 0) { return find_first_string(buf1.str, buf1.count, buf2, strlen(buf2), offset); }
global u32 find_first_string(const char*    buf1, const cstring& buf2, u32 offset = 0) { return find_first_string(buf1, strlen(buf1), buf2.str, buf2.count, offset); }
global u32 find_first_string(const cstring& buf1, const char*    buf2, u32 offset = 0) { return find_first_string(buf1.str, buf1.count, buf2, strlen(buf2), offset); }
global u32 find_first_string(const char*    buf1, const char*    buf2, u32 offset = 0) { return find_first_string(buf1, strlen(buf1), buf2, strlen(buf2), offset); }

FORCE_INLINE global b32
str_begins_with(const char* buf1, const char* buf2, u32 buf2len) {return !memcmp(buf1, buf2, buf2len);}
global b32 str_begins_with(const string&  buf1, const string&  buf2) { return str_begins_with(buf1.str, buf2.str, buf2.count); }
global b32 str_begins_with(const cstring& buf1, const cstring& buf2) { return str_begins_with(buf1.str, buf2.str, buf2.count); }
global b32 str_begins_with(const string&  buf1, const cstring& buf2) { return str_begins_with(buf1.str, buf2.str, buf2.count); }
global b32 str_begins_with(const cstring& buf1, const string&  buf2) { return str_begins_with(buf1.str, buf2.str, buf2.count); }
global b32 str_begins_with(const char*    buf1, const string&  buf2) { return str_begins_with(buf1, buf2.str, buf2.count); }
global b32 str_begins_with(const string&  buf1, const char*    buf2) { return str_begins_with(buf1.str, buf2, strlen(buf2)); }
global b32 str_begins_with(const char*    buf1, const cstring& buf2) { return str_begins_with(buf1, buf2.str, buf2.count); }
global b32 str_begins_with(const cstring& buf1, const char*    buf2) { return str_begins_with(buf1.str, buf2, strlen(buf2)); }
global b32 str_begins_with(const char*    buf1, const char*    buf2) { return str_begins_with(buf1, buf2, strlen(buf2)); }

FORCE_INLINE global b32
str_ends_with(const char* buf1, u32 buf1len, const char* buf2, u32 buf2len) {return (buf2len > buf1len ? 0 : !memcmp(buf1 + (buf1len - buf2len), buf2, buf2len));}
global b32 str_ends_with(const string&  buf1, const string&  buf2) { return str_ends_with(buf1.str, buf1.count, buf2.str, buf2.count); }
global b32 str_ends_with(const cstring& buf1, const cstring& buf2) { return str_ends_with(buf1.str, buf1.count, buf2.str, buf2.count); }
global b32 str_ends_with(const string&  buf1, const cstring& buf2) { return str_ends_with(buf1.str, buf1.count, buf2.str, buf2.count); }
global b32 str_ends_with(const cstring& buf1, const string&  buf2) { return str_ends_with(buf1.str, buf1.count, buf2.str, buf2.count); }
global b32 str_ends_with(const char*    buf1, const string&  buf2) { return str_ends_with(buf1, strlen(buf1), buf2.str, buf2.count); }
global b32 str_ends_with(const string&  buf1, const char*    buf2) { return str_ends_with(buf1.str, buf1.count, buf2, strlen(buf2)); }
global b32 str_ends_with(const char*    buf1, const cstring& buf2) { return str_ends_with(buf1, strlen(buf1), buf2.str, buf2.count); }
global b32 str_ends_with(const cstring& buf1, const char*    buf2) { return str_ends_with(buf1.str, buf1.count, buf2, strlen(buf2)); }
global b32 str_ends_with(const char*    buf1, const char*    buf2) { return str_ends_with(buf1, strlen(buf1), buf2, strlen(buf2)); }

global b32
str_contains(const char* buf1, u32 buf1len, const char* buf2, u32 buf2len) {
	if (buf2len > buf1len) return false;
	for (u32 i = 0; i < buf2len - buf1len; i++) {
		if (!memcmp(buf2, buf1 + i, buf2len)) return true;
	}
	return false;
}
global b32 str_contains(const string&  buf1, const string&  buf2) { return str_contains(buf1.str, buf1.count, buf2.str, buf2.count); }
global b32 str_contains(const cstring& buf1, const cstring& buf2) { return str_contains(buf1.str, buf1.count, buf2.str, buf2.count); }
global b32 str_contains(const string&  buf1, const cstring& buf2) { return str_contains(buf1.str, buf1.count, buf2.str, buf2.count); }
global b32 str_contains(const cstring& buf1, const string&  buf2) { return str_contains(buf1.str, buf1.count, buf2.str, buf2.count); }
global b32 str_contains(const char*    buf1, const string&  buf2) { return str_contains(buf1, strlen(buf1), buf2.str, buf2.count); }
global b32 str_contains(const string&  buf1, const char*    buf2) { return str_contains(buf1.str, buf1.count, buf2, strlen(buf2)); }
global b32 str_contains(const char*    buf1, const cstring& buf2) { return str_contains(buf1, strlen(buf1), buf2.str, buf2.count); }
global b32 str_contains(const cstring& buf1, const char*    buf2) { return str_contains(buf1.str, buf1.count, buf2, strlen(buf2)); }
global b32 str_contains(const char*    buf1, const char*    buf2) { return str_contains(buf1, strlen(buf1), buf2, strlen(buf2)); }

//includes start and end indexes
global cstring
substr(char* buf, u32 buflen, u32 start, u32 end = npos){
	return cstring{buf+start, (end==npos ? buflen-start : (end-start)+1)};
}
global cstring substr(const cstring& buf, u32 start, u32 end = npos){return substr(buf.str, buf.count, start,end);}
global cstring substr(const string&  buf, u32 start, u32 end = npos){return substr(buf.str, buf.count, start,end);}
global cstring substr(const char*    buf, u32 start, u32 end = npos){return substr((char*)buf, strlen(buf), start,end);}

global arrayT<cstring>
chunkstr(char* buf, u32 buflen, char delimiter){
	arrayT<cstring> chunks;
	char* start = buf;
	forI(buflen){
		if(buf[i]==delimiter){
			chunks.add({start, upt((buf+i)-start)});
			start=buf+i+1;
		}else if(i==buflen-1){
			chunks.add({start, upt((buf+i+1)-start)});
		}
	}
	return chunks;
}
global arrayT<cstring> chunkstr(const string&  buf, char delimiter) {return chunkstr(buf.str, buf.count, delimiter);}
global arrayT<cstring> chunkstr(const cstring& buf, char delimiter) {return chunkstr(buf.str, buf.count, delimiter);}
global arrayT<cstring> chunkstr(const char*    buf, char delimiter) {return chunkstr((char*)buf, strlen(buf), delimiter);}

#endif //KIGU_STRING_UTILS_H
