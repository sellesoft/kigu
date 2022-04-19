#include <typeinfo>
#include <cstdio>
#include <chrono>

#define TEST_KIGU_TIMER_START(name) std::chrono::time_point<std::chrono::high_resolution_clock> name = std::chrono::high_resolution_clock::now()
#define TEST_KIGU_TIMER_RESET(name) name = std::chrono::high_resolution_clock::now()
#define TEST_KIGU_TIMER_END(name) std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - name).count()

#define TEST_KIGU_PRINT_VERBOSE true
#if TEST_KIGU_PRINT_VERBOSE
#define print_verbose(fmt,...) printf(fmt,__VA_ARGS__)
#else
#  define print_verbose(fmt,...) (void)0
#endif

#define KIGU_ARRAY_GROWTH_FACTOR 2
#define KIGU_ARRAY_SPACE_ALIGNMENT 4
#include "array.h"
local void TEST_kigu_array(){
	//// constructors ////
	array<int> array1;
	AssertAlways(typeid(array1.data) == typeid(int*));
	AssertAlways(array1.count == 0);
	AssertAlways(array1.space == 0);
	AssertAlways(array1.data == 0);
	AssertAlways(array1.first == 0);
	AssertAlways(array1.last == 0);
	AssertAlways(array1.iter == 0);
	
	persist int destruct_sum = 0;
	struct TestType{
		int value;
		TestType(int a){value = a*2;}
		~TestType(){destruct_sum++;}
		bool operator==(const TestType& rhs) const{return value==rhs.value;}
	};
	array<TestType> array2 = array<TestType>();
	AssertAlways(typeid(array2.data) == typeid(TestType*));
	AssertAlways(array2.count == 0);
	AssertAlways(array2.space == 0);
	AssertAlways(array2.data == 0);
	AssertAlways(array2.first == 0);
	AssertAlways(array2.last == 0);
	AssertAlways(array2.iter == 0);
	
	Allocator test_allocator{malloc,Allocator_ChangeMemory_Noop,Allocator_ChangeMemory_Noop,free,realloc};
	array<TestType> array3(&test_allocator);
	AssertAlways(array3.count == 0);
	AssertAlways(array3.space == 0);
	AssertAlways(array3.data == 0);
	AssertAlways(array3.first == 0);
	AssertAlways(array3.last == 0);
	AssertAlways(array3.iter == 0);
	AssertAlways(array3.allocator == &test_allocator);
	AssertAlways(array3.allocator->reserve == malloc);
	
	array<TestType> array4(5);
	AssertAlways(array4.count == 0);
	AssertAlways(array4.space == 8);
	AssertAlways(array4.data != 0);
	AssertAlways(array4.first == 0);
	AssertAlways(array4.last == 0);
	AssertAlways(array4.iter == 0);
	AssertAlways(array4.data[0].value == 0 && array4.data[1].value == 0 && array4.data[2].value == 0 && array4.data[3].value == 0);
	AssertAlways(array4.data[4].value == 0 && array4.data[5].value == 0 && array4.data[6].value == 0 && array4.data[7].value == 0);
	
	array<TestType> array5({TestType(1), TestType(2), TestType(3)});
	AssertAlways(destruct_sum == 3);
	AssertAlways(array5.count == 3);
	AssertAlways(array5.space == 4);
	AssertAlways(array5.data != 0);
	AssertAlways(array5.first == array5.data);
	AssertAlways(array5.last == array5.data+2);
	AssertAlways(array5.iter == array5.data);
	AssertAlways(array5.data[0].value == 2);
	AssertAlways(array5.data[1].value == 4);
	AssertAlways(array5.data[2].value == 6);
	AssertAlways(PointerDifference(&array5.data[1], &array5.data[0]) == sizeof(TestType));
	AssertAlways(array5.data[3].value == 0);
	
	array<TestType> array6(array5);
	AssertAlways(array6.count == 3);
	AssertAlways(array6.space == 4);
	AssertAlways(array6.data != 0);
	AssertAlways(array6.first == array6.data);
	AssertAlways(array6.last == array6.data+2);
	AssertAlways(array6.iter == array6.data);
	AssertAlways(array6.data[0].value == 2);
	AssertAlways(array6.data[1].value == 4);
	AssertAlways(array6.data[2].value == 6);
	
	array<TestType> array7(array5.data, 2);
	AssertAlways(array7.count == 2);
	AssertAlways(array7.space == 4);
	AssertAlways(array7.data != 0);
	AssertAlways(array7.first == array7.data);
	AssertAlways(array7.last == array7.data+1);
	AssertAlways(array7.iter == array7.data);
	AssertAlways(array7.data[0].value == 2);
	AssertAlways(array7.data[1].value == 4);
	AssertAlways(array7.data[2].value == 0);
	
	array7.~array();
	AssertAlways(destruct_sum == 5);
	array7.data = 0;
	
	//// operators ////
	array3 = array5;
	AssertAlways(destruct_sum == 5);
	AssertAlways(array3.count == 3);
	AssertAlways(array3.space == 4);
	AssertAlways(array3.data != 0 && array3.data != array5.data);
	AssertAlways(array3.first == array3.data);
	AssertAlways(array3.last == array3.data+2);
	AssertAlways(array3.iter == array3.data);
	AssertAlways(array3.allocator == KIGU_ARRAY_ALLOCATOR);
	AssertAlways(array3.data[0].value == 2);
	AssertAlways(array3.data[1].value == 4);
	AssertAlways(array3.data[2].value == 6);
	
	array3[0] = TestType(5);
	AssertAlways(destruct_sum == 6);
	AssertAlways(array3.data[0].value == 10);
	AssertAlways(array3.data[1].value == 4);
	AssertAlways(array3.data[2].value == 6);
	
	AssertAlways(array3[0].value == 10);
	AssertAlways(array3[1].value == 4);
	AssertAlways(array3[2].value == 6);
	
	//// functions ////
	array2.add(TestType(1));
	AssertAlways(destruct_sum == 7);
	AssertAlways(array2.count == 1);
	AssertAlways(array2.space == 4);
	AssertAlways(array2.data != 0);
	AssertAlways(array2.first == array2.data);
	AssertAlways(array2.last == array2.data);
	AssertAlways(array2.iter == array2.data);
	AssertAlways(array2[0].value == 2);
	AssertAlways(array2.data[1].value == 0);
	
	array2.add(TestType(2));
	array2.add(TestType(3));
	array2.add(TestType(4));
	AssertAlways(destruct_sum == 10);
	AssertAlways(array2.count == 4);
	AssertAlways(array2.space == 4);
	AssertAlways(array2.data != 0);
	AssertAlways(array2.last == array2.data+3);
	AssertAlways(array2[0].value == 2);
	AssertAlways(array2[1].value == 4);
	AssertAlways(array2[2].value == 6);
	AssertAlways(array2[3].value == 8);
	
	array2.iter++;
	array2.add(TestType(5));
	AssertAlways(destruct_sum == 11);
	AssertAlways(array2.count == 5);
	AssertAlways(array2.space == 8);
	AssertAlways(array2.data != 0);
	AssertAlways(array2.first == array2.data);
	AssertAlways(array2.last == array2.data+4);
	AssertAlways(array2.iter == array2.data+1);
	AssertAlways(array2[0].value == 2);
	AssertAlways(array2[1].value == 4);
	AssertAlways(array2[2].value == 6);
	AssertAlways(array2[3].value == 8);
	AssertAlways(array2[4].value == 10);
	
	array2.add_array(array6);
	AssertAlways(array2.count == 8);
	AssertAlways(array2.space == 8);
	AssertAlways(array2.last == array2.data+7);
	AssertAlways(array2[0].value == 2);
	AssertAlways(array2[1].value == 4);
	AssertAlways(array2[2].value == 6);
	AssertAlways(array2[3].value == 8);
	AssertAlways(array2[4].value == 10);
	AssertAlways(array2[5].value == 2);
	AssertAlways(array2[6].value == 4);
	AssertAlways(array2[7].value == 6);
	
	array<TestType> array8;
	array8.emplace(10);
	AssertAlways(destruct_sum == 12);
	AssertAlways(array8.count == 1);
	AssertAlways(array8.space == 4);
	AssertAlways(array8.first == array8.data);
	AssertAlways(array8.last == array8.data);
	AssertAlways(array8.iter == array8.data);
	AssertAlways(array8[0].value == 20);
	AssertAlways(array8.data[1].value == 0);
	
	array2.emplace(1);
	AssertAlways(destruct_sum == 13);
	AssertAlways(array2.count == 9);
	AssertAlways(array2.space == 16);
	AssertAlways(array2.first == array2.data);
	AssertAlways(array2.last == array2.data+8);
	AssertAlways(array2.iter == array2.data+1);
	AssertAlways(array2[0].value == 2);
	AssertAlways(array2[1].value == 4);
	AssertAlways(array2[8].value == 2);
	AssertAlways(array2.data[9].value == 0);
	
	array2.emplace(5);
	AssertAlways(destruct_sum == 14);
	AssertAlways(array2.count == 10);
	AssertAlways(array2.space == 16);
	AssertAlways(array2.last == array2.data+9);
	AssertAlways(array2[0].value == 2);
	AssertAlways(array2[1].value == 4);
	AssertAlways(array2[8].value == 2);
	AssertAlways(array2[9].value == 10);
	AssertAlways(array2.data[10].value == 0);
	
	array<TestType> array9;
	array9.insert(TestType(1), 0);
	AssertAlways(destruct_sum == 15);
	AssertAlways(array9.count == 1);
	AssertAlways(array9.space == 4);
	AssertAlways(array9.first == array9.data);
	AssertAlways(array9.last == array9.data);
	AssertAlways(array9.iter == array9.data);
	AssertAlways(array9[0].value == 2);
	AssertAlways(array9.data[1].value == 0);
	
	array9.insert(TestType(3), 1);
	AssertAlways(destruct_sum == 16);
	AssertAlways(array9.count == 2);
	AssertAlways(array9.space == 4);
	AssertAlways(array9.last == array9.data+1);
	AssertAlways(array9[0].value == 2);
	AssertAlways(array9[1].value == 6);
	AssertAlways(array9.data[2].value == 0);
	
	array9.insert(TestType(2), 1);
	AssertAlways(destruct_sum == 17);
	AssertAlways(array9.count == 3);
	AssertAlways(array9.space == 4);
	AssertAlways(array9.last == array9.data+2);
	AssertAlways(array9[0].value == 2);
	AssertAlways(array9[1].value == 4);
	AssertAlways(array9[2].value == 6);
	AssertAlways(array9.data[3].value == 0);
	
	array9.emplace(4);
	array9.insert(TestType(5), 4);
	AssertAlways(destruct_sum == 19);
	AssertAlways(array9.count == 5);
	AssertAlways(array9.space == 8);
	AssertAlways(array9.first == array9.data);
	AssertAlways(array9.last == array9.data+4);
	AssertAlways(array9.iter == array9.data);
	AssertAlways(array9[0].value == 2);
	AssertAlways(array9[1].value == 4);
	AssertAlways(array9[2].value == 6);
	AssertAlways(array9[3].value == 8);
	AssertAlways(array9[4].value == 10);
	AssertAlways(array9.data[5].value == 0);
	
	array9.pop();
	AssertAlways(destruct_sum == 20);
	AssertAlways(array9.count == 4);
	AssertAlways(array9.space == 8);
	AssertAlways(array9.last == array9.data+3);
	AssertAlways(array9[0].value == 2);
	AssertAlways(array9[1].value == 4);
	AssertAlways(array9[2].value == 6);
	AssertAlways(array9[3].value == 8);
	AssertAlways(array9.data[4].value == 0);
	AssertAlways(array9.data[5].value == 0);
	
	array9.pop(2);
	AssertAlways(destruct_sum == 22);
	AssertAlways(array9.count == 2);
	AssertAlways(array9.space == 8);
	AssertAlways(array9.last == array9.data+1);
	AssertAlways(array9[0].value == 2);
	AssertAlways(array9[1].value == 4);
	AssertAlways(array9.data[2].value == 0);
	AssertAlways(array9.data[3].value == 0);
	AssertAlways(array9.data[4].value == 0);
	AssertAlways(array9.data[5].value == 0);
	
	array9.pop(2);
	AssertAlways(destruct_sum == 24);
	AssertAlways(array9.count == 0);
	AssertAlways(array9.space == 8);
	AssertAlways(array9.first == 0);
	AssertAlways(array9.last == 0);
	AssertAlways(array9.iter == 0);
	AssertAlways(array9.data[0].value == 0);
	AssertAlways(array9.data[1].value == 0);
	
	array9.emplace(1);
	array9.emplace(2);
	array9.remove(0);
	AssertAlways(destruct_sum == 27);
	AssertAlways(array9.count == 1);
	AssertAlways(array9.space == 8);
	AssertAlways(array9.last == array9.data);
	AssertAlways(array9[0].value == 4);
	AssertAlways(array9.data[1].value == 0);
	
	array9.remove(0);
	AssertAlways(destruct_sum == 28);
	AssertAlways(array9.count == 0);
	AssertAlways(array9.space == 8);
	AssertAlways(array9.first == 0);
	AssertAlways(array9.last == 0);
	AssertAlways(array9.iter == 0);
	AssertAlways(array9.data[0].value == 0);
	
	array2.clear();
	AssertAlways(destruct_sum == 38);
	AssertAlways(array2.count == 0);
	AssertAlways(array2.space == 16);
	AssertAlways(array2.first == array2.data);
	AssertAlways(array2.last == 0);
	AssertAlways(array2.iter == array2.data);
	AssertAlways(array2.data[0].value == 0);
	AssertAlways(array2.data[10].value == 0);
	
	array5.iter++;
	array5.resize(6);
	AssertAlways(array5.count == 6);
	AssertAlways(array5.space == 6);
	AssertAlways(array5.data != 0);
	AssertAlways(array5.first == array5.data);
	AssertAlways(array5.last == array5.data+5);
	AssertAlways(array5.iter == array5.data+1);
	AssertAlways(array5[0].value == 2);
	AssertAlways(array5[1].value == 4);
	AssertAlways(array5[2].value == 6);
	AssertAlways(array5.data[3].value == 0);
	AssertAlways(array5.data[5].value == 0);
	
	array5.resize(2);
	AssertAlways(array5.count == 2);
	AssertAlways(array5.space == 2);
	AssertAlways(array5.data != 0);
	AssertAlways(array5.first == array5.data);
	AssertAlways(array5.last == array5.data+1);
	AssertAlways(array5.iter == array5.data+1);
	AssertAlways(array5[0].value == 2);
	AssertAlways(array5[1].value == 4);
	
	array5.reserve(3);
	AssertAlways(array5.count == 2);
	AssertAlways(array5.space == 4);
	AssertAlways(array5.data != 0);
	AssertAlways(array5.first == array5.data);
	AssertAlways(array5.last == array5.data+1);
	AssertAlways(array5.iter == array5.data+1);
	AssertAlways(array5[0].value == 2);
	AssertAlways(array5[1].value == 4);
	
	array5.reserve(3);
	AssertAlways(array5.count == 2);
	AssertAlways(array5.space == 4);
	AssertAlways(array5.data != 0);
	AssertAlways(array5.first == array5.data);
	AssertAlways(array5.last == array5.data+1);
	AssertAlways(array5.iter == array5.data+1);
	AssertAlways(array5[0].value == 2);
	AssertAlways(array5[1].value == 4);
	
	array5.swap(0,1);
	AssertAlways(array5[0].value == 4);
	AssertAlways(array5[1].value == 2);
	
	array5.swap(0,1);
	AssertAlways(array5[0].value == 2);
	AssertAlways(array5[1].value == 4);
	
	AssertAlways(array5.has(TestType(2)));
	AssertAlways(!array5.has(TestType(4)));
	
	array5.at(0) = TestType(5);
	AssertAlways(array3[0].value == 10);
	AssertAlways(array3[1].value == 4);
	
	AssertAlways(array3.at(0).value == 10);
	AssertAlways(array3.at(1).value == 4);
	
	//TODO(sushi) setup array special pointer testing
	
	printf("[KIGU-TEST] PASSED: array\n");
}

#include "array_utils.h"
local void TEST_kigu_array_utils(){
	TEST_KIGU_TIMER_START(timer);
	
	//bubble sort
	srand(time(0));
	array<s32> array1(1024);
	forI(1024) array1.add(rand() % 1024);
	TEST_KIGU_TIMER_RESET(timer);
	bubble_sort(array1, [](s32 a, s32 b){return a < b;});
	print_verbose("[KIGU-TEST] bubble_sort() took %fms\n", TEST_KIGU_TIMER_END(timer));
	forI(1024){ if(i){ AssertAlways(array1[i] <= array1[i-1]); } }
	print_verbose("[KIGU-TEST] PASSED: array_utils/bubble_sort\n");
	
	srand(time(0));
	array1.clear();
	forI(1024) array1.add(rand() % 1024);
	TEST_KIGU_TIMER_RESET(timer);
	bubble_sort_low_to_high(array1);
	print_verbose("[KIGU-TEST] bubble_sort_low_to_high() took %fms\n", TEST_KIGU_TIMER_END(timer));
	forI(1024){ if(i){ AssertAlways(array1[i] >= array1[i-1]); } }
	print_verbose("[KIGU-TEST] PASSED: array_utils/bubble_sort_low_to_high\n");
	
	srand(time(0));
	array1.clear();
	forI(1024) array1.add(rand() % 1024);
	TEST_KIGU_TIMER_RESET(timer);
	bubble_sort_high_to_low(array1);
	print_verbose("[KIGU-TEST] bubble_sort_high_to_low() took %fms\n", TEST_KIGU_TIMER_END(timer));
	forI(1024){ if(i){ AssertAlways(array1[i] <= array1[i-1]); } }
	print_verbose("[KIGU-TEST] PASSED: array_utils/bubble_sort_high_to_low\n");
	
	//reverse
	TEST_KIGU_TIMER_RESET(timer);
	reverse(array1);
	print_verbose("[KIGU-TEST] reverse() took %fms\n", TEST_KIGU_TIMER_END(timer));
	forI(1024){ if(i){ AssertAlways(array1[i] >= array1[i-1]); } }
	print_verbose("[KIGU-TEST] PASSED: array_utils/reverse\n");
	
	//binary search
	//TODO test binary search comparator
	
	array1[0] = MAX_S32;
	array1[84] = MIN_S32;
	array1[123] = 0;
	bubble_sort_low_to_high(array1);
	AssertAlways(binary_search_low_to_high(array1, MAX_S32) != -1);
	AssertAlways(binary_search_low_to_high(array1, MIN_S32) != -1);
	AssertAlways(binary_search_low_to_high(array1, 0) != -1);
	print_verbose("[KIGU-TEST] PASSED: array_utils/binary_search_low_to_high\n");
	
	printf("[KIGU-TEST] PASSED: array_utils\n");
}

#include "carray.h"
local void TEST_kigu_carray(){
	int* arr0 = (int*)calloc(1, 16*sizeof(int));
	defer{ free(arr0); };
	forI(16){ arr0[i] = 1 << i; }
	
	carray<int> arr1{arr0, 16};
	AssertAlways(arr1.data == arr0);
	AssertAlways(arr1.count == 16);
	AssertAlways(arr1);
	AssertAlways(arr1[0] == 1 << 0);
	AssertAlways(arr1[1] == 1 << 1);
	AssertAlways(arr1[4] == 1 << 4);
	AssertAlways(arr1[15] == 1 << 15);
	AssertAlways(arr1.at(8) == arr0+8);
	forE(arr1){ AssertAlways(*it == 1 << (it - it_begin)); }
	
	array_pop(arr1, 2);
	AssertAlways(arr1.count == 16);
	arr1.count = 14;
	AssertAlways(arr1);
	AssertAlways(arr1[0] == 1 << 0);
	AssertAlways(arr1[1] == 1 << 1);
	AssertAlways(arr1[4] == 1 << 4);
	AssertAlways(arr1[13] == 1 << 13);
	forE(arr1){ AssertAlways(*it == 1 << (it - it_begin)); }
	
	array_remove_unordered(arr1, 4);
	AssertAlways(arr1.data == arr0);
	AssertAlways(arr1.count == 14);
	arr1.count = 13;
	AssertAlways(arr1);
	AssertAlways(arr1[0] == 1 << 0);
	AssertAlways(arr1[1] == 1 << 1);
	AssertAlways(arr1[4] == 1 << 13);
	AssertAlways(arr1[12] == 1 << 12);
	
	array_remove_ordered(arr1, 4);
	AssertAlways(arr1.data == arr0);
	AssertAlways(arr1.count == 13);
	arr1.count = 12;
	AssertAlways(arr1);
	AssertAlways(arr1[0] == 1 << 0);
	AssertAlways(arr1[1] == 1 << 1);
	AssertAlways(arr1[4] == 1 << 5);
	AssertAlways(arr1[11] == 1 << 12);
	
	printf("[KIGU-TEST] PASSED: carray\n");
}

#include "color.h"
local void TEST_kigu_color(){
	//// constructors ////
	color color1;
	AssertAlways(color1.r == 000 && color1.g == 000 && color1.b == 000 && color1.a == 000);
	AssertAlways(color1.rgba == 0);
	
	color color2(0, 255, 0, 255);
	AssertAlways(color2.r == 000 && color2.g == 255 && color2.b == 000 && color2.a == 255);
	AssertAlways(color2.rgba == 0xFF00FF00);
	
	color color3(0xFFFF0000);
	AssertAlways(color3.r == 255 && color3.g == 255 && color3.b == 000 && color3.a == 000);
	AssertAlways(color3.rgba == 0x0000FFFF);
	
	color color4 = Color_Red;
	AssertAlways(color4.r == 255 && color4.g == 000 && color4.b == 000 && color4.a == 255);
	AssertAlways(color4.rgba == 0xFF0000FF);
	
	//// macros ////
	AssertAlways((Color_Blue.rgba & COLORU32_RMASK) == 0);
	AssertAlways((Color_Blue.rgba & COLORU32_GMASK) == 0);
	AssertAlways((Color_Blue.rgba & COLORU32_BMASK) == COLORU32_BMASK);
	AssertAlways((Color_Blue.rgba & COLORU32_AMASK) == COLORU32_AMASK);
	
	AssertAlways((Color_Green.rgba & COLORU32_RMASK) == 0);
	AssertAlways((Color_Green.rgba & COLORU32_GMASK) == COLORU32_GMASK);
	AssertAlways((Color_Green.rgba & COLORU32_BMASK) == 0);
	AssertAlways((Color_Green.rgba & COLORU32_AMASK) == COLORU32_AMASK);
	
	AssertAlways(Color_Green == PackColorU32(0,255,0,255));
	AssertAlways(color(0,255,0,255) == PackColorU32(0,255,0,255));
	
	//// operators ////
	color color5(32, 32, 32, 128);
	color5 *= 2;
	AssertAlways(color5.r == 64 && color5.g == 64 && color5.b == 64 && color5.a == 128);
	
	AssertAlways(color(255, 0, 0, 255) == Color_Red);
	AssertAlways(Color_Blue == 0x0000FFFF);
	
	AssertAlways(color5*color(2,0,0,2) == color(128,0,0,128));
	
	AssertAlways(color5*2 == color(128,128,128,128));
	
	AssertAlways(color5/2 == color(32,32,32,128));
	
	//// functions ////
	color color6 = color::FloatsToColor(0,1,0,1);
	AssertAlways(color6 == Color_Green);
	color6 = color::FloatsToColor(0,.5f,0,1);
	AssertAlways(color6 == Color_Green/2);
	
	f32 test_arr1[4] = {1.f, 1.f, 1.f, 1.f};
	color::FillFloat3FromU32(test_arr1, Color_Blue.rgba);
	AssertAlways(test_arr1[0] == 0.f);
	AssertAlways(test_arr1[1] == 0.f);
	AssertAlways(test_arr1[2] == 1.f);
	AssertAlways(test_arr1[3] == 1.f);
	
	f32 test_arr2[4] = {0, 0, 1.f, 0};
	color::FillFloat4FromU32(test_arr2, color(128,128,0,128).rgba);
	AssertAlways(abs(test_arr2[0] - .5f) <= .01f);
	AssertAlways(abs(test_arr2[1] - .5f) <= .01f);
	AssertAlways(test_arr2[2] == 0.f);
	AssertAlways(abs(test_arr2[3] - .5f) <= .01f);
	
	printf("[KIGU-TEST] PASSED: color\n");
}

#include "cstring.h"
local void TEST_kigu_cstring(){
	printf("[KIGU-TEST] TODO:   cstring\n");
}

#include "hash.h"
local void TEST_kigu_hash(){
	printf("[KIGU-TEST] TODO:   hash\n");
}

#include "map.h"
local void TEST_kigu_map(){
	printf("[KIGU-TEST] TODO:   map\n");
}

#include "optional.h"
local void TEST_kigu_optional(){
	printf("[KIGU-TEST] TODO:   optional\n");
}

#include "ring_array.h"
local void TEST_kigu_ring_array(){
	printf("[KIGU-TEST] TODO:   ring_array\n");
}

#include "string.h"
local void TEST_kigu_string(){
	
	
	//#define teststr "ABCDEFG"
#define teststr "While the butterflies form a monophyletic group, the moths, comprising the rest of the Lepidoptera, do not. Many attempts have been made to group the superfamilies of the Lepidoptera into natural groups, most of which fail because one of the two groups is not monophyletic: Microlepidoptera and Macrolepidoptera, Heterocera and Rhopalocera, Jugatae and Frenatae, Monotrysia and Ditrysia.\nAlthough the rules for distinguishing moths from butterflies are not well established, one very good guiding principle is that butterflies have thin antennae and (with the exception of the family Hedylidae) have small balls or clubs at the end of their antennae.Moth antennae are usually feathery with no ball on the end.The divisions are named by this principle: \"club-antennae\" (Rhopalocera) or \"varied-antennae\" (Heterocera).Lepidoptera differs between butterflies and other organisms due to evolving a special characteristic of having the tube - like proboscis in the Middle Triassic which allowed them to acquire nectar from flowering plants.[3] "
	const u32 len = strlen(teststr);
	
	//// constructors ////
	{//empty constructor does not allocate or set any vars
		string str;
		AssertAlways(!str.str);
		AssertAlways(!str.count);
		AssertAlways(!str.space);
	}
	
	{//const char* constructor
		string str(teststr);
		AssertAlways(!memcmp(str.str, teststr, len));
		AssertAlways(str.count == len);
		AssertAlways(str.space == RoundUpTo(len + 1, 4));
	}
	
	{//const char* with count constructor
		string str(teststr, len);
		AssertAlways(!memcmp(str.str, teststr, len));
		AssertAlways(str.count == len);
		AssertAlways(str.space == RoundUpTo(len + 1, 4));
	}
	
	{//copy constructor
		string str(teststr);
		string str2(str);
		AssertAlways(!memcmp(str2.str, teststr, len));
		AssertAlways(str2.count == len);
		AssertAlways(str2.space == RoundUpTo(len + 1, 4));
	}
	
	
	//// operators ////
	
	
	{//operator[](u32)
		string str(teststr);
		for (u32 i = 0; i < len; i++)
			AssertAlways(str[i] == teststr[i]);
	}
	
	{//operator[] const
		const string str(teststr);
		for (u32 i = 0; i < len; i++)
			AssertAlways(str[i] == teststr[i]);
	}
	
	{//operator= const char*
		string str = teststr;
		AssertAlways(!memcmp(str.str, teststr, len));
		AssertAlways(str.count == len);
		AssertAlways(str.space == RoundUpTo(len + 1, 4));
	}
	
	{//operator= string
		string str = teststr;
		string str2 = str;
		AssertAlways(!memcmp(str2.str, teststr, len));
		AssertAlways(str2.count == len);
		AssertAlways(str2.space == RoundUpTo(len + 1, 4));
	}
	
	{//operator+= const char*
		string str = teststr;
		str += teststr;
		AssertAlways(!memcmp(str.str, teststr teststr, len * 2));
		AssertAlways(str.count == len * 2);
		AssertAlways(str.space == RoundUpTo(len * 2 + 1, 4));
	}
	
	{//operator+= string
		string str = teststr;
		string str2 = teststr;
		str += str2;
		AssertAlways(!memcmp(str.str, teststr teststr, len * 2));
		AssertAlways(str.count == len * 2);
		AssertAlways(str.space == RoundUpTo(len * 2 + 1 , 4));
	}
	
	{//operator--
		string str = teststr;
		str--;
		for (u32 i = 0; i < len - 1; i++)
			AssertAlways(str[i] == teststr[i]);
		AssertAlways(str.count == len - 1);
		AssertAlways(str.space == RoundUpTo(len + 1, 4));
	}
	
	{//operator+ const char*
		string str = teststr;
		string str2 = str + teststr;
		AssertAlways(!memcmp(str2.str, teststr teststr, len * 2));
		AssertAlways(str2.count == len * 2);
		AssertAlways(str2.space == RoundUpTo(len * 2 + 1, 4));
	}
	
	{//operator+ string
		string str = teststr;
		string str2 = teststr;
		string str3 = str + str2;
		AssertAlways(!memcmp(str3.str, teststr teststr, len * 2));
		AssertAlways(str3.count == len * 2);
		AssertAlways(str3.space == RoundUpTo(len * 2 + 1, 4));
	}
	
	{//operator== string
		string str = teststr;
		string str2 = teststr;
		AssertAlways(str == str2);
	}
	
	{//operator!= string
		string str = teststr;
		string str2 = teststr "a";
		AssertAlways(str != str2);
	}
	
	{//operator== const char*
		string str = teststr;
		AssertAlways(str == teststr);
	}
	
	{//operator!= const char*
		string str = teststr;
		AssertAlways(str != teststr"a");
	}
	
	{//friend operator+ const char* and string
		string str = "yep";
		string str2 = teststr + str;
		AssertAlways(!memcmp(str2.str, teststr "yep", len + 3));
		AssertAlways(str2.count == len+3);
		AssertAlways(str2.space == RoundUpTo(len + 3 + 1, 4));
	}
	
	{//operator bool()
		string str;
		AssertAlways(!str);
		str = teststr;
		AssertAlways(str);
	}
	
	
	//// functions ////
	
	
	{//reserve
		string str;
		str.reserve(len);
		AssertAlways(str.str);
		AssertAlways(!str.count);
		AssertAlways(str.space == RoundUpTo(len + 1, 4));
		
		str.reserve(len - 1);
		AssertAlways(str.str);
		AssertAlways(!str.count);
		AssertAlways(str.space == RoundUpTo(len + 1, 4));
		
		//forI(len) str[i] = teststr[i];
		//AssertAlways(!memcmp(str.str, teststr, len));
		//AssertAlways(str.count == len);
		//AssertAlways(str.space == RoundUpTo(len + 1, 4));
	}
	
	{//clear
		string str = teststr;
		str.clear();
		AssertAlways(!str.str);
		AssertAlways(!str.count);
		AssertAlways(!str.space);
	}
	
	{//erase
		string str;
		for (u32 i = 0; i < len; i++) {
			str = teststr;
			str.erase(i);
			for (u32 o = 0; o < len; o++) {
				if (o != i)
					AssertAlways(str[(o >= i ? o - 1 : o)] == teststr[o]);
			}
		}
		
		str = teststr;
		while (str) str.erase(0);
		AssertAlways(!str.str);
		AssertAlways(!str.count);
		AssertAlways(!str.space);
		
		str = teststr;
		srand(time(0));
		while (str) str.erase(rand() % str.count); 
		AssertAlways(!str.str);
		AssertAlways(!str.count);
		AssertAlways(!str.space);
	}
	
	{//insert
		string str;
		for (u32 i = 0; i < len; i++) {
			str = teststr;
			str.insert('A', i);
			for (u32 o = 0; o < len+1; o++) {
				if (o != i)
					AssertAlways(str[o] == teststr[(o >= i ? o - 1 : o)]);
			}
			
		}
		
	}
	
	{//at
		string str = teststr;
		forI(str.count) AssertAlways(str.at(i) == str[i]);
	}
	
	{//substr
		string str = teststr;
		
		forI(1000) {
			s32 rand1 = rand() % str.count,
			rand2 = rand() % str.count;
			string str2 = str.substr(Min(rand1, rand2), Max(rand1, rand2));
			AssertAlways(!memcmp(str.str + Min(rand1, rand2), str2.str, (upt)fabs(rand1 - rand2)));
		}
		
		
	}
	
	{//findFirstStr
		string str = teststr;
		
		
	}
	
	//TODO(sushi) write tests for the remaining functions
	//TODO write tests for wstring
	
	printf("[KIGU-TEST] PASSED: string\n");
}

#include "string_utils.h"
local void TEST_kigu_string_utils(){
	printf("[KIGU-TEST] TODO:   string_utils\n");
}

#include "pair.h"
local void TEST_kigu_pair(){
	printf("[KIGU-TEST] TODO:   pair\n");
}

#include <io.h>
#include <fcntl.h>
#include "unicode.h"
local void TEST_kigu_unicode(){
#define TEST_KIGU_UNICODE_LITERAL "a b c d Д Е Ж З И Й К Л У Ф Х ≤ ≥ ♪ ♫ ╞ ╟ ╠ ╡ ╢ ╣      🍌"
	str8  test8  = str8_lit (TEST_KIGU_UNICODE_LITERAL);
	str16 test16 = str16_lit(TEST_KIGU_UNICODE_LITERAL);
	str32 test32 = str32_lit(TEST_KIGU_UNICODE_LITERAL);
	wchar_t testw[] = GLUE(L,TEST_KIGU_UNICODE_LITERAL);
	
	{//literals
		u32 codepoint = U'🍌'; //U+1F34C
		AssertAlways(codepoint == 0x1F34C);
		
		AssertAlways(test8.count  == 90);
		AssertAlways(test16.count == 57);
		AssertAlways(test32.count == 56);
#if COMPILER_CL
		AssertAlways(ArrayCount(testw) == 58);
#elif COMPILER_CLANG || COMPILER_GCC
		AssertAlways(ArrayCount(testw) == 57);
#else
#  error "unhandled compiler"
#endif
		
		print_verbose("[KIGU-TEST] PASSED: unicode/literals\n");
	}
	
	//TODO test invalid decoding
	{//decoding
		DecodedCodepoint cp;
		
		cp = decoded_codepoint_from_utf8((u8*)u8"a", 4);
		AssertAlways(cp.codepoint == 0x61);
		AssertAlways(cp.advance == 1);
		cp = decoded_codepoint_from_utf8((u8*)u8"Д", 4);
		AssertAlways(cp.codepoint == 0x414);
		AssertAlways(cp.advance == 2);
		cp = decoded_codepoint_from_utf8((u8*)u8"Ω", 4);
		AssertAlways(cp.codepoint == 0x2126);
		AssertAlways(cp.advance == 3);
		cp = decoded_codepoint_from_utf8((u8*)u8"🍌", 4);
		AssertAlways(cp.codepoint == 0x1F34C);
		AssertAlways(cp.advance == 4);
		
		cp = decoded_codepoint_from_utf16((u16*)u"a", 2);
		AssertAlways(cp.codepoint == 0x61);
		AssertAlways(cp.advance == 1);
		cp = decoded_codepoint_from_utf16((u16*)u"Д", 2);
		AssertAlways(cp.codepoint == 0x414);
		AssertAlways(cp.advance == 1);
		cp = decoded_codepoint_from_utf16((u16*)u"Ω", 2);
		AssertAlways(cp.codepoint == 0x2126);
		AssertAlways(cp.advance == 1);
		cp = decoded_codepoint_from_utf16((u16*)u"🍌", 2);
		AssertAlways(cp.codepoint == 0x1F34C);
		AssertAlways(cp.advance == 2);
		
		cp = decoded_codepoint_from_wchar(L"a", 2);
		AssertAlways(cp.codepoint == 0x61);
		AssertAlways(cp.advance == 1);
		cp = decoded_codepoint_from_wchar(L"Д", 2);
		AssertAlways(cp.codepoint == 0x414);
		AssertAlways(cp.advance == 1);
		cp = decoded_codepoint_from_wchar(L"Ω", 2);
		AssertAlways(cp.codepoint == 0x2126);
		AssertAlways(cp.advance == 1);
		cp  = decoded_codepoint_from_wchar(L"🍌", 2);
		AssertAlways(cp.codepoint == 0x1F34C);
#if COMPILER_CL
		AssertAlways(cp.advance == 2);
#elif COMPILER_CLANG || COMPILER_GCC
		AssertAlways(cp.advance == 1);
#else
#  error "unhandled compiler"
#endif
		
		print_verbose("[KIGU-TEST] PASSED: unicode/decoding\n");
	}
	
	//TODO test invalid encoding
	{//encoding
		u32 cp1 = U'a'; //0x61
		u32 cp2 = U'Д'; //0x0414
		u32 cp3 = U'Ω'; //0x2126
		u32 cp4 = U'🍌'; //0x0001F34C
		u32 advance;
		
		u8 u8_buffer[4] = {0};
		advance = utf8_from_codepoint(u8_buffer, cp1);
		AssertAlways(advance == 1);
		AssertAlways(u8_buffer[0] == 0x61);
		AssertAlways(u8_buffer[1] == 0x00);
		AssertAlways(u8_buffer[2] == 0x00);
		AssertAlways(u8_buffer[3] == 0x00);
		advance = utf8_from_codepoint(u8_buffer, cp2);
		AssertAlways(advance == 2);
		AssertAlways(u8_buffer[0] == 0xD0);
		AssertAlways(u8_buffer[1] == 0x94);
		AssertAlways(u8_buffer[2] == 0x00);
		AssertAlways(u8_buffer[3] == 0x00);
		advance = utf8_from_codepoint(u8_buffer, cp3);
		AssertAlways(advance == 3);
		AssertAlways(u8_buffer[0] == 0xE2);
		AssertAlways(u8_buffer[1] == 0x84);
		AssertAlways(u8_buffer[2] == 0xA6);
		AssertAlways(u8_buffer[3] == 0x00);
		advance = utf8_from_codepoint(u8_buffer, cp4);
		AssertAlways(advance == 4);
		AssertAlways(u8_buffer[0] == 0xF0);
		AssertAlways(u8_buffer[1] == 0x9F);
		AssertAlways(u8_buffer[2] == 0x8D);
		AssertAlways(u8_buffer[3] == 0x8C);
		
		u16 u16_buffer[2] = {0};
		advance = utf16_from_codepoint(u16_buffer, cp1);
		AssertAlways(advance == 1);
		AssertAlways(u16_buffer[0] == 0x0061);
		AssertAlways(u16_buffer[1] == 0x0000);
		advance = utf16_from_codepoint(u16_buffer, cp2);
		AssertAlways(advance == 1);
		AssertAlways(u16_buffer[0] == 0x0414);
		AssertAlways(u16_buffer[1] == 0x0000);
		advance = utf16_from_codepoint(u16_buffer, cp3);
		AssertAlways(advance == 1);
		AssertAlways(u16_buffer[0] == 0x2126);
		AssertAlways(u16_buffer[1] == 0x0000);
		advance = utf16_from_codepoint(u16_buffer, cp4);
		AssertAlways(advance == 2);
		AssertAlways(u16_buffer[0] == 0xD83C);
		AssertAlways(u16_buffer[1] == 0xDF4C);
		
		wchar_t w_buffer[2] = {0};
		advance = wchar_from_codepoint(w_buffer, cp1);
		AssertAlways(advance == 1);
		AssertAlways(w_buffer[0] == 0x0061);
		AssertAlways(w_buffer[1] == 0x0000);
		advance = wchar_from_codepoint(w_buffer, cp2);
		AssertAlways(advance == 1);
		AssertAlways(w_buffer[0] == 0x0414);
		AssertAlways(w_buffer[1] == 0x0000);
		advance = wchar_from_codepoint(w_buffer, cp3);
		AssertAlways(advance == 1);
		AssertAlways(w_buffer[0] == 0x2126);
		AssertAlways(w_buffer[1] == 0x0000);
		 advance = wchar_from_codepoint(w_buffer, cp4);
#if COMPILER_CL
		AssertAlways(advance == 2);
		AssertAlways(w_buffer[0] == 0xD83C);
		AssertAlways(w_buffer[1] == 0xDF4C);
#elif COMPILER_CLANG || COMPILER_GCC
		AssertAlways(advance == 1);
		AssertAlways(w_buffer[0] == 0x0001F34C);
		AssertAlways(w_buffer[1] == 0x00000000);
#else
#  error "unhandled compiler"
#endif
		
		print_verbose("[KIGU-TEST] PASSED: unicode/encoding\n");
	}
	
	{//conversion
		str8 z8 = str8_from_str16(test16);
		forI(z8.count) AssertAlways(z8.str[i] == test8.str[i]);
		
		z8 = str8_from_str32(test32);
		forI(z8.count) AssertAlways(z8.str[i] == test8.str[i]);
		
		z8 = str8_from_wchar(testw);
		forI(z8.count) AssertAlways(z8.str[i] == test8.str[i]);
		
		str16 z16 = str16_from_str8(test8);
		forI(z16.count) AssertAlways(z16.str[i] == test16.str[i]);
		
		str32 z32 = str32_from_str8(test8);
		forI(z32.count) AssertAlways(z32.str[i] == test32.str[i]);
		
		s64 count;
		wchar_t* zw = wchar_from_str8(test8, &count);
#if COMPILER_CL
		AssertAlways(count == test16.count);
		forI(count) AssertAlways(zw[i] == test16.str[i]);
#elif COMPILER_CLANG || COMPILER_GCC
		AssertAlways(count == test32.count);
		forI(count) AssertAlways(zw[i] == test32.str[i]);
#else
#  error "unhandled compiler"
#endif
		
		print_verbose("[KIGU-TEST] PASSED: unicode/conversion\n");
	}
	
	{//advancing
		str8 z1 = test8;
		DecodedCodepoint cp = str8_advance(&z1);
		AssertAlways(cp.codepoint == 'a');
		AssertAlways(cp.advance == 1);
		AssertAlways(z1.str[0] == ' ');
		
		cp = str8_nadvance(&z1, 7);
		AssertAlways(cp.codepoint == ' ');
		AssertAlways(cp.advance == 1);
		AssertAlways(z1.str[0] == 0xD0); //Д
		AssertAlways(z1.str[1] == 0x94);
		
		str8_advance_until(&z1, 0x2563); //╣
		AssertAlways(z1.str[0] == 0xE2); //╣
		AssertAlways(z1.str[1] == 0x95);
		AssertAlways(z1.str[2] == 0xA3);
		
		cp = str8_advance(&z1);
		AssertAlways(cp.codepoint == 0x2563); //╣
		AssertAlways(cp.advance == 3);
		AssertAlways(z1.str[0] == ' ');
		
		str8_advance_while(&z1, ' ');
		AssertAlways(z1.str[0] == 0xF0); //🍌
		AssertAlways(z1.str[1] == 0x9F);
		AssertAlways(z1.str[2] == 0x8D);
		AssertAlways(z1.str[3] == 0x8C);
		
		print_verbose("[KIGU-TEST] PASSED: unicode/advancing\n");
	}
	
	{//indexing
		u32 z1 = str8_index(test8, 18).codepoint;
		AssertAlways(z1 == 0x419); //Й
		
		u32 z2 = str8_length(test8);
		AssertAlways(z2 == 56);
		
		print_verbose("[KIGU-TEST] PASSED: unicode/indexing\n");
	}
	
	{//comparison
		AssertAlways(str8_compare(test8, test8) == 0);
		AssertAlways(str8_compare(str8_lit(TEST_KIGU_UNICODE_LITERAL), str8_lit(TEST_KIGU_UNICODE_LITERAL)) == 0);
		AssertAlways(str8_compare(str8_lit(""), str8_lit("")) == 0);
		AssertAlways(str8_compare(str8_lit("a"), str8_lit("a")) == 0);
		AssertAlways(str8_compare(str8_lit("Д"), str8_lit("Д")) == 0);
		AssertAlways(str8_compare(str8_lit("Ω"), str8_lit("Ω")) == 0);
		AssertAlways(str8_compare(str8_lit("🍌"), str8_lit("🍌")) == 0);
		AssertAlways(str8_compare(str8_lit("abc"), str8_lit("abd")) < 0);
		AssertAlways(str8_compare(str8_lit("abd"), str8_lit("abc")) > 0);
		AssertAlways(str8_compare(str8_lit("abc"), str8_lit("abcd")) < 0);
		AssertAlways(str8_compare(str8_lit("abcd"), str8_lit("abc")) > 0);
		AssertAlways(str8_compare(str8_lit("abcД"), str8_lit("abcΩ")) < 0);
		AssertAlways(str8_compare(str8_lit("abcΩ"), str8_lit("abcД")) > 0);
		
		AssertAlways(str8_ncompare(test8, test8, 1) == 0);
		AssertAlways(str8_ncompare(test8, test8, 10) == 0);
		AssertAlways(str8_ncompare(str8_lit("abcq"), str8_lit("abcb"), 3) == 0);
		AssertAlways(str8_ncompare(str8_lit("abcД"), str8_lit("abcΩ"), 3) == 0);
		AssertAlways(str8_ncompare(str8_lit("abcД"), str8_lit("abc🍌"), 3) == 0);
		AssertAlways(str8_ncompare(str8_lit("qabc"), str8_lit("qxyz"), 1) == 0);
		AssertAlways(str8_ncompare(str8_lit("Дabc"), str8_lit("Дxyz"), 1) == 0);
		AssertAlways(str8_ncompare(str8_lit("Ωabc"), str8_lit("Ωxyz"), 1) == 0);
		AssertAlways(str8_ncompare(str8_lit("🍌abc"), str8_lit("🍌xyz"), 1) == 0);
		AssertAlways(str8_ncompare(str8_lit("abc"), str8_lit("abd"), 3) < 0);
		AssertAlways(str8_ncompare(str8_lit("abd"), str8_lit("abc"), 3) > 0);
		AssertAlways(str8_ncompare(str8_lit("abc"), str8_lit("abcd"), 4) < 0);
		AssertAlways(str8_ncompare(str8_lit("abcd"), str8_lit("abc"), 4) > 0);
		AssertAlways(str8_ncompare(str8_lit("abcД"), str8_lit("abcΩ"), 4) < 0);
		AssertAlways(str8_ncompare(str8_lit("abcΩ"), str8_lit("abcД"), 4) > 0);
		
		AssertAlways(str8_equal(test8, test8));
		AssertAlways(str8_equal(str8_lit(TEST_KIGU_UNICODE_LITERAL), str8_lit(TEST_KIGU_UNICODE_LITERAL)));
		AssertAlways(str8_equal(str8_lit(""), str8_lit("")));
		AssertAlways(str8_equal(str8_lit("a"), str8_lit("a")));
		AssertAlways(str8_equal(str8_lit("Д"), str8_lit("Д")));
		AssertAlways(str8_equal(str8_lit("Ω"), str8_lit("Ω")));
		AssertAlways(str8_equal(str8_lit("🍌"), str8_lit("🍌")));
		
		AssertAlways(str8_nequal(test8, test8, 1));
		AssertAlways(str8_nequal(test8, test8, 10));
		AssertAlways(str8_nequal(str8_lit("abcq"), str8_lit("abcb"), 3));
		AssertAlways(str8_nequal(str8_lit("abcД"), str8_lit("abcΩ"), 3));
		AssertAlways(str8_nequal(str8_lit("abcД"), str8_lit("abc🍌"), 3));
		AssertAlways(str8_nequal(str8_lit("qabc"), str8_lit("qxyz"), 1));
		AssertAlways(str8_nequal(str8_lit("Дabc"), str8_lit("Дxyz"), 1));
		AssertAlways(str8_nequal(str8_lit("Ωabc"), str8_lit("Ωxyz"), 1));
		AssertAlways(str8_nequal(str8_lit("🍌abc"), str8_lit("🍌xyz"), 1));
		
		print_verbose("[KIGU-TEST] PASSED: unicode/comparison\n");
	}
	
	{//searching
		AssertAlways(str8_begins_with(str8{}, str8{}));
		AssertAlways(str8_begins_with(str8_lit("abc123"), str8_lit("abc")));
		AssertAlways(str8_begins_with(str8_lit("abcД"), str8_lit("abc")));
		AssertAlways(str8_begins_with(str8_lit("Д123"), str8_lit("Д")));
		AssertAlways(str8_begins_with(str8_lit("ДД"), str8_lit("Д")));
		AssertAlways(str8_begins_with(str8_lit("Ω123"), str8_lit("Ω")));
		AssertAlways(str8_begins_with(str8_lit("ΩД"), str8_lit("Ω")));
		AssertAlways(str8_begins_with(str8_lit("🍌123"), str8_lit("🍌")));
		AssertAlways(str8_begins_with(str8_lit("🍌Д"), str8_lit("🍌")));
		
		AssertAlways(!str8_begins_with(str8_lit("abc123"), str8_lit("123")));
		AssertAlways(!str8_begins_with(str8_lit("abcД"), str8_lit("Д")));
		AssertAlways(!str8_begins_with(str8_lit("Д123"), str8_lit("123")));
		AssertAlways(!str8_begins_with(str8_lit("ДД"), str8_lit("z")));
		AssertAlways(!str8_begins_with(str8_lit("Ω123"), str8_lit("123")));
		AssertAlways(!str8_begins_with(str8_lit("ΩД"), str8_lit("Д")));
		AssertAlways(!str8_begins_with(str8_lit("🍌123"), str8_lit("123")));
		AssertAlways(!str8_begins_with(str8_lit("🍌Д"), str8_lit("Д")));
		
		AssertAlways(str8_ends_with(str8{}, str8{}));
		AssertAlways(str8_ends_with(str8_lit("abc123"), str8_lit("123")));
		AssertAlways(str8_ends_with(str8_lit("abcД"), str8_lit("Д")));
		AssertAlways(str8_ends_with(str8_lit("Д123"), str8_lit("123")));
		AssertAlways(str8_ends_with(str8_lit("ДД"), str8_lit("Д")));
		AssertAlways(str8_ends_with(str8_lit("Ω123"), str8_lit("123")));
		AssertAlways(str8_ends_with(str8_lit("ΩД"), str8_lit("Д")));
		AssertAlways(str8_ends_with(str8_lit("🍌123"), str8_lit("123")));
		AssertAlways(str8_ends_with(str8_lit("🍌Д"), str8_lit("Д")));
		
		AssertAlways(!str8_ends_with(str8_lit("abc123"), str8_lit("abc")));
		AssertAlways(!str8_ends_with(str8_lit("abcД"), str8_lit("abc")));
		AssertAlways(!str8_ends_with(str8_lit("Д123"), str8_lit("Д")));
		AssertAlways(!str8_ends_with(str8_lit("ДД"), str8_lit("z")));
		AssertAlways(!str8_ends_with(str8_lit("Ω123"), str8_lit("Ω")));
		AssertAlways(!str8_ends_with(str8_lit("ΩД"), str8_lit("Ω")));
		AssertAlways(!str8_ends_with(str8_lit("🍌123"), str8_lit("🍌")));
		AssertAlways(!str8_ends_with(str8_lit("🍌Д"), str8_lit("🍌")));
		
		AssertAlways(str8_contains(str8{}, str8{}));
		AssertAlways(str8_contains(test8, str8_lit("a")));
		AssertAlways(str8_contains(test8, str8_lit(" ")));
		AssertAlways(str8_contains(test8, str8_lit("b c ")));
		AssertAlways(str8_contains(test8, str8_lit("Д")));
		AssertAlways(str8_contains(test8, str8_lit("Д Е")));
		AssertAlways(str8_contains(test8, str8_lit("З И Й")));
		AssertAlways(str8_contains(test8, str8_lit("♪ ♫ ╞")));
		AssertAlways(str8_contains(test8, str8_lit("🍌")));
		AssertAlways(str8_contains(test8, str8_lit("╣      🍌")));
		
		AssertAlways(!str8_contains(test8, str8_lit("!")));
		AssertAlways(!str8_contains(test8, str8_lit("z")));
		AssertAlways(!str8_contains(test8, str8_lit("a b d")));
		AssertAlways(!str8_contains(test8, str8_lit("З И Й_")));
		AssertAlways(!str8_contains(test8, str8_lit("╣  !   🍌")));
		AssertAlways(!str8_contains(test8, str8_lit("᠅")));
		
		print_verbose("[KIGU-TEST] PASSED: unicode/searching\n");
	}
	
	{//slicing
		AssertAlways(str8_equal(str8_eat_one(test8), str8_lit("a")));
		AssertAlways(str8_equal(str8_eat_one(str8_eat_one(test8)), str8{}));
		
		AssertAlways(str8_equal(str8_eat_count(test8, 0), str8{}));
		AssertAlways(str8_equal(str8_eat_count(str8{}, 0), str8{}));
		AssertAlways(str8_equal(str8_eat_count(str8{}, 23), str8{}));
		AssertAlways(str8_equal(str8_eat_count(test8, 5), str8_lit("a b c")));
		AssertAlways(str8_equal(str8_eat_count(test8, 16), str8_lit("a b c d Д Е Ж З ")));
		
		AssertAlways(str8_equal(str8_eat_until(str8_lit("  777777   abc"), U'a'), str8_lit("  777777   ")));
		AssertAlways(str8_equal(str8_eat_until(test8, U' '), str8_lit("a")));
		AssertAlways(str8_equal(str8_eat_until(test8, U'b'), str8_lit("a ")));
		AssertAlways(str8_equal(str8_eat_until(test8, U'♪'), str8_lit("a b c d Д Е Ж З И Й К Л У Ф Х ≤ ≥ ")));
		AssertAlways(str8_eat_until(test8, U'!').count == 0);
		AssertAlways(str8_eat_until(test8, U'á').count == 0);
		
		AssertAlways(str8_equal(str8_eat_while(str8_lit("  777777   abc"), U' '), str8_lit("  ")));
		AssertAlways(str8_equal(str8_eat_while(str8_lit("♪♪♪♪♪   abc"), U'♪'), str8_lit("♪♪♪♪♪")));
		AssertAlways(str8_equal(str8_eat_while(str8_lit("🍌🍌🍌🍌🍌abc"), U'🍌'), str8_lit("🍌🍌🍌🍌🍌")));
		AssertAlways(!str8_equal(str8_eat_while(str8_lit("🍌🍌🍌🍌🍌abc"), U' '), str8_lit("🍌🍌🍌🍌🍌")));
		
		AssertAlways(str8_equal(str8_skip_one(test8), str8_lit(" b c d Д Е Ж З И Й К Л У Ф Х ≤ ≥ ♪ ♫ ╞ ╟ ╠ ╡ ╢ ╣      🍌")));
		AssertAlways(str8_equal(str8_skip_one(str8_skip_one(test8)), str8_lit("b c d Д Е Ж З И Й К Л У Ф Х ≤ ≥ ♪ ♫ ╞ ╟ ╠ ╡ ╢ ╣      🍌")));
		
		AssertAlways(str8_equal(str8_skip_count(test8, 0), test8));
		AssertAlways(str8_equal(str8_skip_count(str8{}, 0), str8{}));
		AssertAlways(str8_equal(str8_skip_count(str8{}, 23), str8{}));
		AssertAlways(str8_equal(str8_skip_count(test8, 5), str8_lit(" d Д Е Ж З И Й К Л У Ф Х ≤ ≥ ♪ ♫ ╞ ╟ ╠ ╡ ╢ ╣      🍌")));
		AssertAlways(str8_equal(str8_skip_count(test8, 16), str8_lit("И Й К Л У Ф Х ≤ ≥ ♪ ♫ ╞ ╟ ╠ ╡ ╢ ╣      🍌")));
		
		AssertAlways(str8_equal(str8_skip_until(str8_lit("  777777   abc"), U'a'), str8_lit("abc")));
		AssertAlways(str8_equal(str8_skip_until(test8, U' '), str8_lit(" b c d Д Е Ж З И Й К Л У Ф Х ≤ ≥ ♪ ♫ ╞ ╟ ╠ ╡ ╢ ╣      🍌")));
		AssertAlways(str8_equal(str8_skip_until(test8, U'b'), str8_lit("b c d Д Е Ж З И Й К Л У Ф Х ≤ ≥ ♪ ♫ ╞ ╟ ╠ ╡ ╢ ╣      🍌")));
		AssertAlways(str8_equal(str8_skip_until(test8, U'♪'), str8_lit("♪ ♫ ╞ ╟ ╠ ╡ ╢ ╣      🍌")));
		AssertAlways(str8_skip_until(test8, U'!').count == 0);
		AssertAlways(str8_skip_until(test8, U'á').count == 0);
		
		AssertAlways(str8_equal(str8_skip_while(str8_lit("  777777   abc"), U' '), str8_lit("777777   abc")));
		AssertAlways(str8_equal(str8_skip_while(str8_lit("♪♪♪♪♪   abc"), U'♪'), str8_lit("   abc")));
		AssertAlways(str8_equal(str8_skip_while(str8_lit("🍌🍌🍌🍌🍌abc"), U'🍌'), str8_lit("abc")));
		AssertAlways(!str8_equal(str8_skip_while(str8_lit("🍌🍌🍌🍌🍌abc"), U' '), str8_lit("abc")));
		
		print_verbose("[KIGU-TEST] PASSED: unicode/slicing\n");
	}
	
	{//building
		AssertAlways(str8_equal(test8, str8_copy(test8)));
		AssertAlways(str8_equal(test8, str8_copy(str8_lit(TEST_KIGU_UNICODE_LITERAL))));
		AssertAlways(str8_equal(str8_lit("abc"), str8_copy(str8_lit("abc"))));
		
		str8_builder builder1;
		str8_builder_init(&builder1, {});
		AssertAlways(builder1.count == 0);
		AssertAlways(builder1.space == 8);
		AssertAlways(builder1.str != 0);
		AssertAlways(builder1.allocator == KIGU_UNICODE_ALLOCATOR);
		str8_builder builder2;
		str8_builder_init(&builder2, test8);
		AssertAlways(builder2.count == 90);
		AssertAlways(builder2.space == 96);
		AssertAlways(builder2.str != 0);
		AssertAlways(builder2.str != test8.str);
		AssertAlways(builder2.allocator == KIGU_UNICODE_ALLOCATOR);
		
		str8_builder_fit(&builder2);
		AssertAlways(builder2.count == 90);
		AssertAlways(builder2.space == 91);
		AssertAlways(builder2.str != 0);
		AssertAlways(builder2.allocator == KIGU_UNICODE_ALLOCATOR);
		
		str8_builder_append(&builder2, str8{});
		AssertAlways(builder2.count == 90);
		AssertAlways(builder2.space == 91);
		AssertAlways(builder2.str != 0);
		AssertAlways(str8_length(str8{builder2.str, builder2.count}) == 56);
		AssertAlways(str8_equal(str8{builder2.str, builder2.count}, str8_lit("a b c d Д Е Ж З И Й К Л У Ф Х ≤ ≥ ♪ ♫ ╞ ╟ ╠ ╡ ╢ ╣      🍌")));
		str8_builder_append(&builder2, str8_lit(""));
		AssertAlways(builder2.count == 90);
		AssertAlways(builder2.space == 91);
		AssertAlways(builder2.str != 0);
		AssertAlways(str8_length(str8{builder2.str, builder2.count}) == 56);
		AssertAlways(str8_equal(str8{builder2.str, builder2.count}, str8_lit("a b c d Д Е Ж З И Й К Л У Ф Х ≤ ≥ ♪ ♫ ╞ ╟ ╠ ╡ ╢ ╣      🍌")));
		str8_builder_append(&builder2, str8_lit("a"));
		AssertAlways(builder2.count == 91);
		AssertAlways(builder2.space == 96);
		AssertAlways(builder2.str != 0);
		AssertAlways(str8_length(str8{builder2.str, builder2.count}) == 57);
		AssertAlways(str8_equal(str8{builder2.str, builder2.count}, str8_lit("a b c d Д Е Ж З И Й К Л У Ф Х ≤ ≥ ♪ ♫ ╞ ╟ ╠ ╡ ╢ ╣      🍌a")));
		str8_builder_append(&builder2, str8_lit("Д"));
		AssertAlways(builder2.count == 93);
		AssertAlways(builder2.space == 96);
		AssertAlways(builder2.str != 0);
		AssertAlways(str8_length(str8{builder2.str, builder2.count}) == 58);
		AssertAlways(str8_equal(str8{builder2.str, builder2.count}, str8_lit("a b c d Д Е Ж З И Й К Л У Ф Х ≤ ≥ ♪ ♫ ╞ ╟ ╠ ╡ ╢ ╣      🍌aД")));
		str8_builder_append(&builder2, str8_lit("Ω"));
		AssertAlways(builder2.count == 96);
		AssertAlways(builder2.space == 104);
		AssertAlways(builder2.str != 0);
		AssertAlways(str8_length(str8{builder2.str, builder2.count}) == 59);
		AssertAlways(str8_equal(str8{builder2.str, builder2.count}, str8_lit("a b c d Д Е Ж З И Й К Л У Ф Х ≤ ≥ ♪ ♫ ╞ ╟ ╠ ╡ ╢ ╣      🍌aДΩ")));
		str8_builder_append(&builder2, str8_lit("🍌"));
		AssertAlways(builder2.count == 100);
		AssertAlways(builder2.space == 104);
		AssertAlways(builder2.str != 0);
		AssertAlways(str8_length(str8{builder2.str, builder2.count}) == 60);
		AssertAlways(str8_equal(str8{builder2.str, builder2.count}, str8_lit("a b c d Д Е Ж З И Й К Л У Ф Х ≤ ≥ ♪ ♫ ╞ ╟ ╠ ╡ ╢ ╣      🍌aДΩ🍌")));
		str8_builder_append(&builder2, str8_lit("abc"));
		AssertAlways(builder2.count == 103);
		AssertAlways(builder2.space == 104);
		AssertAlways(builder2.str != 0);
		AssertAlways(str8_length(str8{builder2.str, builder2.count}) == 63);
		AssertAlways(str8_equal(str8{builder2.str, builder2.count}, str8_lit("a b c d Д Е Ж З И Й К Л У Ф Х ≤ ≥ ♪ ♫ ╞ ╟ ╠ ╡ ╢ ╣      🍌aДΩ🍌abc")));
		
		print_verbose("[KIGU-TEST] PASSED: unicode/building\n");
	}
	
	#if TEST_KIGU_PRINT_VERBOSE
	{//printing
		fflush(stdout);
		_setmode(_fileno(stdout), _O_U16TEXT);
		setlocale(LC_ALL, "en_US.utf8");
		
		wprintf(L"-------- Unicode Printing --------\n");
		wprintf(L"UTF8 : %ls\n", wchar_from_str8(test8));
		wprintf(L"UTF16: %ls\n", wchar_from_str8(str8_from_str16(test16)));
		wprintf(L"UTF32: %ls\n", wchar_from_str8(str8_from_str32(test32)));
		wprintf(L"Wide : %ls\n", testw);
		wchar_t test_buffer[1024] = {0};
		mbstowcs(test_buffer, GLUE(u8,TEST_KIGU_UNICODE_LITERAL), 1024);
		wprintf(L"U8toW: %ls\n", test_buffer);
		
		fflush(stdout);
		_setmode(_fileno(stdout), _O_TEXT);
		print_verbose("[KIGU-TEST] PASSED: unicode/printing\n");
	}
	#endif
	
	printf("[KIGU-TEST] PASSED: unicode\n");
#undef TEST_KIGU_UNICODE_LITERAL
}

#include "utils.h"
local void TEST_kigu_utils(){
	printf("[KIGU-TEST] TODO:   utils\n");
}

local void TEST_kigu(){
	TEST_kigu_array();
	TEST_kigu_array_utils();
	TEST_kigu_carray();
	TEST_kigu_color();
	TEST_kigu_cstring();
	TEST_kigu_hash();
	TEST_kigu_map();
	TEST_kigu_optional();
	TEST_kigu_ring_array();
	TEST_kigu_string();
	TEST_kigu_string_utils();
	TEST_kigu_pair();
	TEST_kigu_unicode();
	TEST_kigu_utils();
}