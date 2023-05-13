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
- Memory movement and copying operations don't call C++ constructors or destructor, just pure memory copying.
- The default growth factor is 2, but this can be changed by defining the macro KIGU_ARRAY_GROWTH_FACTOR.
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
  array_init(T* array, upt count, Allocator* allocator) -> void
  array_create(T type, upt count, Allocator* allocator) -> void
  array_deinit(T* array) -> void
@array_modify
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
@array_tests
@array_license

TODO:
- Maybe have some compile-time optimizations on the type_size when moving memory? (via c++ constexpr or C99 generic selection macro)
	We currently iterate by u8 because we don't know type_size at compile-time, but we could iterate less by copying larger chunks
	of memory (32, 64, 128, 256), but before pursuing that though, check if the CPU/compiler doesn't already optimize this away thru
	some other means or if copying larger chunks of memory doesn't just decompose into u8 copies (unlikely on most modern hardware).
- Rewrite the WHAT section to be more readable.
- Unit tests.

REFERENCES:
https://github.com/nothings/stb/blob/master/stb_ds.h  (Sean Barret, nothings, dynamic array)
https://github.com/Jai-Community/Jai-Community-Library/wiki/Getting-Started#for_expansion  (Jai, for loop expansion macro)

LICENSE:
Placed in the public domain and also MIT licensed. See end of file for detailed license information.
*/


#pragma once
#ifndef KIGU_ARRAY_H
#define KIGU_ARRAY_H

#ifndef KIGU_ARRAY_GROWTH_FACTOR
#  define KIGU_ARRAY_GROWTH_FACTOR 2
#endif

#include "common.h"


StartLinkageC();
//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @array_header


typedef struct ArrayHeader{
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


//Assigns `array` to an allocation of `count` contiguous slots allocated using `allocator`
#define array_init(array,count,allocator) ((array) = kigu__array_init_wrapper((array), sizeof(*(array)), (count), (allocator)))
global void* kigu__array_init(upt type_size, upt count, Allocator* allocator){
	ArrayHeader* header = (ArrayHeader*)allocator->reserve(sizeof(ArrayHeader) + count*type_size);
	header->count = 0;
	header->space = count;
	header->allocator = allocator;
	return header+1;
}


//Creates and returns a `T*` to an allocation of `count` contiguous slots allocated using `allocator`
#define array_create(T,count,allocator) ((T*)kigu__array_init(sizeof(T), (count), (allocator)))


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


//Reserves (growing and reassigning `array` if necessary) and returns the next slot at the end of `array`
#define array_push(array) ((array) = kigu__array_push_wrapper((array), sizeof(*(array))), array_last(array))
#define array_push_value(array,value) (*array_push(array) = (value), array_last(array))
global void* kigu__array_push(void* array, upt type_size){
	ArrayHeader* header = array_header(array);
	
	//grow if needed
	if(header->count >= header->space){
		array = kigu__array_grow(array, type_size, header->count*KIGU_ARRAY_GROWTH_FACTOR);
		header = array_header(array);
	}
	
	//reserve next slot
	void* item = (u8*)array + header->count*type_size;
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
		array = kigu__array_grow(array, type_size, header->count*KIGU_ARRAY_GROWTH_FACTOR);
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
	u8* stop = (u8*)array + ((header->count-2) * type_size);
	while(src <= stop){
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
#elif COMPILER_FEATURE_CPP_11
#  define for_array(array) for(auto it = (array); it < ((array) + array_count(array)); ++it)
#  define forX_array(var,array) for(auto var = (array); var < ((array) + array_count(array)); ++var)
#endif //#if COMPILER_FEATURE_TYPEOF


//-////////////////////////////////////////////////////////////////////////////////////////////////
//// @array_wrappers
//// NOTE: C can implicitly cast from void* to T*, but C++ can not so templates are required there, thus we have to wrap functions


#if COMPILER_FEATURE_CPP
EndLinkageC();
template<class T> FORCE_INLINE T* kigu__array_init_wrapper(T* array, upt type_size, upt count, Allocator* allocator){ return (T*)kigu__array_init(type_size, count, allocator); }
template<class T> FORCE_INLINE T* kigu__array_grow_wrapper(T* array, upt type_size, upt count){ return (T*)kigu__array_grow(array, type_size, count); }
template<class T> FORCE_INLINE T* kigu__array_push_wrapper(T* array, upt type_size){ return (T*)kigu__array_push(array, type_size); }
template<class T> FORCE_INLINE T* kigu__array_insert_wrapper(T* array, upt type_size, upt index){ return (T*)kigu__array_insert(array, type_size, index); }
StartLinkageC();
#else
FORCE_INLINE void* kigu__array_init_wrapper(void* array, upt type_size, upt count, Allocator* allocator){ return  kigu__array_init(type_size, count, allocator); }
FORCE_INLINE void* kigu__array_grow_wrapper(void* array, upt type_size, upt count){ return kigu__array_grow(array, type_size, count); }
FORCE_INLINE void* kigu__array_push_wrapper(void* array, upt type_size){ return kigu__array_push(array, type_size); }
FORCE_INLINE void* kigu__array_insert_wrapper(void* array, upt type_size, upt index){ return kigu__array_insert(array, type_size, index); }
#endif //#if COMPILER_FEATURE_CPP


EndLinkageC();
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
		
	}
	
	{//// modify ////
		
	}
	
	{//// loop ////
		
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