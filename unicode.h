/* Kigu Unicode Library
Notes:
- str8 is the expected usage type thruout the projects, and as such, has string functions for it.
- strings are null-terminated (ending in '\0') in order for libc functions to work, but we don't use it
internally and don't assume they are included in the count (NOTE this assumes the allocators fill memory to zero).
- str16 and str32 use big endian UTF encodings.

Terminology:
codepoint       the value or set of values that represent one unicode character
advance         the number of values of the underlying type (u8 for UTF8, u16 for UTF16, etc) a codepoint takes up to represent
fixed-width     the number of values equals the number of characters and the advance is always one (ASCII, UCS2, UTF32)
multibyte       the number of values depends on the character's codepoint and the advance can vary (UTF8, UTF16)
surrogate pair  in UTF16, codepoints above U+010000 require two 16bit code units called surrogate pairs

Index:
@utf_types
@utf_decoding
@utf_conversion
@str8_advancing
@str8_indexing
@str8_comparison
@str8_searching
@str8_slicing
@str8_building
@str8_hashing

!ref: https://github.com/Dion-Systems/metadesk/blob/master/source/md.h
!ref: https://github.com/Dion-Systems/metadesk/blob/master/source/md.c
!ref: https://unicodebook.readthedocs.io/unicode_encodings.html
!ref: https://unicode-table.com/
*/

#pragma once
#ifndef KIGU_UNICODE_H
#define KIGU_UNICODE_H
#include "common.h"

#ifndef KIGU_UNICODE_ALLOCATOR
#  define KIGU_UNICODE_ALLOCATOR stl_allocator
#endif

//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @utf_types
struct str8{
	u8* str;
	s64 count;
	FORCE_INLINE explicit operator bool(){ return str && count > 0; }
#define str8_lit(s) str8{(u8*)GLUE(u8,s), sizeof(GLUE(u8,s))-1}
};

struct str8_builder{
	u8* str;
	s64 count;
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

//returns the next codepoint and advance from the UTF-8 string `str`
global_ DecodedCodepoint
decoded_codepoint_from_utf8(u8* str, u64 max_advance){
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

//returns the next codepoint and advance from the UTF-16 string `str`
global_ DecodedCodepoint
decoded_codepoint_from_utf16(u16* str, u64 max_advance){
	DecodedCodepoint result = {(u32)-1, 1};
	result.codepoint = str[0];
	result.advance = 1;
	if((1 < max_advance) && (0xD800 <= str[0]) && (str[0] < 0xDC00) && (0xDC00 <= str[1]) && (str[1] < 0xE000)){
        result.codepoint = ((str[0] - 0xD800) << 10) | (str[1] - 0xDC00) + 0x10000;
        result.advance = 2;
    }
	return result;
}

//returns the next codepoint and advance from the wchar_t string `str`
global_ DecodedCodepoint
decoded_codepoint_from_wchar(wchar_t* str, u64 max_advance){
#if COMPILER_CL
	return decoded_codepoint_from_utf16((u16*)str, max_advance);
#elif COMPILER_CLANG || COMPILER_GCC
	return DecodedCodepoint{str[0], 1};
#else
#  error "unhandled compiler"
#endif
}

//fills the u8 string `out` with the Unicode `codepoint`; returns the advance in number of u8, -1 if invalid codepoint
global_ u32
utf8_from_codepoint(u8* out, u32 codepoint){
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

//fills the u16 string `out` with the Unicode `codepoint`; returns the number of u16 advance, -1 if invalid codepoint
global_ u32
utf16_from_codepoint(u16* out, u32 codepoint){
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

//fills the wchar_t string `out` with the Unicode `codepoint`; returns the number of wchar_t advance, -1 if invalid codepoint
global_ u32
wchar_from_codepoint(wchar_t* out, u32 codepoint){
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


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @utf_conversion
global_ str8
str8_from_str16(str16 in, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){
	u64 space = 3*in.count;
	u8* str = (u8*)allocator->reserve((space+1)*sizeof(u8));
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
	str = (u8*)allocator->resize(str, (size+1)*sizeof(u8));
	return str8{str, size};
}

global_ str8
str8_from_str32(str32 in, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){
	u64 space = 4*in.count;
	u8* str = (u8*)allocator->reserve((space+1)*sizeof(u8));
	allocator->commit(str, (space+1)*sizeof(u8));
	u32* ptr = in.str;
	u32* opl = ptr + in.count; //one past last
	s64 size = 0;
	DecodedCodepoint consume;
	for(;ptr < opl; ptr += 1){
		size += utf8_from_codepoint(str + size, *ptr);
	}
	str[size] = '\0';
	str = (u8*)allocator->resize(str, (size+1)*sizeof(u8));
	return str8{str, size};
}

global_ str8
str8_from_wchar(wchar_t* in, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){
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

global_ str16
str16_from_str8(str8 in, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){
	u64 space = 2*in.count;
	u16* str = (u16*)allocator->reserve((space+1)*sizeof(u16));
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
	str = (u16*)allocator->resize(str, (size+1)*sizeof(u16));
	return str16{str, size};
}

global_ str32
str32_from_str8(str8 in, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){
	u64 space = in.count;
	u32* str = (u32*)allocator->reserve((space+1)*sizeof(u32));
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

global_ wchar_t*
wchar_from_str8(str8 in, s64* out_count = 0, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){
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
//advances the utf8 string `a` by one codepoint and returns that codepoint
global_ DecodedCodepoint
str8_advance(str8* a){
	DecodedCodepoint decoded{};
	if(a && *a){
		decoded = decoded_codepoint_from_utf8(a->str, 4);
		a->str   += decoded.advance;
		a->count -= decoded.advance;
	}
	return decoded;
}

//advances the utf8 string `a` by `n` codepoints and returns the last codepoint (n-1 starting from 0)
global_ DecodedCodepoint
str8_nadvance(str8* a, u64 n){
	DecodedCodepoint decoded{};
	if(a && n){
		while(*a && n--){
			decoded = decoded_codepoint_from_utf8(a->str, 4);
			a->str   += decoded.advance;
			a->count -= decoded.advance;
		}
	}
	return decoded;
}

//advances the utf8 string `a` until the codepoint `c` is encountered and returns the last codepoint
global_ void
str8_advance_until(str8* a, u32 c){
	if(a){
		while(*a){
			DecodedCodepoint decoded = decoded_codepoint_from_utf8(a->str, 4);
			if(decoded.codepoint == c) break;
			a->str   += decoded.advance;
			a->count -= decoded.advance;
		}
	}
}

//advances the utf8 string `a` while the codepoint `c` is encountered and returns the last codepoint
global_ void
str8_advance_while(str8* a, u32 c){
	if(a){
		while(*a){
			DecodedCodepoint decoded = decoded_codepoint_from_utf8(a->str, 4);
			if(decoded.codepoint != c) break;
			a->str   += decoded.advance;
			a->count -= decoded.advance;
		}
	}
}


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @str8_indexing
//returns the `n`th codepoint (starting from 0) in the utf8 string `a`
//returns the last codepoint of the string if `n` is greater than the length of the string
global_ inline DecodedCodepoint
str8_index(str8 a, u64 n){
	return str8_nadvance(&a, n+1);
}

//returns the number of codepoints in the unicode string `a`
global_ inline s64
str8_length(str8 a){
	s64 result = 0;
	while(str8_advance(&a).codepoint) result++;
	return result;
}


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @str8_comparison
//compares the utf8 strings `a` and `b` a codepoint at a time until the end of one of the strings
//returns 0 if the strings are equal
//returns <0 if the first character that doesn't match has a lower value codepoint in `a` than in `b`
//returns >0 if the first character that doesn't match has a lower value codepoint in `b` than in `a`
global_ s64
str8_compare(str8 a, str8 b){
	if(a.str == b.str && a.count == b.count) return 0;
	s64 diff = 0;
	while(diff == 0 && (a || b)) diff = (s64)str8_advance(&a).codepoint - (s64)str8_advance(&b).codepoint;
	return diff;
}

//compares the utf8 strings `a` and `b` a codepoint at a time up to `n` codepoints
//returns 0 if the strings are equal
//returns <0 if the first character that doesn't match has a lower value codepoint in `a` than in `b`
//returns >0 if the first character that doesn't match has a lower value codepoint in `b` than in `a`
global_ s64
str8_ncompare(str8 a, str8 b, u64 n){
	if(a.str == b.str && a.count == b.count) return 0;
	s64 diff = 0;
	while(diff == 0 && n-- && (a || b)) diff = (s64)str8_advance(&a).codepoint - (s64)str8_advance(&b).codepoint;
	return diff;
}

//returns true if the utf8 strings `a` and `b` are equal for all codepoints of the strings
global_ inline b32
str8_equal(str8 a, str8 b){
	return a.count == b.count && str8_compare(a, b) == 0;
}

//returns true if the utf8 strings `a` and `b` are equal for `n` codepoints
global_ inline b32
str8_nequal(str8 a, str8 b, u64 n){
	return str8_ncompare(a, b, n) == 0;
}


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @str8_searching
//returns true if utf8 string `a` begins with utf8 string `b`
global_ inline b32
str8_begins_with(str8 a, str8 b){
	if(a.str == b.str && a.count == b.count) return true;
	return a.count >= b.count && str8_ncompare(a, b, str8_length(b)) == 0;
}

//returns true if utf8 string `a` ends with utf8 string `b`
global_ inline b32
str8_ends_with(str8 a, str8 b){
	if(a.str == b.str && a.count == b.count) return true;
	return a.count >= b.count && str8_compare(str8{a.str+a.count-b.count,b.count}, b) == 0;
}

//returns true if utf8 string `a` contains utf8 string `b`
global_ b32
str8_contains(str8 a, str8 b){
	if(a.str == b.str && a.count == b.count) return true;
	u32 b_len = str8_length(b);
	while(a){
		if(b.count > a.count) return false;
		if(str8_nequal(a, b, b_len)) return true;
		str8_advance(&a);
	}
	return false;
}


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @str8_slicing
//returns a slice of the utf8 string `a` ending at the first codepoint
global_ inline str8
str8_eat_one(str8 a){
	str8 b = a;
	str8_advance(&b);
	if(!b) return str8{};
	return str8{a.str, a.count-b.count};
}

//returns a slice of the utf8 string `a` ending at the `n`th codepoint
global_ inline str8
str8_eat_count(str8 a, u64 n){
	str8 b = a;
	str8_nadvance(&b, n);
	if(!b) return str8{};
	return str8{a.str, a.count-b.count};
}

//returns a slice of the utf8 string `a` ending at the first occurance of the codepoint `c`
global_ str8
str8_eat_until(str8 a, u32 c){
	str8 b = a;
	while(b){
		DecodedCodepoint decoded = decoded_codepoint_from_utf8(b.str, 4);
		if(decoded.codepoint == c) break;
		b.str   += decoded.advance;
		b.count -= decoded.advance;
	}
	if(!b) return str8{};
	return str8{a.str, a.count-b.count};
}

//returns a slice of the utf8 string `a` ending at the first occurance of a codepoint that is not the codepoint `c`
global_ str8
str8_eat_while(str8 a, u32 c){
	str8 b = a;
	while(b){
		DecodedCodepoint decoded = decoded_codepoint_from_utf8(b.str, 4);
		if(decoded.codepoint != c) break;
		b.str   += decoded.advance;
		b.count -= decoded.advance;
	}
	if(!b) return str8{};
	return str8{a.str, a.count-b.count};
}

//returns a slice of the utf8 string `a` starting after the first character until the end of the string
global_ inline str8
str8_skip_one(str8 a){
	str8_advance(&a);
	return a;
}

//returns a slice of the utf8 string `a` starting after the `n`th character from the beginning until the end of the string
global_ inline str8
str8_skip_count(str8 a, u64 n){
	str8_nadvance(&a, n);
	return a;
}

//returns a slice of the utf8 string `a` starting at the first occurance of the codepoint `c` until the end of the string
global_ str8
str8_skip_until(str8 a, u32 c){
	while(a){
		DecodedCodepoint decoded = decoded_codepoint_from_utf8(a.str, 4);
		if(decoded.codepoint == c) break;
		a.str   += decoded.advance;
		a.count -= decoded.advance;
	}
	return a;
}

//returns a slice of the utf8 string `a` starting at the first occurance of a codepoint that is not the codepoint `c` until the end of the string
global_ str8
str8_skip_while(str8 a, u32 c){
	while(a){
		DecodedCodepoint decoded = decoded_codepoint_from_utf8(a.str, 4);
		if(decoded.codepoint != c) break;
		a.str   += decoded.advance;
		a.count -= decoded.advance;
	}
	return a;
}


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @str8_building
//allocates a new copy of the utf8 string `a` using `allocator`
global_ str8
str8_copy(str8 a, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){
	str8 result{(u8*)allocator->reserve((a.count+1)*sizeof(u8)), a.count};
	CopyMemory(result.str, a.str, a.count*sizeof(u8));
	return result;
}

//initializes a str8_builder `builder` with the string `initial` using `allocator`
global_ void
str8_builder_init(str8_builder* builder, str8 initial, Allocator* allocator = KIGU_UNICODE_ALLOCATOR){
	builder->count     = initial.count;
	builder->space     = RoundUpTo(builder->count+1, 8);
	builder->str       = (u8*)allocator->reserve(builder->space*sizeof(u8)); Assert(builder->str, "Failed to allocate memory");
	builder->allocator = allocator;
	if(initial.str) CopyMemory(builder->str, initial.str, initial.count*sizeof(u8));
}

//fits the allocation of a str8_builder `builder` to its count (+1 for null-terminator)
global_ void
str8_builder_fit(str8_builder* builder){
	builder->space = builder->count+1;
	builder->str   = (u8*)builder->allocator->resize(builder->str, builder->space*sizeof(u8)); Assert(builder->str, "Failed to allocate memory");
}

//appends `a` to end the str8_builder `builder`
global_ void
str8_builder_append(str8_builder* builder, str8 a){
	if(!a) return;
	
	s64 offset = builder->count;
	builder->count += a.count;
	if(builder->space < builder->count+1){
		builder->space = RoundUpTo(builder->count+1, 8);
		builder->str   = (u8*)builder->allocator->resize(builder->str, builder->space*sizeof(u8)); Assert(builder->str, "Failed to allocate memory");
	}
	CopyMemory(builder->str+offset, a.str, a.count*sizeof(u8));
}


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @str8_hashing
struct str8_static_t{const char* str; u64 count; template<u64 N> constexpr str8_static_t(const char(&a)[N]): str(a), count(N-1){}};
constexpr u64
str8_static_hash64(str8_static_t s, u64 seed = 14695981039346656037){ //64bit FNV_offset_basis
	while(s.count-- != 0){
		seed ^= (u8)*s.str;
		seed *= 1099511628211; //64bit FNV_prime
		s.str++;
	}
	return seed;
}

global_ u64
str8_hash64(str8 s, u64 seed = 14695981039346656037){ //64bit FNV_offset_basis
	while(s.count-- != 0){
		seed ^= *s.str++;
		seed *= 1099511628211; //64bit FNV_prime
	}
	return seed;
}

#endif //KIGU_UNICODE_H
