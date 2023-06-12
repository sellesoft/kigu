/* Kigu Unicode Library
Notes:
- str8 is the expected usage type throughout sellesoft projects, and as such, has string functions for it.
- strings are null-terminated (ending in '\0') in order for libc functions to work, but we don't use it
  internally and don't assume they are included in the count (NOTE this assumes the allocators fill memory to zero).
- str16 and str32 use big endian UTF encodings.
- A codepoint can not start with another codepoint, but it can end with another one.
- All functions with allocators expect the memory to be zero filled on allocation.

Terminology:
codepoint       the value or set of values that represent one unicode character
advance         the number of values of the underlying type (u8 for UTF8, u16 for UTF16, etc) a codepoint takes up to represent
fixed-width     the number of values equals the number of characters and the advance is always one (ASCII, UCS2, UTF32)
multibyte       the number of values depends on the character's codepoint and the advance can vary (UTF8, UTF16)
surrogate pair  in UTF16, codepoints above U+010000 require two 16bit code units called surrogate pairs

Index:
@utf_decoding
  DecodedCodepoint
  decoded_codepoint_from_utf8(u8* str, u64 max_advance) -> DecodedCodepoint
  decoded_codepoint_from_utf16(u16* str, u64 max_advance) -> DecodedCodepoint
  decoded_codepoint_from_wchar(whcar_t* str, u64 max_advance) -> DecodedCodepoint
  utf8_from_codepoint(u8* out, u32 codepoint) -> u32
  utf16_from_codepoint(u16* out, u32 codepoint) -> u32
  wchar_from_codepoint(wchar_t* out, u32 codepoint) -> u32
  utf8_continuation_byte(u8 byte) -> b32
@utf_conversion
  str8_from_str16(str16 in, Allocator* allocator) -> str8
  str8_from_str32(str32 in, Allocator* allocator) -> str8
  str8_from_wchar(wchar_t* in, Allocator* allocator) -> str8
  str16_from_str8(str8 in, Allocator* allocator) -> str16
  str32_from_str8(str8 in, Allocator* allocator) -> str32
  wchar_from_str8(str8 in, Allocator* allocator) -> wchar_t*
@str8_advancing
  str8_increment(str8* a, u64 bytes) -> void
  str8_decrement(str8* a, u64 bytes) -> void
  str8_advance(str8* a) -> DecodedCodepoint
  str8_nadvance(str8* a, u64 n) -> DecodedCodepoint
  str8_advance_until(str8* a, u32 c) -> void
  str8_advance_while(str8* a, u32 c) -> void
@str8_indexing
  str8_index(str8 a, u64 n) -> DecodedCodepoint
  str8_length(str8 a) -> s64
@str8_comparison
  str8_compare(str8 a, str8 b) -> s64
  str8_ncompare(str8 a, str8 b, u64 n) -> s64
  str8_equal(str8 a, str8 b) -> b32
  str8_equal_lazy(str8 a, str8 b) -> b32
  str8_nequal(str8 a, str8 b) -> b32
@str8_searching
  str8_begins_with(str8 a, str8 b) -> b32
  str8_ends_with(str8 a, str8 b) -> b32
  str8_contains(str8 a, str8 b) -> b32
  str8_find_first(str8 a, u32 codepoint) -> u32
  str8_find_last(str8 a, u32 codepoint) -> u32
@str8_slicing
  str8_eat_one(str8 a) -> str8
  str8_eat_count(str8 a, u64 n) -> str8
  str8_eat_until(str8 a, u32 c) -> str8
  str8_eat_until_last(str8 a, u32 c) -> str8
  str8_eat_until_one_of(str8 a, int count, ...) -> str8
  str8_eat_until_str(str8 a, str8 c) -> str8
  str8_eat_while(str8 a, u32 c) -> str8
  str8_skip_one(str8 a) -> str8
  str8_skip_count(str8 a, u64 n) -> str8
  str8_skip_until(str8 a, u32 c) -> str8
  str8_skip_until_last(str8 a, u32 c) -> str8
  str8_skip_while(str8 a, u32 c) -> str8
@str8_building
  str8_copy(str8 a, Allocator* allocator) -> str8
  str8_concat(str8 a, str8 b, Allocator* allocator) -> str8
  str8_concat3(str8 a, str8 b, str8 c, Allocator* allocator) -> str8
  str8_from_cstr(const char* a) -> str8
  dstr8_init(dstr8* builder, str8 initial, Allocator* allocator) -> void
  dstr8_fit(dstr8* builder) -> void
  dstr8_append(dstr8* builder, str8 a) -> void
  dstr8_clear(dstr8* builder) -> void
  dstr8_peek(dstr8* builder) -> str8
  dstr8_grow(dstr8* builder, u64 bytes) -> void
  dstr8_insert_byteoffset(dstr8* builder, u64 byte_offset, str8 a) -> void
  dstr8_remove_codepoint_at_byteoffset(dstr8* builder, u64 byte_offset) -> void
@str8_hashing
  str8_static_t
  str8_static_hash64(str8_static_t a, u64 seed) constexpr -> u64
  str8_hash64(str8 a, u64 seed) -> u64
@str8_other
  str8_valid(str8 a) -> b32

TODOs:
rename str8_advance to str8_eat and str8_eat to str8_slice. make the new str8_eat return like it currently does but edits the input string
is there a point in doing non-lazy comparison?
scanning functions

References:
https://github.com/Dion-Systems/metadesk/blob/master/source/md.h
https://github.com/Dion-Systems/metadesk/blob/master/source/md.c
https://unicodebook.readthedocs.io/unicode_encodings.html
https://unicode-table.com/
*/

#pragma once
#ifndef KIGU_UNICODE_H
#define KIGU_UNICODE_H
#include "common.h"
#include "profiling.h"


#ifdef TRACY_ENABLE
#include "Tracy.hpp"
#endif

#ifndef KIGU_UNICODE_ALLOCATOR
#  define KIGU_UNICODE_ALLOCATOR stl_allocator
#endif


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @utf_decoding
#define unicode_bitmask1 0x01
#define unicode_bitmask2 0x03
#define unicode_bitmask3 0x07
#define unicode_bitmask4 0x0F
#define unicode_bitmask5 0x1F
#define unicode_bitmask6 0x3F
#define unicode_bitmask7 0x7F
#define unicode_bitmask8 0xFF
#define unicode_bitmask9  0x01FF
#define unicode_bitmask10 0x03FF

struct DecodedCodepoint{
	u32 codepoint;
	u32 advance;
};

//Returns the next codepoint and advance from the UTF-8 string `str`
global DecodedCodepoint
decoded_codepoint_from_utf8(u8* str, u64 max_advance){DPZoneScoped;
	persist u8 utf8_class[32] = { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5, };
	
	DecodedCodepoint result = {(u32)-1, 1};
	u8 byte = str[0];
	u8 byte_class = utf8_class[byte >> 3];
	switch(byte_class){
		case 1:{
			result.codepoint = byte;
		}break;
		case 2:{
			if(2 <= max_advance){
				u8 next_byte = str[1];
				if(utf8_class[next_byte >> 3] == 0){
					result.codepoint  = (     byte & unicode_bitmask5) << 6;
					result.codepoint |= (next_byte & unicode_bitmask6);
					result.advance = 2;
				}
			}
		}break;
		case 3:{
			if(3 <= max_advance){
				u8 next_byte[2] = {str[1], str[2]};
				if(   (utf8_class[next_byte[0] >> 3] == 0)
				   && (utf8_class[next_byte[1] >> 3] == 0)){
					result.codepoint  = (        byte & unicode_bitmask4) << 12;
					result.codepoint |= (next_byte[0] & unicode_bitmask6) << 6;
					result.codepoint |= (next_byte[1] & unicode_bitmask6);
					result.advance = 3;
				}
			}
		}break;
		case 4:{
			if(4 <= max_advance){
				u8 next_byte[3] = {str[1], str[2], str[3]};
				if(   (utf8_class[next_byte[0] >> 3] == 0)
				   && (utf8_class[next_byte[1] >> 3] == 0)
				   && (utf8_class[next_byte[2] >> 3] == 0)){
					result.codepoint  = (        byte & unicode_bitmask3) << 18;
					result.codepoint |= (next_byte[0] & unicode_bitmask6) << 12;
					result.codepoint |= (next_byte[1] & unicode_bitmask6) << 6;
					result.codepoint |=  next_byte[2] & unicode_bitmask6;
					result.advance = 4;
				}
			}
		}break;
	}
	return result;
}
#define utf8codepoint(x) decoded_codepoint_from_utf8(x,4).codepoint
#define utf8advance(x) decoded_codepoint_from_utf8(x,4).advance


//Returns the next codepoint and advance from the UTF-16 string `str`
global DecodedCodepoint
decoded_codepoint_from_utf16(u16* str, u64 max_advance){DPZoneScoped;
	DecodedCodepoint result = {(u32)-1, 1};
	result.codepoint = str[0];
	result.advance = 1;
	if((1 < max_advance) && (0xD800 <= str[0]) && (str[0] < 0xDC00) && (0xDC00 <= str[1]) && (str[1] < 0xE000)){
        result.codepoint = ((str[0] - 0xD800) << 10) | (str[1] - 0xDC00) + 0x10000;
        result.advance = 2;
    }
	return result;
}

//Returns the next codepoint and advance from the wchar_t string `str`
global DecodedCodepoint
decoded_codepoint_from_wchar(wchar_t* str, u64 max_advance){DPZoneScoped;
#if COMPILER_CL
	return decoded_codepoint_from_utf16((u16*)str, max_advance);
#elif COMPILER_CLANG || COMPILER_GCC
	return decoded_codepoint_from_utf16((u16*)str, max_advance);
	//return DecodedCodepoint{str[0], 1};
#else
#  error "unhandled compiler"
#endif
}

//Fills the u8 string `out` with the Unicode `codepoint`; returns the advance in number of u8, -1 if invalid codepoint
global u32
utf8_from_codepoint(u8* out, u32 codepoint){DPZoneScoped;
#define unicode_bit8 0x80
	u32 advance = 1;
	if      (codepoint <= 0x7F){
		if(out){
			out[0] = u8(codepoint);
		}
	}else if(codepoint <= 0x7FF){
		advance = 2;
		if(out){
			out[0] = (unicode_bitmask2 << 6) | ((codepoint >> 6 ) & unicode_bitmask5);
			out[1] = unicode_bit8 | (codepoint & unicode_bitmask6);
		}
	}else if(codepoint <= 0xFFFF){
		advance = 3;
		if(out){
			out[0] = (unicode_bitmask3 << 5) | ((codepoint >> 12) & unicode_bitmask4);
			out[1] = unicode_bit8 | ((codepoint >> 6) & unicode_bitmask6);
			out[2] = unicode_bit8 | ( codepoint       & unicode_bitmask6);
		}
	}else if(codepoint <= 0x10FFFF){
		advance = 4;
		if(out){
			out[0] = (unicode_bitmask4 << 4) | ((codepoint >> 18) & unicode_bitmask3);
			out[1] = unicode_bit8 | ((codepoint >> 12) & unicode_bitmask6);
			out[2] = unicode_bit8 | ((codepoint >>  6) & unicode_bitmask6);
			out[3] = unicode_bit8 | ( codepoint        & unicode_bitmask6);
		}
	}else{
		advance = -1;
	}
	return advance;
#undef unicode_bit8
}

//Fills the u16 string `out` with the Unicode `codepoint`; returns the number of u16 advance, -1 if invalid codepoint
global u32
utf16_from_codepoint(u16* out, u32 codepoint){DPZoneScoped;
	u32 advance = 1;
	if(codepoint < 0x10000){
		if(out){
			out[0] = u16(codepoint);
		}
	}else if(codepoint <= 0x10FFFF){
		advance = 2;
		if(out){
			u64 v = codepoint - 0x10000;
			out[0] = 0xD800 + (v >> 10);
			out[1] = 0xDC00 + (v & unicode_bitmask10);
		}
	}else{
		advance = -1;
	}
	return advance;
}

//Fills the wchar_t string `out` with the Unicode `codepoint`; returns the number of wchar_t advance, -1 if invalid codepoint
global u32
wchar_from_codepoint(wchar_t* out, u32 codepoint){DPZoneScoped;
#if COMPILER_CL
	return utf16_from_codepoint((u16*)out, codepoint);
#elif COMPILER_CLANG || COMPILER_GCC
	out[0] = codepoint;
	return 1;
#else
#  error "unhandled compiler"
#endif
}

#undef unicode_bitmask1
#undef unicode_bitmask2
#undef unicode_bitmask3
#undef unicode_bitmask4
#undef unicode_bitmask5
#undef unicode_bitmask6
#undef unicode_bitmask7
#undef unicode_bitmask8
#undef unicode_bitmask9
#undef unicode_bitmask10

//Returns true if the byte represents a continuation byte from UTF8
global b32
utf8_continuation_byte(u8 byte){DPZoneScoped;
	return ((byte & 0xC0) == 0x80);
}


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @utf_conversion
//Converts and returns a utf8 string from the utf16 string `in` using `allocator`
global str8
str8_from_str16(str16 in, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){DPZoneScoped;
	u64 space = 3*in.count;
	u8* str = (u8*)allocator->reserve((space+1)*sizeof(u8)); Assert(str, "failed to allocate memory");
	allocator->commit(str, (space+1)*sizeof(u8));
	u16* ptr = in.str;
	u16* opl = ptr + in.count; //one past last
	s64 size = 0;
	DecodedCodepoint consume;
	for(;ptr < opl;){
		consume = decoded_codepoint_from_utf16(ptr, opl - ptr);
		ptr += consume.advance;
		size += utf8_from_codepoint(str + size, consume.codepoint);
	}
	str[size] = '\0';
	str = (u8*)allocator->resize(str, (size+1)*sizeof(u8)); Assert(str, "failed to allocate memory");
	return str8{str, size};
}

//Converts and returns a utf8 string from the utf32 string `in` using `allocator`
global str8
str8_from_str32(str32 in, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){DPZoneScoped;
	u64 space = 4*in.count;
	u8* str = (u8*)allocator->reserve((space+1)*sizeof(u8)); Assert(str, "failed to allocate memory");
	allocator->commit(str, (space+1)*sizeof(u8));
	u32* ptr = in.str;
	u32* opl = ptr + in.count; //one past last
	s64 size = 0;
	DecodedCodepoint consume;
	for(;ptr < opl; ptr += 1){
		size += utf8_from_codepoint(str + size, *ptr);
	}
	str[size] = '\0';
	str = (u8*)allocator->resize(str, (size+1)*sizeof(u8)); Assert(str, "failed to allocate memory");
	return str8{str, size};
}

//Converts and returns a utf8 string from the wchar_t string `in` using `allocator`
//    this expects `in` to be null-terminated
global str8
str8_from_wchar(wchar_t* in, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){DPZoneScoped;
	s64 in_count = 0;
	for(wchar_t* a = in; *a != L'\0'; ++a) ++in_count;
#if COMPILER_CL
	return str8_from_str16(str16{(u16*)in, in_count}, allocator);
#elif COMPILER_CLANG || COMPILER_GCC
	return str8_from_str32(str32{(u32*)in, in_count}, allocator);
#else
#  error "unhandled compiler"
#endif
}

//Converts and returns a utf16 string from the utf8 string `in` using `allocator`
global str16
str16_from_str8(str8 in, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){DPZoneScoped;
	u64 space = 2*in.count;
	u16* str = (u16*)allocator->reserve((space+1)*sizeof(u16)); Assert(str, "failed to allocate memory");
	allocator->commit(str, (space+1)*sizeof(u16));
	u8* ptr = in.str;
	u8* opl = ptr + in.count; //one past last
	s64 size = 0;
	DecodedCodepoint consume;
	for(;ptr < opl;){
		consume = decoded_codepoint_from_utf8(ptr, opl - ptr);
		ptr += consume.advance;
		size += utf16_from_codepoint(str + size, consume.codepoint);
	}
	str[size] = '\0';
	str = (u16*)allocator->resize(str, (size+1)*sizeof(u16)); Assert(str, "failed to allocate memory");
	return str16{str, size};
}

//Converts and returns a utf32 string from the utf8 string `in` using `allocator`
global str32
str32_from_str8(str8 in, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){DPZoneScoped;
	u64 space = in.count;
	u32* str = (u32*)allocator->reserve((space+1)*sizeof(u32)); Assert(str, "failed to allocate memory");
	allocator->commit(str, (space+1)*sizeof(u32));
	u8* ptr = in.str;
	u8* opl = ptr + in.count; //one past last
	s64 size = 0;
	DecodedCodepoint consume;
	for(;ptr < opl;){
		consume = decoded_codepoint_from_utf8(ptr, opl - ptr);
		ptr += consume.advance;
		str[size] = consume.codepoint;
		size += 1;
	}
	str[size] = '\0';
	str = (u32*)allocator->resize(str, (size+1)*sizeof(u32));
	return str32{str, size};
}

//Converts and returns a wchar_t string from the utf8 string `in` using `allocator`
global wchar_t*
wchar_from_str8(str8 in, s64* out_count = 0, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){DPZoneScoped;
#if COMPILER_CL
	str16 out = str16_from_str8(in, allocator);
	if(out_count) *out_count = out.count;
	return (wchar_t*)out.str;
#elif COMPILER_CLANG || COMPILER_GCC
	str32 out = str32_from_str8(in, allocator);
	if(out_count) *out_count = out.count;
	return (wchar_t*)out.str;
#else
#  error "unhandled compiler"
#endif
}


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @str8_advancing
//Shorthand for: a->str += bytes; a->count -= bytes;
FORCE_INLINE void
str8_increment(str8* a, u64 bytes){DPZoneScoped;
	a->str += bytes;
	a->count -= bytes;
}

//Shorthand for: a->str -= bytes; a->count += bytes;
FORCE_INLINE void
str8_decrement(str8* a, u64 bytes){DPZoneScoped;
	a->str -= bytes;
	a->count += bytes;
}

//Advances the utf8 string `a` by one codepoint and returns that codepoint
global DecodedCodepoint
str8_advance(str8* a){DPZoneScoped;
	DecodedCodepoint decoded{};
	if(a && *a){
		decoded = decoded_codepoint_from_utf8(a->str, 4);
		str8_increment(a, decoded.advance);
	}
	return decoded;
}

//Advances the utf8 string `a` by `n` codepoints and returns the last codepoint (n-1 starting from 0)
global DecodedCodepoint
str8_nadvance(str8* a, u64 n){DPZoneScoped;
	DecodedCodepoint decoded{};
	if(a && n){
		while(*a && n--){
			decoded = decoded_codepoint_from_utf8(a->str, 4);
			str8_increment(a, decoded.advance);
		}
	}
	return decoded;
}

//Advances the utf8 string `a` until the codepoint `c` is encountered
global void
str8_advance_until(str8* a, u32 c){DPZoneScoped;
	if(a){
		while(*a){
			DecodedCodepoint decoded = decoded_codepoint_from_utf8(a->str, 4);
			if(decoded.codepoint == c) break;
			str8_increment(a, decoded.advance);
		}
	}
}

//Advances the utf8 string `a` while the codepoint `c` is encountered
global void
str8_advance_while(str8* a, u32 c){DPZoneScoped;
	if(a){
		while(*a){
			DecodedCodepoint decoded = decoded_codepoint_from_utf8(a->str, 4);
			if(decoded.codepoint != c) break;
			str8_increment(a, decoded.advance);
		}
	}
}


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @str8_indexing
//Returns the `n`th codepoint (starting from 0) in the utf8 string `a`
//    returns the last codepoint of the string if `n` is greater than the length of the string
global inline DecodedCodepoint
str8_index(str8 a, u64 n){DPZoneScoped;
	return str8_nadvance(&a, n+1);
}

//Returns the number of codepoints in the unicode string `a`
global inline s64
str8_length(str8 a){DPZoneScoped;
	s64 result = 0;
	while(str8_advance(&a).codepoint) result++;
	return result;
}


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @str8_comparison
//Compares the utf8 strings `a` and `b` a codepoint at a time until the end of one of the strings
//    returns 0 if the strings are equal
//    returns <0 if the first character that doesn't match has a lower value codepoint in `a` than in `b`
//    returns >0 if the first character that doesn't match has a lower value codepoint in `b` than in `a`
global s64
str8_compare(str8 a, str8 b){DPZoneScoped;
	if(a.str == b.str && a.count == b.count) return 0;
	s64 diff = 0;
	while(diff == 0 && (a || b)) diff = (s64)str8_advance(&a).codepoint - (s64)str8_advance(&b).codepoint;
	return diff;
}

//Compares the utf8 strings `a` and `b` a codepoint at a time up to `n` codepoints
//    returns 0 if the strings are equal
//    returns <0 if the first character that doesn't match has a lower value codepoint in `a` than in `b`
//    returns >0 if the first character that doesn't match has a lower value codepoint in `b` than in `a`
global s64
str8_ncompare(str8 a, str8 b, u64 n){DPZoneScoped;
	if(a.str == b.str && a.count == b.count) return 0;
	s64 diff = 0;
	while(diff == 0 && n-- && (a || b)) diff = (s64)str8_advance(&a).codepoint - (s64)str8_advance(&b).codepoint;
	return diff;
}

//Returns true if the utf8 strings `a` and `b` are equal for all codepoints of the strings
global inline b32
str8_equal(str8 a, str8 b){DPZoneScoped;
	return a.count == b.count && str8_compare(a, b) == 0;
}

//Returns true if the utf8 strings `a` and `b` are equal for all codepoints of the strings by simply comparing memory
global inline b32
str8_equal_lazy(str8 a, str8 b){DPZoneScoped;
	return a.count == b.count && memcmp(a.str, b.str, a.count) == 0;
}

//Returns true if the utf8 strings `a` and `b` are equal for `n` codepoints
global inline b32
str8_nequal(str8 a, str8 b, u64 n){DPZoneScoped;
	return str8_ncompare(a, b, n) == 0;
}


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @str8_searching
//Returns true if utf8 string `a` begins with utf8 string `b`
global inline b32
str8_begins_with(str8 a, str8 b){DPZoneScoped;
	if(a.str == b.str && a.count == b.count) return true;
	return a.count >= b.count && str8_ncompare(a, b, str8_length(b)) == 0;
}

//Returns true if utf8 string `a` ends with utf8 string `b`
global inline b32
str8_ends_with(str8 a, str8 b){DPZoneScoped;
	if(a.str == b.str && a.count == b.count) return true;
	return a.count >= b.count && str8_compare(str8{a.str+a.count-b.count,b.count}, b) == 0;
}

//Returns true if utf8 string `a` contains utf8 string `b`
global b32
str8_contains(str8 a, str8 b){DPZoneScoped;
	if(a.str == b.str && a.count == b.count) return true;
	u32 b_len = str8_length(b);
	while(a){
		if(b.count > a.count) return false;
		if(str8_nequal(a, b, b_len)) return true;
		str8_advance(&a);
	}
	return false;
}

//returns the first occurance of 'codepoint' in a string 'a' as a byte offset
//returns npos if it is not found
global u32
str8_find_first(str8 a, u32 codepoint){DPZoneScoped;
	u32 iter = 0;
	while(a.count){
		DecodedCodepoint d = str8_advance(&a);
		if(d.codepoint==codepoint) return iter;
		iter++;
	}
	return npos;
}

//TODO(sushi) rename and put somewhere better later
global u64 
utf8_move_back(u8* start){DPZoneScoped;
	u64 count = 0;
	while(utf8_continuation_byte(*(start-1))){
		start--; count++;
	}
	return count;
}

//returns the last occurance of 'codepoint' in a string 'a' as a byte offset
//returns npos if it is not found
global u32
str8_find_last(str8 a, u32 codepoint) {DPZoneScoped;
	u32 iter = 0;
	while(iter < a.count){
		iter += utf8_move_back(a.str+a.count-iter);
		iter++;
		if(decoded_codepoint_from_utf8(a.str+a.count-iter,4).codepoint == codepoint) return a.count-iter;
	}
	return npos;
}


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @str8_slicing
//Returns a slice of the utf8 string `a` including one codepoint from the beginning
global inline str8
str8_eat_one(str8 a){DPZoneScoped;
	str8 b = a;
	str8_advance(&b);
	if(!b) return a;
	return str8{a.str, a.count-b.count};
}

//Returns a slice of the utf8 string `a` including `n` codepoints from the beginning
global inline str8
str8_eat_count(str8 a, u64 n){DPZoneScoped;
	str8 b = a;
	str8_nadvance(&b, n);
	if(!b) return a;
	return str8{a.str, a.count-b.count};
}

//Returns a slice of the utf8 string `a` ending before the first occurance of the codepoint `c`
//    if codepoint `c` is not encountered, `a` is returned
global str8
str8_eat_until(str8 a, u32 c){DPZoneScoped;
	str8 b = a;
	while(b){
		DecodedCodepoint decoded = decoded_codepoint_from_utf8(b.str, 4);
		if(decoded.codepoint == c) break;
		str8_increment(&b, decoded.advance);
	}
	if(!b) return a;
	return str8{a.str, a.count-b.count};
}

//Returns a slice of the utf8 string `a` ending before the last occurance of the codepoint `c`
//    if codepoint `c` is not encountered, `a` is returned
global str8
str8_eat_until_last(str8 a, u32 c){DPZoneScoped;
	str8 b = a;
	s64 count = 0;
	while(b){
		DecodedCodepoint decoded = decoded_codepoint_from_utf8(b.str, 4);
		if(decoded.codepoint == c) count = b.count;
		str8_increment(&b, decoded.advance);
	}
	return str8{a.str, a.count-count};
}

//Returns a slice of the utf8 string `a` ending before the first occurance of the one of the `count` codepoints passed
//    `count` should equal the number of arguments you pass to the variadic (...)
//    if no (...) codepoints are not encountered, `a` is returned
global str8
str8_eat_until_one_of(str8 a, int count, ...){DPZoneScoped;
	str8 b = a;
	while(b){
		DecodedCodepoint decoded = decoded_codepoint_from_utf8(b.str, 4);
		va_list args; va_start(args, count);
		forI(count){
			if(decoded.codepoint == va_arg(args, u32)){
				goto found_one;
			}
		}
		va_end(args);
		str8_increment(&b, decoded.advance);
	}
	found_one:;
	if(!b) return a;
	return str8{a.str, a.count-b.count};
}

//Returns a slice of the utf8 string `a` ending before the first occurance of the string `c`
//    if string `c` is not encountered, `a` is returned
global str8
str8_eat_until_str(str8 a, str8 c){DPZoneScoped;
	str8 b = a;
	while(b){
		if(str8_begins_with(b, c)) break;
		str8_advance(&b);
	}
	if(!b) return a;
	return str8{a.str, a.count-b.count};
}

//Returns a slice of the utf8 string `a` ending before the first occurance of a codepoint that is not the codepoint `c`
//    if codepoint `c` is not encountered, an empty string is returned
global str8
str8_eat_while(str8 a, u32 c){DPZoneScoped;
	str8 b = a;
	while(b){
		DecodedCodepoint decoded = decoded_codepoint_from_utf8(b.str, 4);
		if(decoded.codepoint != c) break;
		str8_increment(&b, decoded.advance);
	}
	if(!b) return str8{};
	return str8{a.str, a.count-b.count};
}

//TODO(sushi) move this somewhere more appropriate
global b32 
is_whitespace(u32 codepoint){
	switch(codepoint){
		case '\t': case '\n': case '\v': case '\f':  case '\r':
		case ' ': case 133: case 160: case 5760: case 8192:
		case 8193: case 8194: case 8195: case 8196: case 8197:
		case 8198: case 8199: case 8200: case 8201: case 8202:
		case 8232: case 8239: case 8287: case 12288: return true;
		default: return false;
	}
}

global b32 
isalnum(u32 codepoint){
	if(codepoint >= 'A' && codepoint <= 'Z' ||
	   codepoint >= 'a' && codepoint <= 'z' ||
	   codepoint >= '0' && codepoint <= '9') return true;
	return false;
}

global b32 
isdigit(u32 codepoint){
	if(codepoint >= '0' && codepoint <= '9') return true;
	return false;
}

//Returns a slice of `a` ending before the first occurance of a non-whitespace character.
//this includes all characters marked by unicode as whitespace (see is_whitespace())
global str8
str8_eat_whitespace(str8 a){DPZoneScoped;
	str8 out = {a.str,0};
	while(a){
		DecodedCodepoint dc = str8_advance(&a); 
		if(!is_whitespace(dc.codepoint)) break;
		out.count += dc.advance;
	}
	return out;
}

//Returns a slice of `a` ending before the first occurance of a non-word character
//words consist of all alphanumeric ascii characters
global str8
str8_eat_word(str8 a, b32 include_underscore = 0){
	str8 out = {a.str,0};
	while(a){
		DecodedCodepoint dc = str8_advance(&a);
		if(!isalnum(dc.codepoint)) break;
		if(dc.codepoint == '_' && !include_underscore) break;
		out.count += dc.advance;
	}
	return out;
}

//Returns a slice of `a` ending before the first occurance of a non-integer character
global str8
str8_eat_int(str8 a){
	str8 out = {a.str,0};
	while(a){
		DecodedCodepoint dc = str8_advance(&a);
		if(!isdigit(dc.codepoint)) break;
		out.count += dc.advance;
	}
	return out;
}

//Returns a slice of the utf8 string `a` starting after the first character until the end of the string
global inline str8
str8_skip_one(str8 a){DPZoneScoped;
	str8_advance(&a);
	return a;
}

//Returns a slice of the utf8 string `a` starting after the `n`th character from the beginning until the end of the string
global inline str8
str8_skip_count(str8 a, u64 n){DPZoneScoped;
	str8_nadvance(&a, n);
	return a;
}

//Returns a slice of the utf8 string `a` starting at the first occurance of the codepoint `c` until the end of the string
global str8
str8_skip_until(str8 a, u32 c){DPZoneScoped;
	while(a){
		DecodedCodepoint decoded = decoded_codepoint_from_utf8(a.str, 4);
		if(decoded.codepoint == c) break;
		str8_increment(&a, decoded.advance);
	}
	return a;
}

//Returns a slice of the utf8 string `a` starting at the last occurance of the codepoint `c` until the end of the string
global str8
str8_skip_until_last(str8 a, u32 c){DPZoneScoped;
	str8 b{};
	while(a){
		DecodedCodepoint decoded = decoded_codepoint_from_utf8(a.str, 4);
		if(decoded.codepoint == c) b = a;
		str8_increment(&a, decoded.advance);
	}
	return b;
}

global str8
str8_skip_until_one_of(str8 a, int count, ...) {DPZoneScoped;
	str8 b = a;
	while(b) {
		DecodedCodepoint decoded = decoded_codepoint_from_utf8(b.str, 4);
		va_list args; va_start(args, count);
		forI(count){
			if(decoded.codepoint == va_arg(args, u32)) {
				goto found_one;
			}
		}
		va_end(args);
		str8_increment(&b, decoded.advance);
	}
	found_one:;
	return b;
}

//Returns a slice of the utf8 string `a` starting at the first occurance of a codepoint that is not the codepoint `c` until the end of the string
global str8
str8_skip_while(str8 a, u32 c){DPZoneScoped;
	while(a){
		DecodedCodepoint decoded = decoded_codepoint_from_utf8(a.str, 4);
		if(decoded.codepoint != c) break;
		str8_increment(&a, decoded.advance);
	}
	return a;
}

//Returns a slice of `a` starting at the first occurance of a non-whitespace character.
//this includes all characters marked by unicode as whitespace (see is_whitespace())
global str8
str8_skip_whitespace(str8 a){DPZoneScoped;
	while(a){
		DecodedCodepoint dc = decoded_codepoint_from_utf8(a.str,4); 
		if(!is_whitespace(dc.codepoint)) break;
		str8_increment(&a,dc.advance);
	}
	return a;
}

//Returns a slice of `a` starting at the first occurance of a non-word character
//words consist of all alphanumeric ascii characters
global str8
str8_skip_word(str8 a, b32 include_underscore = 0){
	while(a){
		DecodedCodepoint dc = decoded_codepoint_from_utf8(a.str,4);
		if(!isalnum(dc.codepoint)) break;
		if(dc.codepoint == '_' && !include_underscore) break;
		str8_increment(&a,dc.advance);
	}
	return a;
}

//Returns a slice of `a` starting at the first occurance of a non-integer character
global str8
str8_skip_int(str8 a){
	while(a){
		DecodedCodepoint dc = decoded_codepoint_from_utf8(a.str,4);
		if(!isdigit(dc.codepoint)) break;
		str8_increment(&a,dc.advance);
	}
	return a;
}


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @str8_building
#ifndef KIGU_STR8BUILDER_BYTE_ALIGNMENT
#  define KIGU_STR8BUILDER_BYTE_ALIGNMENT 8
#endif

//Allocates and returns a new copy of the utf8 string `a` using `allocator`
global str8
str8_copy(str8 a, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){DPZoneScoped;
	str8 result{(u8*)allocator->reserve((a.count+1)*sizeof(u8)), a.count};
	Assert(result.str, "failed to allocate memory");
	CopyMemory(result.str, a.str, a.count*sizeof(u8));
	return result;
}

//Allocates and returns a utf8 string of `a` with `b` appended to it using `allocator`
global str8
str8_concat(str8 a, str8 b, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){DPZoneScoped;
	str8 result{(u8*)allocator->reserve((a.count+b.count+1)*sizeof(u8)), a.count+b.count};
	Assert(result.str, "failed to allocate memory");
	CopyMemory(result.str,         a.str, a.count*sizeof(u8));
	CopyMemory(result.str+a.count, b.str, b.count*sizeof(u8));
	return result;
}

//Allocates and returns a utf8 string of `a` with `b` and then `c` appended to it using `allocator`
global str8
str8_concat3(str8 a, str8 b, str8 c, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){DPZoneScoped;
	str8 result{(u8*)allocator->reserve((a.count+b.count+c.count+1)*sizeof(u8)), a.count+b.count+c.count};
	Assert(result.str, "failed to allocate memory");
	CopyMemory(result.str,                 a.str, a.count*sizeof(u8));
	CopyMemory(result.str+a.count,         b.str, b.count*sizeof(u8));
	CopyMemory(result.str+a.count+b.count, c.str, c.count*sizeof(u8));
	return result;
}

//Allocates and returns a utf8 string of the null-terminated c string `a` using `allocator`
FORCE_INLINE str8
str8_from_cstr(const char* a){DPZoneScoped;
	return str8{(u8*)a, (s64)strlen(a)};
}

//Initializes `builder` with the string `initial` using `allocator`
global void
dstr8_init(dstr8* builder, str8 initial, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){DPZoneScoped;
	builder->count     = initial.count;
	builder->space     = RoundUpTo(builder->count+1, KIGU_STR8BUILDER_BYTE_ALIGNMENT);
	builder->str       = (u8*)allocator->reserve(builder->space*sizeof(u8)); Assert(builder->str, "Failed to allocate memory");
	builder->allocator = allocator;
	if(initial.str) CopyMemory(builder->str, initial.str, initial.count*sizeof(u8));
}

// Deinitializes 'builder'
global void
dstr8_deinit(dstr8* builder){
	builder->allocator->release(builder->str);
	*builder = {0};
}

//Fits the allocation of `builder` to its `count` (+1 for null-terminator)
global void
dstr8_fit(dstr8* builder){DPZoneScoped;
	builder->space = builder->count+1;
	builder->str   = (u8*)builder->allocator->resize(builder->str, builder->space*sizeof(u8)); Assert(builder->str, "Failed to allocate memory");
}

//Appends `a` to the end of `builder`
global void
dstr8_append(dstr8* builder, str8 a){DPZoneScoped;
	s64 offset = builder->count;
	builder->count += a.count;
	if(builder->space < builder->count+1){
		builder->space = RoundUpTo(builder->count+1, KIGU_STR8BUILDER_BYTE_ALIGNMENT);
		builder->str   = (u8*)builder->allocator->resize(builder->str, builder->space*sizeof(u8)); Assert(builder->str, "Failed to allocate memory");
	}
	CopyMemory(builder->str+offset, a.str, a.count*sizeof(u8));
}

//Zeros the allocation of `builder` and sets `count` to `0`, but does not affect `space`
global void
dstr8_clear(dstr8* builder){DPZoneScoped;
	ZeroMemory(builder->str, builder->count);
	builder->count = 0;
}

//Returns a str8 of the internal string of `builder`
FORCE_INLINE str8
dstr8_peek(dstr8* builder){DPZoneScoped;
	return str8{builder->str, builder->count};
}

//Grows the buffer of `builder` by at least `bytes` (`space` will be aligned to `KIGU_STR8BUILDER_BYTE_ALIGNMENT`)
global void
dstr8_grow(dstr8* builder, u64 bytes){DPZoneScoped;
	if(bytes){
		builder->space = RoundUpTo(builder->space+bytes, KIGU_STR8BUILDER_BYTE_ALIGNMENT);
		builder->str   = (u8*)builder->allocator->resize(builder->str, builder->space*sizeof(u8)); Assert(builder->str, "Failed to allocate memory");
	}
}

//Inserts the utf8 string `a` at `byte_offset` into the buffer of `builder`
//    does no error checking to see if `byte_offset` is in the middle of a multi-byte codepoint
global void
dstr8_insert_byteoffset(dstr8* builder, u64 byte_offset, str8 a){DPZoneScoped;
	if(a && byte_offset <= builder->count){
		s64 required_space = builder->count + a.count + 1;
		if(required_space > builder->space) dstr8_grow(builder, required_space - builder->space);
		MoveMemory(builder->str + byte_offset + a.count, builder->str + byte_offset, ((builder->count - byte_offset) + 1)*sizeof(u8));
		CopyMemory(builder->str + byte_offset,           a.str,                      a.count*sizeof(u8));
		builder->count += a.count;
	}
}

//Removes one codepoint starting at `byte_offset` into the buffer of `builder`
//    does nothing if `byte_offset` is greater than `dstr8.count` or `byte_offset` is a UTF8 continuation byte
global u64
dstr8_remove_codepoint_at_byteoffset(dstr8* builder, u64 byte_offset){DPZoneScoped;
	if((byte_offset < builder->count) && !utf8_continuation_byte(*(builder->str+byte_offset))){
		DecodedCodepoint decoded = decoded_codepoint_from_utf8(builder->str + byte_offset, 4);
		CopyMemory(builder->str + byte_offset, builder->str + byte_offset + decoded.advance, builder->count - byte_offset);
		builder->count -= decoded.advance;
		return decoded.advance;
	}
	return 0;
}

// template<class... T> global str8
// to_dstr8v(Allocator* allocator, T... args){DPZoneScoped;
// 	str8b str; dstr8_init(&str, {0}, allocator);
// 	constexpr auto arg_count{sizeof...(T)};
// 	str8 arr[arg_count] = {to_dstr8(args, allocator)...};
// 	forI(arg_count) dstr8_append(&str, arr[i]);
// 	return str.fin;
// }

template<class... T> global void
dstr8_append(dstr8* builder, T... args){DPZoneScoped;
	constexpr auto arg_count{sizeof...(T)};
	dstr8 arr[arg_count] = {to_dstr8(args, builder->allocator)...};
	forI(arg_count){
		dstr8_append(builder, arr[i].fin);
		dstr8_deinit(&arr[i]);
	}
}

// replaces all instances of the codepoint 'find' with a different codepoint
global
void dstr8_replace_codepoint(dstr8* builder, u32 find, u32 replace){
	str8 parse = builder->fin;
	u32 offset = 0;
	while(parse){
		auto [codepoint, size] = str8_advance(&parse);
		if(codepoint == find)
			builder->str[offset] = replace;
		offset += size;
	}
}

//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @str8_hashing
struct str8_static_t{const char* str; u64 count; template<u64 N> constexpr str8_static_t(const char(&a)[N]): str(a), count(N-1){}};

//Returns a 32bit FNV-1a hash of the string `a` seeded with `seed` at compile-time
constexpr u64
str8_static_hash32(str8_static_t a, u64 seed = 2166136261){ //32bit FNV_offset_basis
	while(a.count-- != 0){
		seed ^= (u8)*a.str;
		seed *= 16777619; //32bit FNV_prime
		a.str++;
	}
	return seed;
}

//Returns a 32bit FNV-1a hash of the string `a` seeded with `seed`
global u64
str8_hash32(str8 a, u64 seed = 2166136261){DPZoneScoped; //32bit FNV_offset_basis
	while(a.count-- != 0){
		seed ^= *a.str++;
		seed *= 16777619; //32bit FNV_prime
	}
	return seed;
}

//Returns a 64bit FNV-1a hash of the string `a` seeded with `seed` at compile-time
constexpr u64
str8_static_hash64(str8_static_t a, u64 seed = 14695981039346656037){ //64bit FNV_offset_basis
	while(a.count-- != 0){
		seed ^= (u8)*a.str;
		seed *= 1099511628211; //64bit FNV_prime
		a.str++;
	}
	return seed;
}

//Returns a 64bit FNV-1a hash of the string `a` seeded with `seed`
global u64
str8_hash64(str8 a, u64 seed = 14695981039346656037){DPZoneScoped; //64bit FNV_offset_basis
	while(a.count-- != 0){
		seed ^= *a.str++;
		seed *= 1099511628211; //64bit FNV_prime
	}
	return seed;
}

//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @str8_other
//Returns true if the string `a` has a non-null pointer `str8.str` and a positive `str8.count`
//    equivalent to operator bool() call, but intended for C code
global b32
str8_valid(str8 a){DPZoneScoped;
	return (a.str) && (a.count > 0) && (*a.str != '\0');
}

#endif //KIGU_UNICODE_H
