#pragma once
#include <iostream>
#include <string>

template <typename Tuple,int Index> 
struct print_helper {
	static void print(const Tuple& t) {
		std::cout << boost::tuples::get<Index>(t) << '\n';
		print_helper<Tuple,Index-1>::print(t);
	}
};

template<typename Tuple> 
struct print_helper<Tuple,0> {
	static void print(const Tuple& t) {
		std::cout << boost::tuples::get<0>(t) << '\n';
	}
};

template <typename Tuple> void print_all(const Tuple& t) {
	print_helper<
		Tuple,boost::tuples::length<Tuple>::value-1>::print(t);
}

struct None{};

template <int index>
struct argType
{
	typedef None Type;
	typedef argType<index-1> Next;
	//typedef Next::Type NextType;
};

template <>
struct argType<0>
{
	typedef int Type;
};

template <>
struct argType<1>
{
	typedef double Type;
	typedef argType<1-1> Next;
};

struct AAA{
	AAA(){ 
		argType<0>::Type a = 1; 
		argType<2>::Next::Next::Type b = a;
		const char* str1 = typeid(a).name();
		const char* str2 = typeid(b).name();
		printf("%s\n",str1);
	}
};
static AAA aaa;