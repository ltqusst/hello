/*
 * cpp11_callable1.cpp
 *
 *  Created on: Sep 20, 2017
 *      Author: tingqian
 */
#include <iostream>
#include <functional>
#include <cmath>

using namespace std;

int f(int x, int y){
	return x+y;
}

auto l = [](int x, int y){
	return x+y;
};

class C{
public:
	C(int id=10){
		code = id*100;
	}
	int operator()(int x, int y){
		return code+x+y;
	}
	C operator+(const C &a){
		C temp(0);
		temp.code = this->code + a.code;
		return temp;
	}
	int memberfunc(int x, int y){
		return code+x+y;
	}
	int code;
};



std::ostream & operator<<(std::ostream &os, const C &m){
	return os << "C{ .code=" << m.code << "}";
}


template<class F>
F add1(F a){
	return a+(F)1;
}

template<class F>
int test(F func, int x, int y)
{
	//cout << "test(F=" << typeid(func).name() << ")" << endl;
	func(x,y);
}

int mysqrt(double d){
	return sqrt(d);
}

////////////////////////////////////////////////////////////////////////////
// C++ functor
// an upgraged function pointer, only useable through template
class funcPeter{
public:
	funcPeter(int id){
		m_id = id;
	}
	void operator()(const char * p){
		printf("funcPeter_%d::() is called with p=%s\n",m_id, p);
	}
	int m_id;
};


void funcHelix(const char * p){
	printf("%s is called with p=%s\n", __FUNCTION__, p);
}

template<class Functor>
void test_func(Functor func, const char *p){
	cout <<"test_func is calling func(p): ";
	func(p);
}

void test_functor()
{
	funcPeter peter(333);
	test_func(funcHelix, "hello Helix!");
	test_func(funcPeter(123), "hello Peter1!");
	test_func(peter, "hello Peter2!");
}
/////////////////////////////////////////////////////////////////////////////
int main()
{
	test_functor();

	C c4(4);

	//automatic data types deduction of template
	cout<<"add1(1)=" << add1(1) << endl;
	cout<<"add1(1.1)=" << add1(1.1) << endl;
	cout<<"add1('a')=" << add1('a') << endl;
	cout<<"add1(c4)=" << add1(c4) << endl;

	//bind: dynamically construct function wrapper
	int n = 7;
	using namespace std::placeholders;//with place holders _1,_2,...
	auto coXX = std::bind(&C::memberfunc,std::ref(c4), _1, _2);
	auto coX_ = std::bind(&C::memberfunc,c4, _1, _1);
	auto co67 = std::bind(&C::memberfunc,c4, 6, 7);
	auto co6n = std::bind(&C::memberfunc,c4, 6, n);
	auto co6rn = std::bind(&C::memberfunc,c4, 6, std::ref(n));
	//nest bind shares placeholders
	auto cobb = std::bind(&C::memberfunc,c4,
							std::bind(mysqrt, _2),
							std::bind(mysqrt, std::ref(n)));
	n = 4;
	c4.code = 300;

	//automatic function/callable type deduction of template
	cout << "test(f, 1,2)=" << test(f, 1,2) << "  f(1,2)=" << f(1,2) << endl;
	cout << "test(l, 1,2)=" << test(l, 1,2) << "  l(1,2)=" << l(1,2) << endl;
	cout << "test(co67, 1,2)=" << test(co67, 1,2) << "  co67(1,2)=" << co67(1,2) << endl;
	cout << "test(coXX, 1,2)=" << test(coXX, 1,2) << "  coXX(1,2)=" << coXX(1,2) << endl;
	cout << "test(coX_, 1,2)=" << test(coX_, 1,2) << "  coX_(1,2)=" << coX_(1,2) << endl;
	cout << "test(co6n, 1,2)=" << test(co6n, 1,2) << "  co6n(1,2)=" << co6n(1,2) << endl;
	cout << "test(co6rn, 1,2)=" << test(co6rn, 1,2) << "  co6rn(1,2)=" << co6rn(1,2) << endl;
	cout << "test(cobb, 9,16)=" << test(cobb, 9.0,16.0) << "  cobb(9,16)=" << cobb(9.0,16.0) << endl;
}


