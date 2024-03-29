#pragma once
#ifndef KIGU_MAP_H
#define KIGU_MAP_H

#include "common.h"
#include "arrayT.h"
#include "hash.h"
#include "pair.h"
#include "profiling.h"

template<typename Key, typename Value, typename HashStruct = hash<Key>>
struct map{
	arrayT<u32>   hashes;
	arrayT<Value> data;
	u32 count;
	
	map(Allocator* a = stl_allocator);
	map(std::initializer_list<pair<Key,Value>> list, Allocator* a = stl_allocator);
	
	Value& operator[](const Key& key);
	Value& operator[](u32 idx);
	
	void   clear();
	u32    add(const Key& key); //returns index of added or existing key
	u32    add(const Key& key, const Value& value);
	void   remove(const Key& key);
	void   swap(u32 idx1, u32 idx2);
	bool   has(const Key& key) const;
	Value* at(const Key& key);
	Value  atPtrVal(const Key& key); //use when value is already a pointer
	Value* atIdx(u32 index);
	Value  atIdxPtrVal(u32 index); //use when value is already a pointer
	u32    findkey(const Key& key) const; //returns index of key if it exists
	
	Value* begin(){DPZoneScoped; return data.begin(); }
	Value* end()  {DPZoneScoped; return data.end(); }
	const Value* begin()const{DPZoneScoped; return data.begin(); }
	const Value* end()  const{DPZoneScoped; return data.end(); }
};

template<typename Key, typename HashStruct = hash<Key>>
using set = map<Key,Key,HashStruct>;

//////////////////////
//// @contructors ////
//////////////////////
template<typename Key, typename Value, typename HashStruct> inline map<Key,Value,HashStruct>::
map(Allocator* a){DPZoneScoped;
	hashes.allocator = a;
	data.allocator = a;
	count = 0;
}

template<typename Key, typename Value, typename HashStruct> inline map<Key,Value,HashStruct>::
map(std::initializer_list<pair<Key,Value>> list, Allocator* a){DPZoneScoped;
	hashes.allocator = a;
	data.allocator = a;
	count = 0;
	
	for (auto& p : list){
		add(p.first, p.second);
	}
}

////////////////////
//// @operators ////
////////////////////
template<typename Key, typename Value, typename HashStruct> inline Value& map<Key,Value,HashStruct>::
operator[](const Key& key){DPZoneScoped;
	u32 hashed = HashStruct{}(key);
	forI(hashes.count){ if(hashed == hashes[i]){ return data[i]; } }
	throw "nokey";
}

template<typename Key, typename Value, typename HashStruct> inline Value& map<Key,Value,HashStruct>::
operator[](u32 idx){
	return data[idx];
}

////////////////////
//// @functions ////
////////////////////
template<typename Key, typename Value, typename HashStruct> inline void map<Key,Value,HashStruct>::
clear(){DPZoneScoped;
	hashes.clear();
	data.clear();
	count = 0;
}

template<typename Key, typename Value, typename HashStruct> inline u32 map<Key,Value,HashStruct>::
add(const Key& key){DPZoneScoped;
	u32 hashed = HashStruct{}(key);
	forI(hashes.count){ if(hashed == hashes[i]){ return i; } }
	hashes.add(hashed);
	data.add(Value());
	count++;
	return count-1;
}

template<typename Key, typename Value, typename HashStruct> inline u32 map<Key,Value,HashStruct>::
add(const Key& key, const Value& value){DPZoneScoped;
	u32 hashed = HashStruct{}(key);
	forI(hashes.count){ if(hashed == hashes[i]){ return i; } }
	hashes.add(hashed);
	data.add(value);
	count++;
	return count-1;
}

template<typename Key, typename Value, typename HashStruct> inline void map<Key,Value,HashStruct>::
remove(const Key& key){DPZoneScoped;
	u32 hashed = HashStruct{}(key);
	forI(hashes.count){
		if(hashed == hashes[i]){
			data.remove(i);
			hashes.remove(i);
			count--;
			return;
		}
	}
}

template<typename Key, typename Value, typename HashStruct> inline void map<Key,Value,HashStruct>::
swap(u32 idx1, u32 idx2){DPZoneScoped;
	hashes.swap(idx1, idx2);
	data.swap(idx1, idx2);
}

template<typename Key, typename Value, typename HashStruct> inline bool map<Key,Value,HashStruct>::
has(const Key& key)const{DPZoneScoped;
	u32 hashed = HashStruct{}(key);
	forI(hashes.count){ if(hashed == hashes[i]){ return true; } }
	return false;
}

template<typename Key, typename Value, typename HashStruct> inline Value* map<Key,Value,HashStruct>::
at(const Key& key){DPZoneScoped;
	u32 hashed = HashStruct{}(key);
	forI(hashes.count){ if(hashed == hashes[i]){ return &data[i]; } }
	return 0;
}

template<typename Key, typename Value, typename HashStruct> inline Value map<Key,Value,HashStruct>::
atPtrVal(const Key& key){DPZoneScoped;
	u32 hashed = HashStruct{}(key);
	forI(hashes.count){ if(hashed == hashes[i]){ return data[i]; } }
	return 0;
}

template<typename Key, typename Value, typename HashStruct> inline Value* map<Key,Value,HashStruct>::
atIdx(u32 index){DPZoneScoped;
	return &data[index];
}

template<typename Key, typename Value, typename HashStruct> inline Value map<Key,Value,HashStruct>::
atIdxPtrVal(u32 index){DPZoneScoped;
	return data[index];
}


template<typename Key, typename Value, typename HashStruct> inline u32 map<Key,Value,HashStruct>::
findkey(const Key& key)const{
	u32 hashed = HashStruct{}(key);
	forI(hashes.count){ if(hashed == hashes[i]){ return i; } }
	return -1;
}


#endif //KIGU_MAP_H