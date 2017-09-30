#include <stdio.h>
#include <vector>
#include <initializer_list>
#include <typeinfo>
#include <cmath>



class localv{
public:
	localv(const char * name){
		id = name;
		printf("\tctor(%s):v\n", id);
	}
	~localv(){
		printf("\tdetor(%s):v\n",id);
	}
	const char * id;
};

template<class _E>
class myset
{
public:
    typedef _E 			value_type;
    typedef _E& 		reference;
    typedef const _E& 	const_reference;
    typedef size_t 		size_type;
    typedef _E* 		iterator;
    typedef const _E* 	const_iterator;

	myset(std::initializer_list<value_type> vals){
		int i=0;
		for(auto v:vals)
		{
			data[i] = v;
			i++;
		}
		cnt = i;
		printf("myset({}) is called\n");
	}
	myset(int a){
		cnt=1;
		data[0] = a;
		printf("myset(int a) is called\n");
	}
	myset(double d){
		cnt=1;
		data[0] = d;
		printf("myset(double d) is called\n");
	}
	~myset(){
		printf("detor of myset is called\n");
	}

	int do_something(int a, int b) noexcept
	{
		printf("do_something() is doing %d / %d\n", a,b);
		return a/b;
	}

	iterator begin(void){return &data[0];}
	iterator end(void){return &data[cnt];}

	void show(void){
		printf("%s's data:", typeid(this).name());
		for(int i=0;i<cnt;i++)
		{
			printf("%d ", data[i]);
		}
		printf("\n");
	}
private:
	value_type 	data[10];
	size_type 	cnt;
};


// returned value can be used at compile-time
constexpr int Olog2(int n){
	return log2(n);
}
static int arr[Olog2(1028)]{};





int main(int argc, char * argv[])
{
	std::vector<int> val={1,2,3,4,5};
	//int val[10]{};//{1,2,3,4,5};

	auto p = [](const char *name){
			localv v(name);
			auto len_expected = Olog2(1028);
			printf("\tnumof element of arr=%lu, as expected %d\n",
							sizeof(arr)/sizeof(arr[0]), len_expected);
			return 44;
	};

	printf("lambda expression p start\n");
	printf("lambda expression p end(return %d)\n\n", p("LambdaABC"));


	decltype(val)::value_type val_item;
	auto enumprint=[&]{
		printf("%d,", val_item);
	};

	printf("sizeof(val)=%lu, val=", sizeof(val));
	for(auto &v:val) v*=2;
	for(auto v:val) {
		val_item = v;
		enumprint();
	}
	printf("\n");

	myset<int> m1(1.0);
	myset<int> m2(1);
	//myset<int> m3{1.0}; //compile error
	myset<int> m4{1};

	myset<int> m{5,4,3,2,1};
	m.show();


	m.do_something(2,0);

	for(auto &p: m) p++;
	for(auto p: m)
	{
		decltype(m)::value_type mv = p*100;
		printf("%d,", mv);
	}
	printf("\n");

}

