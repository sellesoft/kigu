#pragma once
#ifndef KIGU_STRING_UTILS_H
#define KIGU_STRING_UTILS_H

#include "arrayT.h"
#include "cstring.h"
#include "color.h"
#include "common.h"
#include "unicode.h"

#include <cstdarg>
#include <string>
#include <type_traits>

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
FORCE_INLINE s32 stoi(str8 s){return stoi((char*)s.str);}

global s64
stolli(char* s){
	s64 x;
	(void)sscanf(s, "%lli", &x);
	return x;
}
FORCE_INLINE s64 stolli(cstring s){ return stolli(s.str); }
FORCE_INLINE s64 stolli(str8 s) { return stolli((char*)s.str); }


global f64
stod(char* s) {
	return strtod(s, 0);
}
FORCE_INLINE f64 stod(cstring s){ return stod(s.str); }
FORCE_INLINE f64 stod(str8 s){ return stod((char*)s.str); }

// https://stackoverflow.com/questions/301330/determine-if-type-is-a-pointer-in-a-template-function
template<typename T> struct is_ptr { static const bool value = false; };
template<typename T> struct is_ptr<T*> { static const bool value = true; };

/////////////////////
//// @to_string8 ////
/////////////////////
template<typename T> global dstr8
to_dstr8(T x, Allocator* a = KIGU_STRING_ALLOCATOR){DPZoneScoped;
	dstr8 builder = {};
	builder.allocator = a;
	if       constexpr(std::is_same_v<T, char*> || std::is_same_v<T, const char*>){
		dstr8_init(&builder, {(u8*)x, (s64)strlen(x)}, a);
	}else if constexpr(std::is_same_v<T, str8> || std::is_same_v<T, const str8&>){
		dstr8_init(&builder, x, a);
	}else if constexpr(std::is_same_v<T, dstr8>) {
		dstr8_init(&builder, x.fin, a);
	}else if constexpr(std::is_same_v<T, const std::string&>){
		dstr8_init(&builder, {(u8*)x.c_str(), (s64)x.size()}, a);
	}else if constexpr(std::is_same_v<T, char>){
		dstr8_init(&builder, {(u8*)&x, 1}, a);
	}else if constexpr(std::is_same_v<T, unsigned char>) {
		dstr8_init(&builder, {(u8*)&x, 1}, a);
	}else if constexpr(std::is_same_v<T, s32>){
		builder.count = snprintf(nullptr, 0, "%d", x);
		builder.space = builder.count+1;
		builder.str   = (u8*)a->reserve(builder.count+1);
		Assert(builder.str, "Failed to allocate memory");
		snprintf((char*)builder.str, builder.count+1, "%d", x);
	}else if constexpr(std::is_same_v<T, long>){
		builder.count = snprintf(nullptr, 0, "%ld", x);
		builder.space = builder.count+1;
		builder.str   = (u8*)a->reserve(builder.count+1);
		Assert(builder.str, "Failed to allocate memory");
		snprintf((char*)builder.str, builder.count+1, "%ld", x);
	}else if constexpr(std::is_same_v<T, s64>){
		builder.count = snprintf(nullptr, 0, "%lld", x);
		builder.space = builder.count+1;
		builder.str   = (u8*)a->reserve(builder.count+1);
		Assert(builder.str, "Failed to allocate memory");
		snprintf((char*)builder.str, builder.count+1, "%lld", x);
	}else if constexpr(std::is_same_v<T, u32>){
		builder.count = snprintf(nullptr, 0, "%u", x);
		builder.space = builder.count+1;
		builder.str   = (u8*)a->reserve(builder.count+1);
		Assert(builder.str, "Failed to allocate memory");
		snprintf((char*)builder.str, builder.count+1, "%u", x);
	}else if constexpr(std::is_same_v<T, u64>){
		builder.count = snprintf(nullptr, 0, "%llu", x);
		builder.space = builder.count+1;
		builder.str   = (u8*)a->reserve(builder.count+1);
		Assert(builder.str, "Failed to allocate memory");
		snprintf((char*)builder.str, builder.count+1, "%llu", x);
	}else if constexpr(std::is_same_v<T, f32> || std::is_same_v<T, f64>){
		builder.count = snprintf(nullptr, 0, "%g", x);
		builder.space = builder.count+1;
		builder.str   = (u8*)a->reserve(builder.count+1);
		Assert(builder.str, "Failed to allocate memory");
		snprintf((char*)builder.str, builder.count+1, "%g", x);
	}else if constexpr(std::is_same_v<T, upt>){
		builder.count = snprintf(nullptr, 0, "%zu", x);
		builder.space = builder.count+1;
		builder.str   = (u8*)a->reserve(builder.count+1);
		Assert(builder.str, "Failed to allocate memory");
		snprintf((char*)builder.str, builder.count+1, "%zu", x);
	}else if constexpr(std::is_same_v<T, void*>){
		builder.count = snprintf(nullptr, 0, "%p", x);
		builder.space = builder.count+1;
		builder.str   = (u8*)a->reserve(builder.count+1);
		Assert(builder.str, "Failed to allocate memory");
		snprintf((char*)builder.str, builder.count+1, "%p", x);
	}else if constexpr(std::is_same_v<T, color> || std::is_same_v<T, const color&>){
		builder.count = snprintf(nullptr, 0, "{r:%u, g:%u, b:%u, a:%u}", x.r, x.g, x.b, x.a);
		builder.space = builder.count+1;
		builder.str   = (u8*)a->reserve(builder.count+1);
		Assert(builder.str, "Failed to allocate memory");
		snprintf((char*)builder.str, builder.count+1, "{r:%u, g:%u, b:%u, a:%u}", x.r, x.g, x.b, x.a);
	} else if constexpr(is_ptr<T>::value) {
		builder.count = snprintf(nullptr, 0, "%p", (void*)x);
		builder.space = builder.count + 1;
		builder.str = (u8*)a->reserve(builder.count+1);
		Assert(builder.str, "Failed to allocate memory");
		snprintf((char*)builder.str, builder.count+1, "%p", (void*)x);
	} else {
		Assert(0, "unhandled to_dstr8 case");
	}
	return builder;
}

template<class... T> global dstr8
to_dstr8v(Allocator* allocator, T... args){DPZoneScoped;
	dstr8 str; dstr8_init(&str, {0}, allocator);
	constexpr auto arg_count{sizeof...(T)};
	dstr8 arr[arg_count] = { to_dstr8(args, allocator)... };
	forI(arg_count){
		dstr8_append(&str, arr[i].fin);
		dstr8_deinit(&arr[i]);
	} 
	return str;
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

global u32 
find_first_char_not(const char* str, u32 strsize, char c, u32 offset = 0) {
	for (u32 i = 0; i < strsize; ++i)
		if (str[i] != c) return i;
	return npos;
}
global u32 find_first_char_not(const char*    str, char c, u32 offset = 0) { return find_first_char_not(str, strlen(str), c, offset); }
global u32 find_first_char_not(const cstring& str, char c, u32 offset = 0) { return find_first_char_not(str.str, str.count, c, offset); }

global u32 
find_last_char(const char* str, u32 strsize, char c, u32 offset = 0) {
	for (u32 i = (offset != 0 ? offset : strsize - 1); i != 0; --i)
		if (str[i] == c) return i;
	return npos;
}
global u32 find_last_char(const char*    str, char c, u32 offset = 0) { return find_last_char(str, strlen(str), c, offset); } 
global u32 find_last_char(const cstring& str, char c, u32 offset = 0) { return find_last_char(str.str, str.count, c, offset); } 

global u32 
find_last_char_not(const char* str, u32 strsize, char c, u32 offset = 0) {
	for (u32 i = strsize - 1; i != 0; --i)
		if (str[i] != c) return i;
	return npos;
}
global u32 find_last_char_not(const char*    str, char c, u32 offset = 0) { return find_last_char_not(str, strlen(str), c, offset); }
global u32 find_last_char_not(const cstring& str, char c, u32 offset = 0) { return find_last_char_not(str.str, str.count, c, offset); }

global u32 
find_first_string(const char* str, u32 strsize, const char* str2, u32 str2size, u32 offset = 0) {
	if (str2size > strsize) return npos;
	for (u32 i = offset; i < strsize - str2size; i++)
		if (!memcmp(str + i, str2, str2size)) return i;
	return npos;
}
global u32 find_first_string(const cstring& buf1, const cstring& buf2, u32 offset = 0) { return find_first_string(buf1.str, buf1.count, buf2.str, buf2.count, offset); }
global u32 find_first_string(const char*    buf1, const cstring& buf2, u32 offset = 0) { return find_first_string(buf1, strlen(buf1), buf2.str, buf2.count, offset); }
global u32 find_first_string(const cstring& buf1, const char*    buf2, u32 offset = 0) { return find_first_string(buf1.str, buf1.count, buf2, strlen(buf2), offset); }
global u32 find_first_string(const char*    buf1, const char*    buf2, u32 offset = 0) { return find_first_string(buf1, strlen(buf1), buf2, strlen(buf2), offset); }

FORCE_INLINE global b32
str_begins_with(const char* buf1, const char* buf2, u32 buf2len) {return !memcmp(buf1, buf2, buf2len);}
global b32 str_begins_with(const cstring& buf1, const cstring& buf2) { return str_begins_with(buf1.str, buf2.str, buf2.count); }
global b32 str_begins_with(const char*    buf1, const cstring& buf2) { return str_begins_with(buf1, buf2.str, buf2.count); }
global b32 str_begins_with(const cstring& buf1, const char*    buf2) { return str_begins_with(buf1.str, buf2, strlen(buf2)); }
global b32 str_begins_with(const char*    buf1, const char*    buf2) { return str_begins_with(buf1, buf2, strlen(buf2)); }

FORCE_INLINE global b32
str_ends_with(const char* buf1, u32 buf1len, const char* buf2, u32 buf2len) {return (buf2len > buf1len ? 0 : !memcmp(buf1 + (buf1len - buf2len), buf2, buf2len));}
global b32 str_ends_with(const cstring& buf1, const cstring& buf2) { return str_ends_with(buf1.str, buf1.count, buf2.str, buf2.count); }
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
global b32 str_contains(const cstring& buf1, const cstring& buf2) { return str_contains(buf1.str, buf1.count, buf2.str, buf2.count); }
global b32 str_contains(const char*    buf1, const cstring& buf2) { return str_contains(buf1, strlen(buf1), buf2.str, buf2.count); }
global b32 str_contains(const cstring& buf1, const char*    buf2) { return str_contains(buf1.str, buf1.count, buf2, strlen(buf2)); }
global b32 str_contains(const char*    buf1, const char*    buf2) { return str_contains(buf1, strlen(buf1), buf2, strlen(buf2)); }

//includes start and end indexes
global cstring
substr(char* buf, u32 buflen, u32 start, u32 end = npos){
	return cstring{buf+start, (end==npos ? buflen-start : (end-start)+1)};
}
global cstring substr(const cstring& buf, u32 start, u32 end = npos){return substr(buf.str, buf.count, start,end);}
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
global arrayT<cstring> chunkstr(const cstring& buf, char delimiter) {return chunkstr(buf.str, buf.count, delimiter);}
global arrayT<cstring> chunkstr(const char*    buf, char delimiter) {return chunkstr((char*)buf, strlen(buf), delimiter);}

#endif //KIGU_STRING_UTILS_H
