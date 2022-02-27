#pragma once
#ifndef KIGU_TYPE_H
#define KIGU_TYPE_H

#include "common.h"
#include "utils.h"
#include "unicode.h"

#define type_info(var) __type_info_dummy

struct TypeInfo{
	u32  index; //in the global array
	str8 name; //special format for generics array?u32  array?string
	u32*  member_var_types; //type indexes of member vars
	u64   member_var_types_count;
	str8* member_var_names;
	u64   member_var_names_count;
	

	//and maybe member funcs, enums, sub-structs
};
TypeInfo __type_info_dummy{0, str8_lit("void")};

#endif //KIGU_TYPE_H