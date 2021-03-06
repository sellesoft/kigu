#pragma once
#ifndef KIGU_STRING_H
#define KIGU_STRING_H

#ifdef TRACY_ENABLE
#include "Tracy.hpp"
#endif
#include "common.h"
#include "profiling.h" 
#include <cstdio>

#ifndef KIGU_STRING_ALLOCATOR
#  define KIGU_STRING_ALLOCATOR stl_allocator
#endif

struct string{
	typedef char CHAR;
	static constexpr u32 CHAR_SIZE = sizeof(CHAR);
	
	u32   count;
	u32   space;
	CHAR* str;
	
	Allocator* allocator;
	
	string();
	string(Allocator* a);
	string(const CHAR* s, Allocator* a = KIGU_STRING_ALLOCATOR);
	string(const CHAR* s, u32 count, Allocator* a = KIGU_STRING_ALLOCATOR);
	string(const string& s, Allocator* a = KIGU_STRING_ALLOCATOR);
	string(const cstring& s, Allocator* a = KIGU_STRING_ALLOCATOR);
	~string();
	
	CHAR&  operator[](u32 idx);
	CHAR   operator[](u32 idx) const;
	void   operator= (const CHAR* s);
	void   operator= (const string& s);
	void   operator+=(const CHAR* s);
	void   operator+=(const string& s);
	string operator--(int);
	string operator+ (const CHAR* c) const;
	string operator+ (const string& s) const;
	bool   operator==(const string& s) const;
	bool   operator!=(const string& s) const;
	bool   operator==(const CHAR* s) const;
	bool   operator!=(const CHAR* s) const;
	friend string operator+ (const CHAR* c, const string& s);
	inline explicit operator bool(){ return count; }
	
	void   reserve(u32 _space);
	void   clear();
	void   erase(u32 idx);
	void   insert(CHAR c, u32 idx); //inserts at specified idx, pushing the CHARacter at idx and all following CHARacters to the right
	void   replace(CHAR c, const string& with); //replaces all occurences of a char with a string
	CHAR   at(u32 idx) const; //returns a copy of the CHARacter at idx
	string substr(u32 start, u32 end = npos) const; //returns a string including the start and end CHARacters, end equals the end of the string (size-1) if npos
	u32    findFirstStr(const string& s) const;
	u32    findFirstChar(CHAR c, u32 offset = 0) const; //returns first of CHAR from offset
	u32    findFirstCharNot(CHAR c, u32 offset = 0) const; //returns first of CHAR from offset backwards
	u32    findLastChar(CHAR c, u32 offset = 0) const;
	u32    findLastCharNot(CHAR c) const;
	u32    CHARCount(CHAR c) const; //returns how many times a CHAR appears in the string
	string substrToChar(CHAR c) const; //returns a substring from the beginning to specifiec CHAR, not including the CHAR
	b32    beginsWith(const string& s) const;
	b32    endsWith(const string& s) const;
	b32    contains(const string& s) const;
	
	static string eatSpacesLeading(const string& s);
	static string eatSpacesTrailing(const string& s);
	static string toUpper(const string& s);
	static string toLower(const string& s);
};


///////////////////////
//// @constructors ////
///////////////////////
inline string::string(){DPZoneScoped;
	allocator = KIGU_STRING_ALLOCATOR;
	count = 0;
	space = 0;
	str   = 0;
};

inline string::string(Allocator* a){DPZoneScoped;
	allocator = a;
	count = 0;
	space = 0;
	str   = 0;
};

inline string::string(const CHAR* s, Allocator* a){
	allocator = a;
	count = strlen(s);
	space = RoundUpTo(count+1, 4);
	str   = (CHAR*)allocator->reserve(space*CHAR_SIZE); Assert(str, "Failed to allocate memory");
	allocator->commit(str, space*CHAR_SIZE);
	memcpy(str, s, count*CHAR_SIZE);
}

inline string::string(const CHAR* s, u32 _size, Allocator* a){DPZoneScoped;
	allocator = a;
	count  = _size;
	space  = RoundUpTo(count+1, 4);
	str    = (CHAR*)allocator->reserve(space*CHAR_SIZE); Assert(str, "Failed to allocate memory");
	allocator->commit(str, space*CHAR_SIZE);
	memcpy(str, s, count*CHAR_SIZE);
}

inline string::string(const string& s, Allocator* a){DPZoneScoped;
	allocator = a;
	count = s.count;
	space = RoundUpTo(count + 1, 4);
	str   = (CHAR*)allocator->reserve(space * CHAR_SIZE); Assert(str, "Failed to allocate memory");
	allocator->commit(str, space * CHAR_SIZE);
	memcpy(str, s.str, count * CHAR_SIZE);
}

inline string::string(const cstring& s, Allocator* a){DPZoneScoped;
	allocator = a;
	count = s.count;
	space = RoundUpTo(count + 1, 4);
	str   = (CHAR*)allocator->reserve(space * CHAR_SIZE); Assert(str, "Failed to allocate memory");
	allocator->commit(str, space * CHAR_SIZE);
	memcpy(str, s.str, count * CHAR_SIZE);
}

inline string::~string(){DPZoneScoped;
	if(allocator){
		allocator->release(str);
	}
}


////////////////////
//// @operators ////
////////////////////
inline string::CHAR& string::operator[](u32 idx){DPZoneScoped;
	Assert(idx < count+1);
	return str[idx];
}

inline string::CHAR string::operator[](u32 idx) const {DPZoneScoped;
	Assert(idx < count + 1);
	return str[idx];
}

inline void string::operator= (const CHAR* s){DPZoneScoped;
	if(allocator){
		allocator->release(str);
	}else{
		allocator = KIGU_STRING_ALLOCATOR;
	}
	
	count = strlen(s);
	space = RoundUpTo(count+1, 4);
	str   = (CHAR*)allocator->reserve(space*CHAR_SIZE); Assert(str, "Failed to allocate memory");
	allocator->commit(str, space*CHAR_SIZE);
	memcpy(str, s, count*CHAR_SIZE);
}

inline void string::operator= (const string& s){DPZoneScoped;
	if(allocator) allocator->release(str);
	allocator = s.allocator;
	
	count = s.count;
	space = RoundUpTo(count+1, 4);
	str   = (CHAR*)allocator->reserve(space*CHAR_SIZE); Assert(str, "Failed to allocate memory");
	allocator->commit(str, space*CHAR_SIZE); 
	memcpy(str, s.str, count*CHAR_SIZE);
}

inline void string::operator+=(const CHAR* s){DPZoneScoped;
	u32 old_len = count;
	u32 str_len = strlen(s);
	if(str_len == 0) return;
	count += str_len;
	
	if(!allocator) allocator = KIGU_STRING_ALLOCATOR;
	if(space == 0){
		space = RoundUpTo(count+1, 4);
		str   = (CHAR*)allocator->reserve(space*CHAR_SIZE); Assert(str, "Failed to allocate memory");
		allocator->commit(str, space*CHAR_SIZE);
		memcpy(str, s, count*CHAR_SIZE);
	}else if(space < count+1){
		space = RoundUpTo(count+1, 4);
		str   = (CHAR*)allocator->resize(str, space*CHAR_SIZE); Assert(str, "Failed to allocate memory");
		memcpy(str+old_len, s, (str_len+1)*CHAR_SIZE);
	}else{
		memcpy(str+old_len, s, (str_len+1)*CHAR_SIZE);
	}
}

inline void string::operator+=(const string& s){DPZoneScoped;
	u32 old_len = count;
	u32 str_len = s.count;
	if(str_len == 0) return;
	count += str_len;
	
	if(!allocator) allocator = KIGU_STRING_ALLOCATOR;
	if(space == 0){
		space = RoundUpTo(count+1, 4);
		str = (CHAR*)allocator->reserve(space*CHAR_SIZE); Assert(str, "Failed to allocate memory");
		allocator->commit(str, space*CHAR_SIZE);
		memcpy(str, s.str, count*CHAR_SIZE);
	}else if(space < count+1){
		space = RoundUpTo(count+1, 4);
		str = (CHAR*)allocator->resize(str, space*CHAR_SIZE); Assert(str, "Failed to allocate memory");
		memcpy(str+old_len, s.str, (str_len+1)*CHAR_SIZE);
	}else{
		memcpy(str+old_len, s.str, (str_len+1)*CHAR_SIZE);
	}
}

inline string string::operator--(int){DPZoneScoped;
	if(count == 0) return *this;
	str[--count] = '\0';
	return *this;
}

inline string string::operator+ (const CHAR* c) const{DPZoneScoped;
	if(count == 0) return string(c);
	u32 str_len = strlen(c);
	if(str_len == 0) return *this;
	
	string result;
	result.count  = count + str_len;
	result.space = RoundUpTo(result.count+1, 4);
	result.str = (CHAR*)result.allocator->reserve(result.space*CHAR_SIZE); Assert(result.str, "Failed to allocate memory");
	result.allocator->commit(result.str, result.space*CHAR_SIZE);
	memcpy(result.str,       str, count*CHAR_SIZE);
	memcpy(result.str+count, c,   str_len*CHAR_SIZE);
	return result;
}

inline string string::operator+(const string& s) const{DPZoneScoped;
	if(s.count == 0) return *this;
	
	string result;
	result.count  = count + s.count;
	result.space = RoundUpTo(result.count+1, 4);
	result.str = (CHAR*)result.allocator->reserve(result.space*CHAR_SIZE); Assert(result.str, "Failed to allocate memory");
	result.allocator->commit(result.str, result.space*CHAR_SIZE);
	memcpy(result.str,       str,   count*CHAR_SIZE);
	memcpy(result.str+count, s.str, s.count*CHAR_SIZE);
	return result;
}

inline bool string::operator==(const string& s) const{DPZoneScoped;
	return !strcmp(str, s.str);
}

inline bool string::operator==(const CHAR* s) const{DPZoneScoped;
	return !strcmp(str, s);
}

inline bool string::operator!=(const string& s) const {DPZoneScoped;
	return strcmp(str, s.str);
}

inline bool string::operator!=(const CHAR* s) const {DPZoneScoped;
	return strcmp(str, s);
}


////////////////////////////
//// @special operators ////
////////////////////////////
inline string operator+ (const string::CHAR* c, const string& s){DPZoneScoped;
	return string(c) + s;
}

////////////////////
//// @functions ////
////////////////////
inline void string::reserve(u32 _space){DPZoneScoped;
	if(_space > space){
		space = RoundUpTo(_space+1, 4);
		str = (CHAR*)allocator->resize(str, space*CHAR_SIZE); Assert(str, "Failed to allocate memory");
	}
}

inline void string::clear(){DPZoneScoped;
	if(allocator){
		allocator->release(str);
	}else{
		allocator = KIGU_STRING_ALLOCATOR;
	}
	count = 0;
	space = 0;
	str   = 0;
}

inline void string::erase(u32 idx){DPZoneScoped;
	Assert(idx < count && idx >= 0);
	if (count == 1) clear();
	else memmove(str+idx, str+idx+1, (count-- - idx)*CHAR_SIZE);
}

inline void string::insert(CHAR c, u32 idx){DPZoneScoped;
	Assert(idx <= count);
	count += 1;
	
	if(!allocator) allocator = KIGU_STRING_ALLOCATOR;
	if(space == 0){
		space = 4;
		str = (CHAR*)allocator->reserve(space*CHAR_SIZE); Assert(str, "Failed to allocate memory");
		allocator->commit(str, space*CHAR_SIZE);
		str[0] = c;
	}else if(space < count+1){
		space = RoundUpTo(count+1, 4);
		str = (CHAR*)allocator->resize(str, space*CHAR_SIZE); Assert(str, "Failed to allocate memory");
		memmove(str+idx+1, str+idx, (count-idx)*CHAR_SIZE);
		str[idx] = c;
	}else{
		memmove(str+idx+1, str+idx, (count-idx)*CHAR_SIZE);
		str[idx] = c;
	}
}

inline void string::replace(CHAR c, const string& with) {DPZoneScoped;
	//TODO implement this better
	for (u32 i = 0; i < count; i++) {
		if (str[i] == c) {
			erase(i);
			for (u32 o = with.count; o != 0; o--) {
				insert(with[o - 1], i);
			}
		}
	}
}

inline string::CHAR string::at(u32 idx) const{DPZoneScoped;
	Assert(idx <= count);
	return str[idx];
}

inline string string::substr(u32 start, u32 end) const{DPZoneScoped;
	if(end == npos) end = count-1;
	Assert(start <= count && end <= count && start <= end, "check start/end vars");
	return string(str+start, (end-start)+1);
}

inline u32 string::findFirstStr(const string& s) const{DPZoneScoped;
	for(u32 i = 0; i < count; ++i){
		if(strncmp(str+i, s.str, s.count) == 0) return i;
	}
	return npos;
}

inline u32 string::findFirstChar(CHAR c, u32 offset) const{DPZoneScoped;
	for(u32 i = offset; i < count; ++i){
		if(str[i] == c) return i;
	}
	return npos;
}

inline u32 string::findFirstCharNot(CHAR c, u32 offset) const{DPZoneScoped;
	for(u32 i = 0; i < count; ++i){
		if(str[i] != c) return i;
	}
	return npos;
}

inline u32 string::findLastChar(CHAR c, u32 offset) const{DPZoneScoped;
	Assert(offset < count);
	for(u32 i = (offset != 0 ? offset : count - 1); i != 0; --i){
		if(str[i] == c) return i;
	}
	return npos;
}

inline u32 string::findLastCharNot(CHAR c) const{DPZoneScoped;
	for(u32 i = count-1; i != 0; --i){
		if(str[i] != c) return i;
	}
	return npos;
}

inline u32 string::CHARCount(CHAR c) const{DPZoneScoped;
	u32 sum = 0;
	for(u32 i = 0; i < count; ++i){ if(str[i] == c){ sum++; } }
	return sum;
}

inline string string::substrToChar(CHAR c) const{DPZoneScoped;
	u32 idx = findFirstChar(c);
	return (idx != npos) ? string(str, idx) : *this;
}

inline b32 string::beginsWith(const string& s) const{DPZoneScoped;
	if (s.count > count) return false;
	return !memcmp(str, s.str, s.count);
}

inline b32 string::endsWith(const string& s) const{DPZoneScoped;
	if (s.count > count) return false;
	return !memcmp(str + (count - s.count), s.str, s.count);
}

inline b32 string::contains(const string& s) const{DPZoneScoped;
	if (s.count > count) return false;
	for (u32 i = 0; i < count - s.count; i++) {
		if (!memcmp(s.str, str + i, s.count)) return true;
	}
	return false;
}


///////////////////////////
//// @static functions ////
///////////////////////////
inline string string::eatSpacesLeading(const string& text){DPZoneScoped;
	u32 idx = text.findFirstCharNot(' ');
	return (idx != npos) ? text.substr(idx) : string();
}


inline string string::eatSpacesTrailing(const string& text){DPZoneScoped;
	u32 idx = text.findLastCharNot(' ');
	return (idx != npos) ? text.substr(0, idx+1) : string();
}


inline string string::toUpper(const string& in){DPZoneScoped;
	string result = in;
	forI(result.count) if(result.str[i] >= 'a' && result.str[i] <= 'z') result.str[i] -= 32;
	return result;
}


inline string string::toLower(const string& in){DPZoneScoped;
	string result = in;
	forI(result.count) if(result.str[i] >= 'A' && result.str[i] <= 'Z') result.str[i] += 32;
	return result;
}

#endif //KIGU_STRING_H