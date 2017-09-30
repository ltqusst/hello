/*
 * cpp11_objmodel2.cpp
 *
 *  Created on: Sep 24, 2017
 *      Author: tingqian
 */
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <typeinfo>


//
//  get_vtbl(p,-3) <==  virtual-base member offset
//  get_vtbl(p,-1) <==  typeinfo
//  get_vtbl(p,0)  <==  first virtual function
//
//
#define get_vtbl(p, id) (*(reinterpret_cast<void ***>(p)))[id]
#define SHOW_POS(v,type) \
	if((reinterpret_cast<int*>(static_cast<type *>(&v))==&p[k])) \
		printf("   <----static_cast<%s *>(&%s),size=%lu,type=%s",#type,#v, sizeof(type), typeid(v).name());


#define SHOW_MY_FLAG()  	printf("\n============================== %s ==============================\n",__FUNCTION__);

//=======================================================================================
class Foo
{
public:
	Foo(const char *name){
		strcpy(m_name,name);
	}
	virtual void vfunc(int a, int b){
		printf("%s.vfunc(%d,%d) is called\n",m_name,a,b);
	};
	virtual void vfunc2(int a, int b){
		printf("%s.vfunc2(%d,%d) is called\n",m_name,a,b);
	};

	void func(int a, int b){
		printf("%s.func(%d,%d) is called\n",m_name,a,b);
	};
private:
	char m_name[10];
};

class Interface1
{
public:
	virtual void vfunc1(void){printf("Interface1 vfunc1 is called\n");};
	virtual void vfunc2(void){printf("Interface1 vfunc2 is called\n");};
	virtual void vfunc3(void){printf("Interface1 vfunc3 is called\n");};
};
class Interface2
{
public:
	virtual void vfunc1(void){printf("Interface2 vfunc1 is called\n");};
	virtual void vfunc2(void){printf("Interface2 vfunc2 is called\n");};
	virtual void vfunc3(void){printf("Interface2 vfunc3 is called\n");};
};

class Implement: public Interface1, public Interface2
{
public:
	Implement(const char * pname):m_name(pname){}

	virtual void vfunc1(void){printf("Implement %s's vfunc1 is called\n", m_name);};

	virtual void vfuncXA(void){printf("Implement %s's vfuncXA is called\n", m_name);};
	virtual void vfuncXB(void){printf("Implement %s's vfuncXB is called\n", m_name);};
	virtual void vfuncXC(void){printf("Implement %s's vfuncXC is called\n", m_name);};

	const char * m_name;
};

void test_virtual_func()
{
	SHOW_MY_FLAG();

	Foo bar("bar");
	Foo *pbar = &bar;

	bar.func(1,2);
	bar.vfunc(1,2);    //call virtual function on original object will not involve vptr;
	pbar->func(1,2);
	pbar->vfunc(1,2);

	//151               	pbar->func();
	//0000000000401523:   mov -0x10(%rbp),%rax
	//0000000000401527:   mov %rax,%rdi ( pointer "this" )
	//000000000040152a:   callq 0x401726 <Foo::func()>

	//152               	pbar->vfunc();
	//000000000040152f:   mov -0x10(%rbp),%rax
	//0000000000401533:   mov (%rax),%rax       // read table address stored in first element of object
	//0000000000401536:   mov (%rax),%rax		// read first entry of the table
	//0000000000401539:   mov -0x10(%rbp),%rdx	// read "this" pointer again
	//000000000040153d:   mov %rdx,%rdi 		// pass "this" pointer as first argument
	//0000000000401540:   callq *%rax			// call the virtual function


	//==================================================
	//call virtual function manually
	uint64_t * pi64 = reinterpret_cast<uint64_t *>(&bar);
	uint64_t * pvptr = reinterpret_cast<uint64_t *>(pi64[0]); //first element of bar is vptr base addr
	void (*pvfunc)(Foo *,int, int) = reinterpret_cast<void (*)(Foo*,int,int)>(pvptr[0]);
	void (*pvfunc2)(Foo *,int, int) = reinterpret_cast<void (*)(Foo*,int,int)>(pvptr[1]);

	(*pvfunc)(&bar,123,456);
	(*pvfunc2)(&bar,789,234);


	printf("\nclass Implement: public Interface1, public Interface2\n");
	printf("Implement imp;\n");

	Implement imp("imp123");
	imp.vfunc1();
	imp.Interface1::vfunc1();
	imp.Interface1::vfunc2();
	imp.Interface1::vfunc3();
	imp.Interface2::vfunc1();
	imp.Interface2::vfunc2();
	imp.Interface2::vfunc3();

	printf("\nclass Implement: public Interface1, public Interface2\n");
	printf("Implement imp;\n");
	int *p = reinterpret_cast<int*>(&imp);
	for(int k=0;k<sizeof(imp)/sizeof(int);k++)
	{
		printf("\tp[%d]=0x%8X",k, p[k]);
		SHOW_POS(imp,Implement);
		SHOW_POS(imp,Interface1);
		SHOW_POS(imp,Interface2);
		SHOW_POS(imp.m_name,const char *);
		printf("\n");
	}

	printf("\n[Inside virtual function table of imp]:\n");
	void * pvptr_imp = *(reinterpret_cast<void **>(&imp));
	void (**pvfuncs_imp)(Implement *) = reinterpret_cast<void (**)(Implement *)>(pvptr_imp);
	for(int i=0;i<6;i++)
		(*pvfuncs_imp[i])(&imp);


	printf("\n[Inside virtual function table of  static_cast<Interface2*>(&imp)]:\n");
	Interface2 * pinf2 = static_cast<Interface2*>(&imp);
	void * pvptr_inf2 = *(reinterpret_cast<void **>(pinf2));
	void (**pvfuncs_inf2)(Interface2 *) = reinterpret_cast<void (**)(Interface2 *)>(pvptr_inf2);
	for(int i=0;i<3;i++)
		(*pvfuncs_inf2[i])(pinf2);

}

class Foo2: public Foo
{
public:
	Foo2(const char * name): Foo(name){
		foo2_data = 1;
	}
	int foo2_data;
};

void test_typeid()
{
	SHOW_MY_FLAG();

	Foo  bar("bar");
	Foo2 bar2("bar2");
	Foo *pbarA = &bar;
	Foo *pbarB = &bar2;

	//we can see how virtual table is arranged by dissembly
	void **vptrA = *(reinterpret_cast<void ***>(pbarA));
	std::type_info * ptiA = reinterpret_cast<std::type_info *>(vptrA[-1]);
	void **vptrB = *(reinterpret_cast<void ***>(pbarB));
	std::type_info * ptiB = reinterpret_cast<std::type_info *>(vptrB[-1]);

	const char * pbarA_type = typeid(*pbarA).name();
	printf("(*pbarA)'s type is %s (%s)\n", pbarA_type, ptiA->name());

	const char * pbarB_type = typeid(*pbarB).name();
	printf("(*pbarB)'s type is %s (%s)\n", pbarB_type, ptiB->name());


	// 92               	const char * pbarA_type = typeid(*pbarA).name();
	//00000000004012c8:   mov -0x60(%rbp),%rax
	//00000000004012cc:   test %rax,%rax
	//00000000004012cf:   je 0x401305 <test_typeid()+154>
	//00000000004012d1:   mov (%rax),%rax                    the first pointer in object memory-layout, the vptr
	//00000000004012d4:   mov -0x8(%rax),%rax                vptr[-1] is pointing/address-of type_info object
	//00000000004012d8:   mov %rax,%rdi                      %rdi is the this pointer
	//00000000004012db:   callq 0x4019ec <std::type_info::name() const>

}

//=======================================================================================
class A
{
public:
	virtual void vfunc(void){};//just to make dynamic_cast() work
	int a0=0xa0;
	int a1=0xa1;
};

class B1: virtual public A
{
public:
	int b10=0xb10;
	int b11=0xb11;
};
class B2: virtual public A
{
public:
	int b20=0xb20;
	int b21=0xb21;
};
class B3: virtual  public A
{
public:
	int b30=0xb30;
	int b31=0xb31;
};
class B4: public A
{
public:
	int a0 = 0xba0;
	int b40=0xb40;
	int b41=0xb41;
};


class C: public B1, public B2, public B3
{
public:
	int c0=0xc0;
	int c1=0xc1;
};



class D
{
public:
	int d0=0xd0;
};
class E1: public D
{
public:
	int e1=0xe1;
};
class E2: public D
{
public:
	int e2=0xe2;
};
class F:public E1, public E2
{
public:
	int f0=0xf0;
};


void test_virtual_base(void)
{
	SHOW_MY_FLAG();

	B4 b4;
	int * p;

	printf("sizeof(void*)=%lu\n", sizeof(void*));


	printf("class B4: public A\n");
	printf("B4 b4;\n");
	p = reinterpret_cast<int*>(&b4);
	for(int k=0;k<sizeof(b4)/sizeof(int);k++)
	{
		printf("\tp[%d]=0x%8X",k, p[k]);
		SHOW_POS(b4,B4);
		SHOW_POS(b4,A);
		SHOW_POS(b4.a0,int);
		SHOW_POS(b4.A::a0,int);
		printf("\n");
	}

	printf("class B1: virtual public A\n");
	printf("B1 b1i;\n");
	B1 b1i, b1j;
	p = reinterpret_cast<int*>(&b1i);
	for(int k=0;k<sizeof(b1i)/sizeof(int);k++)
	{
		printf("\tp[%d]=0x%8X",k, p[k]);
		SHOW_POS(b1i,B1);
		SHOW_POS(b1i,A);
		printf("\n");
	}
	printf("B1 b1j;\n");
	p = reinterpret_cast<int*>(&b1j);
	for(int k=0;k<sizeof(b1j)/sizeof(int);k++)
	{
		printf("\tp[%d]=0x%8X",k, p[k]);
		SHOW_POS(b1j,B1);
		SHOW_POS(b1j,A);
		printf("\n");
	}

	printf("***---------------------------------------------------------------------\n");
	printf("*** Note, the vptr of B1 is same for b1i and b1j, but it will change when appear in a derived class\n");
	printf("***    This is because vptr contains memory layout sensitive part\n");
	printf("***---------------------------------------------------------------------\n");

	printf("class C: public B1, public B2, public B3\n");
	printf("C c;\n");
	C c;
	p = reinterpret_cast<int*>(&c);
	for(int k=0;k<sizeof(c)/sizeof(int);k++)
	{
		printf("\tp[%d]=0x%8X",k, p[k]);

		SHOW_POS(c,C);
		SHOW_POS(c,B1);
		SHOW_POS(c,B2);
		SHOW_POS(c,B3);
		SHOW_POS(c,A);

		printf("\n");
	}

	printf("class F: public E1, public E2\n");
	printf("F objf;\n");
	F objf;
	p = reinterpret_cast<int*>(&objf);
	for(int k=0;k<sizeof(F)/sizeof(int);k++)
	{
		printf("\tp[%d]=0x%8X",k, p[k]);
		SHOW_POS(objf,F);
		SHOW_POS(objf,E1);
		SHOW_POS(objf,E2);

		// SHOW_POS(objf,D);   		compile error: ambiguous base
		// SHOW_POS(objf,E1::D);   	compile error: ambiguous base
		// SHOW_POS(objf,F::E1::D); compile error: ambiguous base

		//have to do it in 2 stages
		SHOW_POS((static_cast<E1&>(objf)),D);
		SHOW_POS((static_cast<E2&>(objf)),D);



		printf("\n");
	}

	printf("Consider this piece of code:\n"
		   "    void func(B1 * pb1){\n"
		   "    pb1->a0 = 0x1234;\n"
		   "}\n");
	printf("What compiler shall do to ensure correctness in both following cases:\n"
			" 1.pb1 pointing to c.\n"
			" 2.pb1 pointing to b1.\n");

	B2 b2;
	C *  pc1 = static_cast<C*>(&c);

	//from dissamble we saw no vptr access needed
	B2 * pb2a = static_cast<B2*>(pc1);
	B2 * pb2b = dynamic_cast<B2*>(pc1);
	B2 * pbre = reinterpret_cast<B2*>(pc1);

	//virtual table access involved for both static&dynamic cast
	A * pa1 = static_cast<A*>(pb2a);
	A * pa2 = dynamic_cast<A*>(pb2a);

	//downcast:base object ptr ==> Derived object ptr
	C * pc1_backB = static_cast<C*>(pb2b);
	//C * pc1_backA = dynamic_cast<C*>(pa1); //cannot downcast from virtual base!!!!

	printf("====[Up/Down cast]======================================\n");
	printf("B2 b2                                   @%p\n",&b2);
	printf("C c                                     @%p\n",&c);
	printf("C * pc1=static_cast<C*>(&c);            @%p\n",pc1);
	printf("B2 * pb2a = static_cast<B2*>(pc1);      @%p\n",pb2a);
	printf("B2 * pb2b = dynamic_cast<B2*>(pc1);     @%p\n",pb2b);
	printf("B2 * pbre = reinterpret_cast<B2*>(pc1); @%p\n",pbre);
	printf("A * pa1 = static_cast<A*>(pb2a);        @%p\n",pa1);
	printf("A * pa2 = dynamic_cast<A*>(pb2a);       @%p\n",pa2);
	printf("C * pc1_backB = static_cast<C*>(pb2a);  @%p Downcast\n",pc1_backB);
	printf("static_cast<C*>(&b2);                   @%p Invalid Downcast would success for static_cast\n",static_cast<C*>(&b2));
	printf("dynamic_cast<C*>(&b2);                  @%p Invalid Downcast will be recognized by dynamic_cast\n",dynamic_cast<C*>(&b2));

	printf("C * pc1_backA = static_cast<C*>(pa1);   @??? Error:cannot downcast from virtual base!!!!\n");

	//reinterpret_cast did nothing but just change the signature/semantics of the address.
	//programer should be responsible for the validation
	A * pa0 = reinterpret_cast<A*>(pb2a);



	const char * pb2a_name = typeid(pb2a).name();
	const char * pc1_name = typeid(pc1).name();
	pb2a->a0 = 0xABCD;
	pc1->b20 = 0x1234;
	pc1->a0 = 0x5678;

	//A * pa1 = get_vtbl(pb1, -3);

	//
	//238               	pc1->a0 = 0x5678;
	//0000000000401a41:   mov -0xb8(%rbp),%rax
	//0000000000401a48:   mov (%rax),%rax
	//0000000000401a4b:   sub $0x18,%rax
	//0000000000401a4f:   mov (%rax),%rax
	//0000000000401a52:   mov %rax,%rdx
	//0000000000401a55:   mov -0xb8(%rbp),%rax
	//0000000000401a5c:   add %rdx,%rax
	//0000000000401a5f:   movl $0x5678,(%rax)
}





int main()
{
	test_virtual_func();
	test_typeid();
	test_virtual_base();
}
