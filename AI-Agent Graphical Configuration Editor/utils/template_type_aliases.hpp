#if defined(_MSC_VER) || defined(__GNUC__) || defined(__clang__) || defined(__INTEL_COMPILER) || defined(__BORLANDC__) || defined(__MWERKS__) || defined(__DMC__)
#pragma once
#endif

#ifndef UTILS_TYPE_ALIASES_HPP
#define UTILS_TYPE_ALIASES_HPP


/*
	头文件: utils/template_type_aliases.hpp
	
	定义模板类型别名
	
	in = const T&
	out = T&
	ref = T&&
*/
namespace utils 
{
	template<typename T>
	using in = const T&;
	template<typename T>
	using out = T&;
	template<typename T>
	using ref = T&&;
}



#endif