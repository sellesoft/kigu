#pragma once
#ifndef KIGU_HASH_H
#define KIGU_HASH_H

#include "common.h"
#include "array.h"
#include "string.h"

template<int N>
static constexpr const u32 compile_time_string_hash(const char(&a)[N]){
	const char* const p(a);
	u32 seed = 2166136261;
	size_t data_size = N-1;
	const u8* data = (const u8*)&a;
	while (data_size-- != 0) {
		seed ^= *data++;
		seed *= 16777619;
	}
	return seed;
}

template<class T>
struct hash {
	
	hash() {};
	
	inline u32 operator()(const T& v)const {
		u32 seed = 2166136261;
		size_t data_size = sizeof(T);
		const u8* data = (const u8*)&v;
		while (data_size-- != 0) {
			seed ^= *data++;
			seed *= 16777619;
		}
		return seed;
		
	}
	
	inline u32 operator()(T* v)const {
		u32 seed = 2166136261;
		size_t data_size = sizeof(T);
		const u8* data = (const u8*)v;
		while (data_size-- != 0) {
			seed ^= *data++;
			seed *= 16777619;
		}
		return seed;
		
	}
};

template<> 
struct hash<string> {
	inline u32 operator()(const string& s) {
		u32 seed = 2166136261;
		u32 size = s.count+1;
		while (size-- != 0) {
			seed ^= s.str[size];
			seed *= 16777619;
		}
		return seed;
	}
};

template<> 
struct hash<cstring> {
	inline u32 operator()(cstring s) {
		u32 seed = 2166136261;
		u32 size = s.count;
		while (size-- != 0) {
			seed ^= s.str[size];
			seed *= 16777619;
		}
		return seed;
	}
};

template<> 
struct hash<const char*> {
	inline u32 operator()(const char* s) {
		u32 seed = 2166136261;
		u32 size = strlen(s)+1;
		while (size-- != 0) {
			seed ^= s[size];
			seed *= 16777619;
		}
		return seed;
	}
};

template<class T> 
struct hash<array<T>> {
	inline u32 operator()(array<T>* s) {
		u32 seed = 2166136261;
		u32 size = s->size;
		while (size-- != 0) {
			seed ^= s->operator[](size - 1);
			seed *= 16777619;
		}
		return seed;
	}
};

#endif //KIGU_HASH_H