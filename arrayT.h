#pragma once
#ifndef KIGU_ARRAYT_H
#define KIGU_ARRAYT_H

#ifndef KIGU_ARRAY_GROWTH_FACTOR
#  define KIGU_ARRAY_GROWTH_FACTOR 2
#endif
#ifndef KIGU_ARRAY_SPACE_ALIGNMENT
#  define KIGU_ARRAY_SPACE_ALIGNMENT 4
#endif
#ifndef KIGU_ARRAY_ALLOCATOR
#  define KIGU_ARRAY_ALLOCATOR stl_allocator
#endif

#ifdef TRACY_ENABLE
#include "tracy/Tracy.hpp"
#endif

#include "common.h"
#include "profiling.h"

#include <cstdlib>
#include <cstring>
#include <initializer_list>


template<typename T>
struct arrayT{
	u32 count;
	u32 space; //number of items array can fit
	T*  data;
	T*  first;
	T*  last;
	T*  iter;
	Allocator* allocator;
	
	arrayT();
	arrayT(Allocator* a);
	arrayT(u32 _count, Allocator* a = KIGU_ARRAY_ALLOCATOR);
	arrayT(std::initializer_list<T> l, Allocator* a = KIGU_ARRAY_ALLOCATOR);
	arrayT(const arrayT<T>& array, Allocator* a = KIGU_ARRAY_ALLOCATOR);
	arrayT(T* _data, u32 _count, Allocator* a = KIGU_ARRAY_ALLOCATOR);
	arrayT(carray<T> arr, Allocator* a = KIGU_ARRAY_ALLOCATOR);
	~arrayT();
	
	//copies the values and allocator from rhs
	arrayT<T>& operator= (const arrayT<T>& rhs);
	T& operator[](u32 i);
	T  operator[](u32 i) const;
	
	u32  size() const;
	void add(const T& t);
	void add_array(const arrayT<T>& t);
	//for taking in something without copying it
	void emplace(const T& t);
	//inserts before the specified idx
	void insert(const T& t, u32 idx);
	//removes _count elements from the end and returns the last item popped
	T pop(u32 _count = 1);
	//removes element at i and shifts all following elements down one
	void remove(u32 i);
	//sets element at i to the last element and zeros last element
	//NOTE does not explicitly destruct element at i as it uses operator= do replace value at i
	void remove_unordered(u32 i);
	//removes all elements but DOES NOT affect space
	void clear();
	//resizes space for count elements and zero-inits any new elements
	void resize(u32 _count);
	//allocates space for count elements
	void reserve(u32 nuspace);
	//swaps two elements in the array
	void swap(u32 idx1, u32 idx2);
	bool has(const T& value);
	//this is really only necessary for the copy constructor as far as i know
	T&   at(u32 i);
	
	//TODO add out of bounds checking for these functions
	//returns the value of iter and increments it by one
	T& next(u32 count = 0);
	//returns the value of iter + some value and doesn't increment it 
	T& peek(u32 i = 1);
	//returns the value of iter and decrements it by one
	T& prev(u32 count = 0);
	T& lookback(u32 i = 1);
	//iterator functions that return pou32ers if the object isnt already one
	T* nextptr();
	//returns the value of iter + some value and doesn't increment it 
	T* peekptr(u32 i = 1);
	//returns the value of iter and decrements it by one
	T* prevptr();
	T* lookbackptr(u32 i = 1);
	void setiter(u32 i);
	
	//begin/end functions for for-each loops
	inline T* begin(){ return &data[0]; }
	inline T* end()  { return &data[count]; }
	inline const T* begin()const{ return &data[0]; }
	inline const T* end()  const{ return &data[count]; }
};

///////////////////////
//// @constructors ////
///////////////////////
template<typename T> inline arrayT<T>::
arrayT(){DPZoneScoped;
	allocator = KIGU_ARRAY_ALLOCATOR;
	
	space = 0;
	count = 0;
	data  = 0;
	first = 0;
	iter  = 0;
	last  = 0;
}

template<typename T> inline arrayT<T>::
arrayT(Allocator* a){DPZoneScoped;
	allocator = a;
	
	space = 0;
	count = 0;
	data  = 0;
	first = 0;
	iter  = 0;
	last  = 0;
}

template<typename T> inline arrayT<T>::
arrayT(u32 _count, Allocator* a){DPZoneScoped;
	allocator = a;
	
	count = 0;
	space = RoundUpTo(_count, KIGU_ARRAY_SPACE_ALIGNMENT);
	data  = (T*)allocator->reserve(space*sizeof(T));
	
	first = 0;
	iter  = 0;
	last  = 0;
}

template<typename T> inline arrayT<T>::
arrayT(std::initializer_list<T> l, Allocator* a){DPZoneScoped;
	allocator = a;
	
	count = l.size();
	space = RoundUpTo(l.size(), KIGU_ARRAY_SPACE_ALIGNMENT);
	data  = (T*)allocator->reserve(space*sizeof(T));
	
	forI(l.size()) data[i] = *(l.begin()+i);
	
	first = data;
	iter  = data;
	last  = &data[count - 1];
}

//TODO this can probably be much better
//its necessary so when we return elements the entire array copies properly
//so we have to make sure everything in the array gets recreated
template<typename T> inline arrayT<T>::
arrayT(const arrayT<T>& array, Allocator* a){DPZoneScoped;
	allocator = a;
	
	count = array.count;
	space = array.space;
	data  = (T*)allocator->reserve(space*sizeof(T));
	
	forI(array.count) data[i] = array.data[i];
	
	first = data;
	iter  = first;
	last  = (array.last == 0) ? 0 : data+(array.count-1);
}

template<typename T> inline arrayT<T>::
arrayT(T* _data, u32 _count, Allocator* a){DPZoneScoped;
	allocator = a;
	
	count = _count;
	space = RoundUpTo(_count, KIGU_ARRAY_SPACE_ALIGNMENT);
	data  = (T*)allocator->reserve(space*sizeof(T));
	memcpy(data, _data, _count*sizeof(T));
	
	first = data;
	iter  = data;
	last  = data + (_count-1);
}

template<typename T> inline arrayT<T>::
arrayT(carray<T> arr, Allocator* a){DPZoneScoped;
	allocator = a;
	
	count = arr.count;
	space = RoundUpTo(arr.count, KIGU_ARRAY_SPACE_ALIGNMENT);
	data  = (T*)allocator->reserve(space*sizeof(T));
	
	forI(arr.count) data[i] = arr.data[i];
	
	first = data;
	iter  = first;
	last  = data+(arr.count-1);
}

template<typename T> inline arrayT<T>::
~arrayT(){
	forI(count){ data[i].~T(); }
	allocator->release(data);
	space = 0;
	count = 0;
	data  = 0;
	first = 0;
	iter  = 0;
	last  = 0;
}

////////////////////
//// @operators ////
////////////////////
template<typename T> inline arrayT<T>& arrayT<T>::
operator= (const arrayT<T>& rhs){
	if(!allocator) allocator = KIGU_ARRAY_ALLOCATOR;
	forI(count){ data[i].~T(); }
	allocator->release(data);  //TODO maybe resize rather than release and reserve
	
	allocator = rhs.allocator;
	space = rhs.space;
	count = rhs.count;
	data  = (T*)allocator->reserve(space*sizeof(T));
	
	forI(rhs.count) data[i] = rhs.data[i];
	
	first = data;
	iter  = data + (rhs.iter - rhs.first);
	last  = data + (count-1);
	return *this;
}

template<typename T> inline T& arrayT<T>::
operator[](u32 i){
	Assert(i < count);
	return data[i];
}

template<typename T> inline T arrayT<T>::
operator[](u32 i) const {
	Assert(i < count);
	return data[i];
}


////////////////////
//// @functions ////
////////////////////
template<typename T> inline u32 arrayT<T>::
size() const{
	return count;
}

template<typename T> inline void arrayT<T>::
add(const T& t){DPZoneScoped;
	if(space == 0){ //if first item, allocate memory
		space = KIGU_ARRAY_SPACE_ALIGNMENT;
		data  = (T*)allocator->reserve(space*sizeof(T));
		
		first = data;
		iter  = data;
		last  = data;
		
		data[0] = t;
		count = 1;
	}else if(count == space){ //if array is full, resize the memory by the growth factor
		space *= KIGU_ARRAY_GROWTH_FACTOR;
		data = (T*)allocator->resize(data, space*sizeof(T));
		memset(data+count, 0, (space-count)*sizeof(T)); //NOTE STL doesnt guarantee memory is zero on realloc
		
		iter  = data + (iter - first);
		first = data;
		last  = data + count;
		
		data[count] = t;
		count++;
	}else{
		last = data + count;
		data[count] = t;
		count++;
	}
}

template<typename T> inline void arrayT<T>::
add_array(const arrayT<T>& t){DPZoneScoped;
	for(const T& item : t){
		this->add(item);
	}
}

template<typename T> inline void arrayT<T>::
emplace(const T& t){DPZoneScoped;
	this->add(t); //TODO emplace function signature should mimic the type's constructor
}

template<typename T> inline void arrayT<T>::
insert(const T& t, u32 idx){DPZoneScoped;
	Assert(idx <= count);
	if(space == 0){ //if first item, allocate memory
		space = KIGU_ARRAY_SPACE_ALIGNMENT;
		data  = (T*)allocator->reserve(space*sizeof(T));
		
		first = data;
		iter  = data;
		last  = data;
		
		count = 1;
		data[0] = t;
	}else if(count == space){ //if array is full, resize the memory by the growth factor
		space *= KIGU_ARRAY_GROWTH_FACTOR;
		data = (T*)allocator->resize(data, space*sizeof(T));
		memset(data+count, 0, (space-count)*sizeof(T)); //NOTE STL doesnt guarantee memory is zero on realloc
		memmove(data+idx+1, data+idx, (count-idx)*sizeof(T));
		memset(data+idx, 0, 1*sizeof(T));
		
		iter  = data + (iter - first);
		first = data;
		last  = data + count;
		
		data[idx] = t;
		count++;
	}else if(idx == count){
		this->add(t);
	}else{
		memmove(data+idx+1, data+idx, (count-idx)*sizeof(T));
		memset(data+idx, 0, 1*sizeof(T));
		data[idx] = t;
		count++;
		last++;
	}
}

template<typename T> inline T arrayT<T>::
pop(u32 _count){DPZoneScoped;
	Assert(count >= _count, "attempted to pop more than array size");
	T ret;
	forI(_count){
		if(i==_count) memcpy(&ret, last, sizeof(T));
		last->~T();
		last--;
		count--;
	}
	memset(last+1, 0, _count*sizeof(T));
	if(count == 0){
		first = 0;
		last  = 0;
		iter  = 0;
	}
	return ret;
}	

template<typename T> inline void arrayT<T>::
remove(u32 i){DPZoneScoped;
	Assert(count > 0, "can't remove element from empty array");
	Assert(i < count, "index is out of bounds");
	data[i].~T();
	for(u32 o = i; o < count-1; o++){
		data[o] = data[o+1];
	}
	memset(last, 0, sizeof(T));
	last--;
	count--;
	if(count == 0){
		first = 0;
		last  = 0;
		iter  = 0;
	}
}

template<typename T> inline void arrayT<T>::
remove_unordered(u32 i){DPZoneScoped;
	Assert(count > 0, "can't remove element from empty array");
	Assert(i < count, "index is out of bounds");
	T endval = data[count-1];
	memset(last, 0, sizeof(T));
	last--;
	count--;
	if(count == 0){
		first = 0;
		last  = 0;
		iter  = 0;
	}else{
		data[i] = endval;
	}
}

//TODO(sushi) impl this
//void remove(u32 i1, u32 i2){
//	Assert(count > 0, "can't remove element from empty vector");
//	Assert(i < count, "index is out of bounds");
//	data[i].~T();
//	for(u32 o = i2; o < size(); o++){
//		data[o] = data[o + 1];
//	}
//	memset(last, 0, sizeof(T));
//	last--;
//	count--;
//}

template<typename T> inline void arrayT<T>::
clear(){DPZoneScoped;
	forI(count){ data[i].~T(); }
	memset(data, 0, count*sizeof(T));
	
	count = 0;
	first = data;
	iter  = data;
	last  = 0;
}

template<typename T> inline void arrayT<T>::
resize(u32 new_count){DPZoneScoped;
	if(!new_count){ this->~arrayT(); return; } //this may not be the appropriate thing to do here
	if(new_count > space){
		space = new_count;
		data = (T*)allocator->resize(data, space*sizeof(T));
		memset(data+count, 0, (new_count-count)*sizeof(T)); //NOTE STL doesnt guarantee memory is zero on realloc
		count = new_count;
		
		iter  = data + (iter - first);
		first = data;
		last  = data + (space-1);
	}else if(new_count < space){
		for(u32 i = new_count+1; i < count; ++i){ data[i].~T(); }
		
		count = new_count;
		space = new_count;
		data = (T*)allocator->resize(data, space*sizeof(T));
		
		iter  = data + (iter - first); //TODO check that iter isnt beyond last
		first = data;
		last  = data + (space-1);
	}
}

template<typename T> inline void arrayT<T>::
reserve(u32 new_space){DPZoneScoped;
	if(new_space > space){
		space = RoundUpTo(new_space, KIGU_ARRAY_SPACE_ALIGNMENT);
		data = (T*)allocator->resize(data, space*sizeof(T));
		memset(data+count, 0, (new_space-count)*sizeof(T)); //NOTE STL doesnt guarantee memory is zero on realloc
		
		iter  = data + (iter - first);
		first = data;
		last  = (count) ? data + (count-1) : 0;
	}
}

template<typename T> inline void arrayT<T>::
swap(u32 idx1, u32 idx2){DPZoneScoped;
	Assert(idx1 < count && idx2 < count, "index out of bounds");
	Assert(idx1 != idx2, "can't swap an element with itself");
	T save = data[idx1];
	data[idx1] = data[idx2];
	data[idx2] = save;
}

template<typename T> inline bool arrayT<T>::
has(const T& value){DPZoneScoped;
	for(const T& blahabuasjdas : *this){
		if(blahabuasjdas == value){
			return true;
		}
	}
	return false;
}

template<typename T> inline T& arrayT<T>::
at(u32 i){DPZoneScoped;
	Assert(i < count);
	return data[i];
}

template<typename T> inline T& arrayT<T>::
next(u32 count){DPZoneScoped;
	if (last - iter + 1 >= 0) {
		return iter += count, *iter;
	}
	return *iter;
}

template<typename T> inline T& arrayT<T>::
peek(u32 i){DPZoneScoped;
	if(last - iter + 1 >= 0) return *(iter + i);
	return *iter;
}

template<typename T> inline T& arrayT<T>::
prev(u32 count){DPZoneScoped;
	if (iter - first + 1 >= 0) {
		return iter -= count, *iter;
	}
	return *--iter;
}

template<typename T> inline void arrayT<T>::
setiter(u32 i) {
	iter = data + i;
}

template<typename T> inline T& arrayT<T>::
lookback(u32 i){DPZoneScoped;
	if(first - iter + 1 >= 0) return *(iter - i);
}

template<typename T> inline T* arrayT<T>::
nextptr(){DPZoneScoped;
	if(iter + 1 - last >= 0) return iter++;
	else return nullptr;
}

//TODO come up with a better name for this and the corresponding previous overload
template<typename T> inline T* arrayT<T>::
peekptr(u32 i){DPZoneScoped;
	if(iter + 1 - last >= 0) return iter + i;
	else return nullptr;
}

template<typename T> inline T* arrayT<T>::
prevptr(){DPZoneScoped;
	if(iter - 1 - first >= 0) return iter--;
	else return nullptr;
}

template<typename T> inline T* arrayT<T>::
lookbackptr(u32 i){DPZoneScoped;
	if(iter - 1 - first >= 0) return iter - i;
	else return nullptr;
}

#endif //KIGU_ARRAYT_H