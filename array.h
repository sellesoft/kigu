/* kigu array module
WHAT:
This module provides functions for creating and working with dynamic arrays in a C89-compatible way. Since C does not have
generic types nor the ability to pass types as arguments, the interface to this module is mostly macros that take in a typed
pointer to the first element in the array. This allows the actual function signatures to take in an untyped pointer to the
first element and the size of the type used to fill the array slots. But where do we store the container information such as
the number of elements or space allocated? When you init an array (via array_init() or array_create()), we simply allocate the
space you requested along with a header (ArrayHeader) for that container information. The header is placed right before the
first element, so accessing the container information is as simple as doing a pointer offset from the first index. So, the
overall interface is based on the idea that a user will store the pointer to first element as their reference to the array,
and any functions that can possibly grow the array can reassign that array variable passed in to the new array location.

WHY:
We used stb_ds for a while after moving away from using templated arrays in data structures that will be exposed to C code.
However, the main pain point with using stb_ds was working with allocators, in that the allocators used by stb_ds are selected
at compile-time. So, this module is a simple replacement of stb_ds except that array initialization requires an allocator and we
store that on the array header for later memory operations. Of course though, since C/C++ doesn't have a standardized allocator
structure, we rely on kigu's common.h which defines an Allocator struct used all throughout kigu and other sellesoft projects.

NOTES:
- None of the functions do error checking on the values you give them.
- The space of an array doubles in size if necessary when pushing new slots.
- Memory movement and copying operations don't call C++ constructors or destructor, just pure memory copying.
- Allocation does not zero the memory of new slots as that is left up to the allocator provided (popping and inserting does though).

INDEX:
@array_header
  ArrayHeader: struct
  array_header(T* array) -> ArrayHeader*
  array_count(T* array) -> upt
  array_space(T* array) -> upt
  array_allocator(T* array) -> Allocator*
  array_last(T* array) -> T*
@array_init
  array_init(T* array, upt space, Allocator* allocator) -> void*
  array_init_with_count(T* array, upt count, Allocator* allocator) -> void*
  array_create(T type, upt space, Allocator* allocator) -> T*
  array_create_with_count(T type, upt count, Allocator* allocator) -> T*
  array_deinit(T* array) -> void
@array_modify
  array_grow(T* array, upt count) -> T*
  array_clear(T* array) -> void
  array_push(T* array) -> T*
  array_push_value(T* array, T value) -> T*
  array_pop(T* array) -> void
  array_insert(T* array, upt index) -> T*
  array_insert_value(T* array, upt index, T value) -> T*
  array_remove_ordered(T* array, upt index) -> void
  array_remove_unordered(T* array, upt index) -> void
@array_loop
  for_array(array){ it... }
  for_array(var, array){ var... }
@array_wrappers
@array_shorthands
@array_cpp
@array_tests
@array_license

TODO:
- Maybe have some compile-time optimizations on the type_size when moving memory? (via c++ constexpr or C99 generic selection macro)
	We currently iterate by u8 because we don't know type_size at compile-time, but we could iterate less by copying larger chunks
	of memory (32, 64, 128, 256), but before pursuing that though, check if the CPU/compiler doesn't already optimize this away thru
	some other means or if copying larger chunks of memory doesn't just decompose into u8 copies (unlikely on most modern hardware).
- Rewrite the WHAT section to be more readable.

REFERENCES:
https://github.com/nothings/stb/blob/master/stb_ds.h  (Sean Barret, nothings, dynamic array)
https://github.com/Jai-Community/Jai-Community-Library/wiki/Getting-Started#for_expansion  (Jai, for loop expansion macro)

LICENSE:
Placed in the public domain and also MIT licensed. See end of file for detailed license information.
*/
#pragma once
#ifndef KIGU_ARRAY_H
#define KIGU_ARRAY_H


#include "common.h"


StartLinkageC();
//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @array_header


typedef struct ArrayHeader{
#ifdef KIGU_ARRAY_MAGIC
	u64 magic;
#endif //#ifdef KIGU_ARRAY_MAGIC
	upt count;
	upt space;
	Allocator* allocator;
}ArrayHeader;


//Returns the header for the `array`
#define array_header(array) ((ArrayHeader*)(array) - 1)


//Returns the number of items in the `array`
#define array_count(array) (array_header(array)->count)


//Returns the number of slots in the `array`
#define array_space(array) (array_header(array)->space)


//Returns the allocator of the `array`
#define array_allocator(array) (array_header(array)->allocator)


//Returns the last item in the `array`
#define array_last(array) ((array) + array_count(array) - 1)


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @array_init


global void* kigu__array_init(upt type_size, upt space, upt count, Allocator* allocator){
	ArrayHeader* header = (ArrayHeader*)allocator->reserve(sizeof(ArrayHeader) + space*type_size);
#ifdef KIGU_ARRAY_MAGIC
	header->magic = 0xf0f0f0f0f0f0f0f0;
#endif //#ifdef KIGU_ARRAY_MAGIC
	header->count = count;
	header->space = space;
	header->allocator = allocator;
	return header+1;
}

//Assigns `array` to an allocation of `space` contiguous slots allocated using `allocator`
#define array_init(array,space,allocator) ((array) = kigu__array_init_wrapper((array), sizeof(*(array)), (space), 0, (allocator)))


//Assigns `array` to an allocation of `count` contiguous slots allocated using `allocator` and sets the count to `count`
#define array_init_with_count(array,count,allocator) ((array) = kigu__array_init_wrapper((array), sizeof(*(array)), (count), (count), (allocator)))


//Creates and returns a `T*` to an allocation of `space` contiguous slots allocated using `allocator`
#define array_create(T,space,allocator) ((T*)kigu__array_init(sizeof(T), (space), 0, (allocator)))


//Creates and returns a `T*` to an allocation of `count` contiguous slots allocated using `allocator` and sets the count to `count`
#define array_create_with_count(T,count,allocator) ((T*)kigu__array_init(sizeof(T), (count), (count), (allocator)))


//Deallocates `array` using the allocator set on init
global void array_deinit(void* array){
	ArrayHeader* header = array_header(array);
	header->allocator->release(header);
}


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @array_modify


//Allocates and assigns `array` to a new array that can hold the slots of `array` plus `count` more items, copies
//  `array` to that new array, and deletes the old allocation
#define array_grow(array,count) ((array) = kigu__array_grow_wrapper((array), sizeof(*(array)), (count)))
global void* kigu__array_grow(void* array, upt type_size, upt count){
	ArrayHeader* header = array_header(array);
	
	//realloc the array with the new space
	upt new_space = header->space + count;
	header = (ArrayHeader*)header->allocator->resize(header, sizeof(ArrayHeader) + new_space*type_size);
	header->space = new_space;
	
	//copy old array to new array
	return header+1;
}


//Clears the `array` by zeroing the used memory, setting the `count` to zero, and leaving the `space` alone
#define array_clear(array) kigu__array_clear((array), sizeof(*(array)))
global void kigu__array_clear(void* array, upt type_size){
	ArrayHeader* header = array_header(array);
	ZeroMemory(array, header->count*type_size);
	header->count = 0;
}


//Reserves (growing and reassigning `array` if necessary) and returns the next slot at the end of `array`
#define array_push(array) ((array) = kigu__array_push_wrapper((array), sizeof(*(array))), array_last(array))
#define array_push_value(array,value) (*array_push(array) = (value), array_last(array))
global void* kigu__array_push(void* array, upt type_size){
	ArrayHeader* header = array_header(array);
	
	//grow if needed
	if(header->count >= header->space){
		array = kigu__array_grow(array, type_size, Max(header->count, 1));
		header = array_header(array);
	}
	
	//reserve next slot
	header->count += 1;
	
	//return the array incase it's moved
	return array;
}


//Zeros and unreserves the last item in `array`
#define array_pop(array) kigu__array_pop(array, sizeof(*(array)));
global void kigu__array_pop(void* array, upt type_size){
	ArrayHeader* header = array_header(array);
	ZeroMemory((u8*)array + (header->count-1)*type_size, type_size);
	header->count -= 1;
}


//Reserves (growing and reassigning `array` if necessary) the next slot at the end of `array`, shifts all items
//  at and after `index` to the right, zeros the slot at `index`, and returns the slot at `index`
#define array_insert(array,index) ((array) = kigu__array_insert_wrapper((array), sizeof(*(array)), (index)), (array)+(index))
#define array_insert_value(array,index,value) (*array_insert((array),(index)) = (value), (array)+(index))
global void* kigu__array_insert(void* array, upt type_size, upt index){
	ArrayHeader* header = array_header(array);
	
	//grow if needed
	if(header->count >= header->space){
		array = kigu__array_grow(array, type_size, Max(header->count, 1));
		header = array_header(array);
	}
	header->count += 1;
	
	//copy each item to the next slot by iterating backwards from
	//the last slot to the desired insert slot
	u8* dst = (u8*)array + (header->count * type_size) - 1;
	u8* src = dst - type_size;
	u8* stop = (u8*)array + (index * type_size);
	while(src >= stop){
		*dst = *src;
		src -= 1;
		dst -= 1;
	}
	
	//zero the desired slot
	ZeroMemory(src, type_size);
	
	//return the array incase it's moved
	return array;
}

//Removes the item at `index` into `array` by shifting all items after `index` to the left then popping the `array`
#define array_remove_ordered(array,index) kigu__array_remove_ordered((array), sizeof(*(array)), (index))
global void kigu__array_remove_ordered(void* array, upt type_size, upt index){
	ArrayHeader* header = array_header(array);
	
	//copy each item to the previous slot by iterating forwards from
	//the index slot to the last slot
	u8* src = (u8*)array + ((index+1) * type_size);
	u8* dst = (u8*)array + (index * type_size);
	u8* stop = (u8*)array + (header->count * type_size);
	while(src < stop){
		*dst = *src;
		src += 1;
		dst += 1;
	}
	
	//unreserve and zero the last slot
	kigu__array_pop(array, type_size);
}


//Removes the item at `index` into `array` by copying the last item to the `index` slot and zeroing the old slot of
//  the last item (faster than remove_ordered because you're only copying and zeroing one item)
#define array_remove_unordered(array,index) kigu__array_remove_unordered((array), sizeof(*(array)), (index))
global void kigu__array_remove_unordered(void* array, upt type_size, upt index){
	ArrayHeader* header = array_header(array);
	CopyMemory((u8*)array + index*type_size, (u8*)array + (header->count-1)*type_size, type_size);
	kigu__array_pop(array, type_size);
}


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @array_loop


#if COMPILER_FEATURE_TYPEOF
#  define for_array(array) for(typeof(*(array))* it = (array); it < ((array) + array_count(array)); ++it)
#  define forX_array(var,array) for(typeof(*(array))* var = (array); var < ((array) + array_count(array)); ++var)
#elif COMPILER_FEATURE_CPP_AUTO //#if COMPILER_FEATURE_TYPEOF
#  define for_array(array) for(auto it = (array); it < ((array) + array_count(array)); ++it)
#  define forX_array(var,array) for(auto var = (array); var < ((array) + array_count(array)); ++var)
#endif //#elif COMPILER_FEATURE_CPP_AUTO //#if COMPILER_FEATURE_TYPEOF


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @array_wrappers
//// NOTE: C can implicitly cast from void* to T*, but C++ can not so templates are required there, thus we have to wrap functions


#if COMPILER_FEATURE_CPP
EndLinkageC();
template<class T> FORCE_INLINE T* kigu__array_init_wrapper(T* array, upt type_size, upt space, upt count, Allocator* allocator){ return (T*)kigu__array_init(type_size, space, count, allocator); }
template<class T> FORCE_INLINE T* kigu__array_grow_wrapper(T* array, upt type_size, upt count){ return (T*)kigu__array_grow(array, type_size, count); }
template<class T> FORCE_INLINE T* kigu__array_push_wrapper(T* array, upt type_size){ return (T*)kigu__array_push(array, type_size); }
template<class T> FORCE_INLINE T* kigu__array_insert_wrapper(T* array, upt type_size, upt index){ return (T*)kigu__array_insert(array, type_size, index); }
StartLinkageC();
#else //#if COMPILER_FEATURE_CPP
FORCE_INLINE void* kigu__array_init_wrapper(void* array, upt type_size, upt count, Allocator* allocator){ return  kigu__array_init(type_size, count, allocator); }
FORCE_INLINE void* kigu__array_grow_wrapper(void* array, upt type_size, upt count){ return kigu__array_grow(array, type_size, count); }
FORCE_INLINE void* kigu__array_push_wrapper(void* array, upt type_size){ return kigu__array_push(array, type_size); }
FORCE_INLINE void* kigu__array_insert_wrapper(void* array, upt type_size, upt index){ return kigu__array_insert(array, type_size, index); }
#endif //#else //#if COMPILER_FEATURE_CPP


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @array_shorthands


#ifdef KIGU_ARRAY_SHORTHANDS
#  define arrlen(arr)   array_count(arr)
#  define arrspace(arr) array_space(arr)
#  define arrlast(arr) array_last(arr)
#  define arrpush(arr) array_push(arr)
#  define arrpushval(arr,val) array_push_value(arr,val)
#  define arrpop(arr) array_pop(arr)
#  define arrins(arr, idx) array_insert(arr, idx)
#  define arrinsval(arr, idx) array_insert_value(arr, idx, value)
#  define arrrmv(arr, idx) array_remove_unordered(arr, idx)
#  define arrrmvord(arr, idx) array_remove_ordered(arr,idx)
#endif //#ifdef KIGU_ARRAY_SHORTHANDS


EndLinkageC();
//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @array_cpp
#if COMPILER_FEATURE_CPP


#include <initializer_list>


template<typename T> struct scoped_array;


#ifndef KIGU_ARRAY_ALLOCATOR
#  define KIGU_ARRAY_ALLOCATOR stl_allocator
#endif //#ifndef KIGU_ARRAY_ALLOCATOR


// cpp wrapper over kigu array
template<typename T> struct array{
	T* ptr;
	
	// Create an empty array with space for one element
	static FORCE_INLINE array<T> create(Allocator* a = KIGU_ARRAY_ALLOCATOR);
	
	// Create an array with space
	static FORCE_INLINE array<T> create(u32 space, Allocator* a = KIGU_ARRAY_ALLOCATOR);
	
	// Create an array from an initializer list
	static array<T> create(std::initializer_list<T> l, Allocator* a = KIGU_ARRAY_ALLOCATOR);
	
	// Creates an array with 'count' elements zero initialized
	static array<T> create_with_count(u32 count, Allocator* a = KIGU_ARRAY_ALLOCATOR);
	
	// Create an array from an already existing kigu array pointer
	static FORCE_INLINE array<T> from(T* ptr);
	
	// Destroy this array, freeing its memory.
	void destroy();
	
	// Push a new element to the end of the array and return a pointer to it.
	T* push();
	
	// Push a value to the end of the array
	void push(const T& v);
	
	void push(std::initializer_list<T> l);
	
	// Pop 'count' elements from the end of the array returning a copy of the last item.
	T pop(u32 count = 1);
	
	// Inserts 'v' at 'idx'
	void insert(u32 idx, const T& v);
	
	// Removes element at 'idx' and moves all following elements backwards.
	void remove(u32 idx);
	
	// Removes element at 'idx' and fills the empty slot with the last element.
	void remove_unordered(u32 idx);
	
	// Removes all elements
	void clear();
	
	// Resizes the array to hold 'n' elements and 
	// zero inits any new elements. If n is less than
	// the amount of already existing items, does nothing.
	void recount(u32 n);
	
	// returns the number of elements currently in the array
	u32 count();
	
	// returns the amount of space currently allocated for this array
	u32 space();
	
	// returns a pointer to the last element of the array
	T* last();
	
	T& operator[](u32 i);
	T  operator[](u32 i) const;
	
	scoped_array<T> scoped();
};

template<typename T> array<T> array<T>::create(Allocator* a){
	array<T> out = {};
	array_init(out.ptr, 0, a);
	return out;
}

template<typename T> array<T> array<T>::create(u32 space, Allocator* a){
	array<T> out = {};
	array_init(out.ptr, space, a);
	return out;
}

template<typename T> array<T> array<T>::create(std::initializer_list<T> l, Allocator* a){
	array<T> out = {};
	array_init(out.ptr, l.size(), a);
	array_count(out.ptr) = l.size();
	forI(l.size()){
		out.ptr[i] = *(l.begin() + i);
	}
	return out;
}

template<typename T> array<T> array<T>::create_with_count(u32 count, Allocator* a){
	array<T> out = {};
	array_init(out.ptr, count, a);
	array_count(out.ptr) = count;
	return out;
}

template<typename T> array<T> array<T>::from(T* ptr){
	array<T> out = {};
	out.ptr = ptr;
	return out;
}

template<typename T> void array<T>::destroy(){
	array_deinit(ptr);
}

template<typename T> T* array<T>::push(){
	return array_push(ptr);
}

template<typename T> void array<T>::push(const T& v){
	array_push_value(ptr, v);
}

template<typename T> void array<T>::push(std::initializer_list<T> l){
	forI(l.size()){
		push(*(l.begin() + i));
	}
}

template<typename T> T array<T>::pop(u32 count){
	forI(count-1){
		array_pop(ptr);
	}
	T out = *ptr[array_count(ptr)-1];
	array_pop(ptr);
	return out;
}

template<typename T> void array<T>::insert(u32 idx, const T& v){
	array_insert_value(ptr, idx, v);
}

template<typename T> void array<T>::remove(u32 idx){
	array_remove_ordered(ptr, idx);
}

template<typename T> void array<T>::remove_unordered(u32 idx){
	array_remove_unordered(ptr, idx);
}

template<typename T> void array<T>::clear(){
	array_clear(ptr);
}

template<typename T> void array<T>::recount(u32 n){
	u32 old_count = count();
	if(n > old_count){
		forI(n - old_count){
			*array_push(ptr) = {};
		}
	}
}

template<typename T> u32 array<T>::count(){
	return (ptr ? array_count(ptr) : 0);
}

template<typename T> u32 array<T>::space(){
	return array_space(ptr);
}

template<typename T> T& array<T>::operator[](u32 i){
	return ptr[i];
}

template<typename T> T array<T>::operator[](u32 i)const{
	return ptr[i];
}

template<typename T>
struct scoped_array : public array<T>{
	scoped_array(const array<T>& in){
		CopyMemory(this, (void*)&in, sizeof(array<T>));
	}
	
	~scoped_array(){
		this->destroy();
	}
};

template<typename T> scoped_array<T> array<T>::scoped(){
	return scoped_array(*this);
}

// Util for creating an array from a kigu array 
// implicitly determining template type.
// Note that modifying this array will likely separate it from
// the pointer it was created from. It should really only 
// be used to wrap a kigu array for easier reading.
template<typename T> array<T> array_from(T* ptr){
	return array<T>::from(ptr);
}

// Util for pushing a single value to a kigu array.
// Because array's api is through macros, we cannot use struct initializers
// as arguments to array_push_value (because C sucks really bad), so this is here
// to prevent writing the extremely ugly 
//  	*array_push(ptr) = {...};
template<typename T> void array_wrap_and_push(T*& ptr, const T& v){
	array_push_value(ptr, v);
}

// Util for pushing several values to a kigu array.
template<typename T> void array_wrap_and_push(T*& ptr, std::initializer_list<T> l){
	forI(l.size()){
		array_push_value(ptr, *(l.begin()+i));
	}
}

// Util for creating a new array with the same contents as the one provided
template<typename T> array<T> array_copy(T* ptr){
	auto out = array<T>::create_with_count(array_count(ptr), array_allocator(ptr));
	forI(out.count()){
		out[i] = ptr[i];
	}
	return out;
}

template<typename T> void array_init_with_elements(T*& ptr, std::initializer_list<T> l, Allocator* a = KIGU_ARRAY_ALLOCATOR){
	ptr = array<T>::create(l, a).ptr;
}


#endif //#if COMPILER_FEATURE_CPP
//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @array_tests
#ifdef KIGU_UNIT_TESTS


global void kigu__array_unit_tests()
{
	//setup an allcator to use for the tests
	Allocator libc_allocator_{
		[](upt bytes){ return malloc(bytes); },
		Allocator_ChangeMemory_Noop,
		Allocator_ChangeMemory_Noop,
		[](void* p){ free(p); },
		[](void* p, upt bytes){ return realloc(p, bytes); }
	};
	Allocator* libc_allocator = &libc_allocator_;
	
	{//// init ////
		u64* array1 = 0;
		array_init(array1, 32, libc_allocator);
		AssertAlways(array1 != 0);
		AssertAlways(array_header(array1) != 0);
		AssertAlways(array_count(array1) == 0);
		AssertAlways(array_space(array1) == 32);
		AssertAlways(array_allocator(array1) == libc_allocator);
		AssertAlways(array_last(array1) == array1-1);
		
		u64* array2 = array_create(u64, 16, libc_allocator);
		AssertAlways(array2 != 0);
		AssertAlways(array_header(array2) != 0);
		AssertAlways(array_count(array2) == 0);
		AssertAlways(array_space(array2) == 16);
		AssertAlways(array_allocator(array2) == libc_allocator);
		AssertAlways(array_last(array2) == array2-1);
		
		array_deinit(array1);
		array_deinit(array2);
	}
	
	{//// modify ////
		u64* array1 = array_create(u64, 16, libc_allocator);{
			for(u64 i = 0; i < 16; i += 1){
				array1[i] = 0;
			}
			
			u64* array1_backup = array1;
			u64* v0 = array_push(array1);
			AssertAlways(array1 == array1_backup);
			AssertAlways(*v0 == 0);
			*v0 = 5;
			AssertAlways(array1 == v0);
			AssertAlways(array1[0] == 5);
			AssertAlways(array_count(array1) == 1);
			AssertAlways(array_space(array1) == 16);
			AssertAlways(array_last(array1) == array1);
			
			array_pop(array1);
			AssertAlways(array1 == array1_backup);
			AssertAlways(array1 == v0);
			AssertAlways(array1[0] == 0);
			AssertAlways(array_count(array1) == 0);
			AssertAlways(array_space(array1) == 16);
			AssertAlways(array_last(array1) == array1-1);
			
			v0 = array_push_value(array1, 5);
			AssertAlways(array1 == array1_backup);
			AssertAlways(array1 == v0);
			AssertAlways(array1[0] == 5);
			AssertAlways(array_count(array1) == 1);
			AssertAlways(array_space(array1) == 16);
			AssertAlways(array_last(array1) == array1);
			array_pop(array1);
		}array_deinit(array1);
		
		for(u64 i = 0; i < 20000; i += 50){
			array_init(array1, 4, libc_allocator);
			for(u64 j = 0; j < i; j += 1){
				array_push_value(array1, j);
			}
			AssertAlways(array_count(array1) == i);
			array_deinit(array1);
		}
		
		for(u64 i = 0; i < 4; i += 1){
			array_init(array1, 5, libc_allocator);
			
			array_push_value(array1, 1);
			array_push_value(array1, 2);
			array_push_value(array1, 3);
			array_push_value(array1, 4);
			
			u64* v0 = array_insert(array1, i);
			*v0 = 5;
			
			AssertAlways(array1 + i == v0);
			AssertAlways(array1[i] == 5);
			
			if(i < 4){
				AssertAlways(array1[4] == 4);
			}
			
			array_deinit(array1);
		}
		
		for(u64 i = 0; i < 4; i += 1){
			array_init(array1, 5, libc_allocator);
			
			array_push_value(array1, 1);
			array_push_value(array1, 2);
			array_push_value(array1, 3);
			array_push_value(array1, 4);
			
			u64* v0 = array_insert_value(array1, i, 5);
			
			AssertAlways(array1 + i == v0);
			AssertAlways(array1[i] == 5);
			
			if(i < 4){
				AssertAlways(array1[4] == 4);
			}
			
			array_deinit(array1);
		}
		
		for(u64 i = 0; i < 4; i += 1){
			array_init(array1, 4, libc_allocator);
			
			array_push_value(array1, 1);
			array_push_value(array1, 2);
			array_push_value(array1, 3);
			array_push_value(array1, 4);
			
			array_remove_ordered(array1, i);
			
			AssertAlways(array1[3] == 0);
			AssertAlways(array_count(array1) == 3);
			AssertAlways(array_space(array1) == 4);
			
			AssertAlways(array1[0] < array1[1]);
			AssertAlways(array1[1] < array1[2]);
			AssertAlways(array1[2] > array1[3]);
			
			if(i == 0){
				AssertAlways(array1[i] == 2);
			}else if(i < 3){
				AssertAlways(array1[i] - array1[i-1] == 2);
			}
			
			array_deinit(array1);
		}
		
		for(u64 i = 0; i < 4; i += 1){
			array_init(array1, 4, libc_allocator);
			
			array_push_value(array1, 1);
			array_push_value(array1, 2);
			array_push_value(array1, 3);
			array_push_value(array1, 4);
			
			array_remove_unordered(array1, i);
			
			AssertAlways(array1[3] == 0);
			AssertAlways(array_count(array1) == 3);
			AssertAlways(array_space(array1) == 4);
			
			if(i < 3){
				AssertAlways(array1[i] == 4);
			}
			
			array_deinit(array1);
		}
	}
	
	{//// loop ////
		u64* array1 = array_create(u64, 64, libc_allocator);{
			for(u64 i = 0; i < 64; i += 1) array_push_value(array1, 0);
			
			for_array(array1) AssertAlways(*it == 0);
			
			for_array(array1) *it = it - array1;
			
			for(u64 i = 0; i < 64; i += 1) AssertAlways(array1[i] == i);
		}array_deinit(array1);
		
		array_init(array1, 64, libc_allocator);{
			for(u64 i = 0; i < 64; i += 1) array_push_value(array1, 0);
			
			forX_array(p,array1) AssertAlways(*p == 0);
			
			forX_array(p,array1) *p = p - array1;
			
			for(u64 i = 0; i < 64; i += 1) AssertAlways(array1[i] == i);
		}array_deinit(array1);
	}
}


#endif //#if KIGU_UNIT_TESTS
#endif //#ifndef KIGU_ARRAY_H
//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @array_license
/*
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2023 dellevelled, e-sushi
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
