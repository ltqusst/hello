/*
 * cpp11_objmodel1.cpp
 *
 *  Created on: Sep 22, 2017
 *      Author: tingqian
 */
#include <cstdio>


#define ANIMAL_VIRTUAL virtual
//#define ANIMAL_VIRTUAL

class Animal
{
public:

	ANIMAL_VIRTUAL void veat(void){printf("Animal veat()\n");}
	ANIMAL_VIRTUAL void veat1(void){printf("eat1 ?\n");}
	ANIMAL_VIRTUAL void veat2(void){printf("eat2 ?\n");}
	ANIMAL_VIRTUAL void veat3(void){printf("eat3 ?\n");}

	void eat(void){printf("Animal eat()\n");};

	Animal(const char * n){
		name = n;
		printf("\t>>>> ctor_Animal(%s)\n",name);
		sex = 0; age = 0;
	}
	~Animal(){
		printf("\t>>>> dector_Animal(%s)\n",name);
	}
	int get_sex(void){return sex;}
	int get_age(void){return age;}
	void set_sex(int i){sex = i;}
	void set_age(int i){age = i;}

	friend int main();

protected:
	int sex;
	int age;
	const char * name;
};



#define SHEEP_VIRTUAL virtual
//#define SHEEP_VIRTUAL

class Sheep:public Animal
{
public:
	Sheep(const char * n): Animal(n){
		printf("\t>>>> ctor_Sheep(%s)\n", name);
		type = 0;
	}
	~Sheep(){
		printf("\t>>>> dector_Sheep(%s)\n",name);
	}

	SHEEP_VIRTUAL void veat(void){printf("Sheep veat()\n");}
	SHEEP_VIRTUAL void eat1(void){printf("eat1 Grass\n");}
	SHEEP_VIRTUAL void eat2(void){printf("eat2 Fruit\n");}
	SHEEP_VIRTUAL void eat3(void){printf("eat3 Flower\n");}

	SHEEP_VIRTUAL void like(void){printf("like fly\n");}


	void eat(void){printf("Sheep eat()\n");};

	int get_sex(void){return sex;}
	int get_age(void){return age;}
	void set_sex(int i){sex = i;}
	void set_age(int i){age = i;}
	int get_type(void){return type;}
	void set_type(int i){type = i;}

	friend int main();

protected:
	int type;
};


int main()
{
	int * pi;
	printf(":::: Declare Animal a\n");
	Animal a("a");
	printf(":::: Declare Animal b\n");
	Animal b("b");
	printf(":::: Declare Shep s\n");
	Sheep s("s");

	Animal *ps = &s;
	Animal &rs = s;
	Animal *pupcast_s = static_cast<Animal *>(&s);
	Animal *pupcast_d = dynamic_cast<Animal *>(&s);

	Sheep  * pdowncast_s = static_cast<Sheep *>(ps);
	Sheep  * pdowncast_d = static_cast<Sheep *>(ps); //dynamic down cast is not allowed

	printf("\n========= experiment setup =====================\n");
	printf("Sheep s;           @(%p)\n", &s);
	printf("Animal *ps = &s;   @(%p)\n", ps);
	printf("Animal &rs =  s;   @(%p)\n", &rs);
	printf("Animal *pupcast_s = static_cast<Animal *>&s;    @(%p)\n", pupcast_s);
	printf("Animal *pupcast_d = dynamic_cast<Animal *>&s;   @(%p)\n", pupcast_d);
	printf("Sheep  *pdowncast_s = static_cast<Sheep *>ps;   @(%p)\n", pdowncast_s);
	printf("Sheep  *pdowncast_d = dynamic_cast<Sheep *>ps;  @(%p)\n", pdowncast_d);

	printf("\n");

	printf("========= memory map of derived class ==========\n");
	printf("sizeof   base class  Animal =%lu\t+.\t%p\n", sizeof(Animal),ps);
	if(&(((Animal*)(0))->sex))
	printf("                     .vptr?  %lu\t+0\n", &(((Animal*)(0))->sex));
	printf("                     .sex    %lu\t+%d\t%p\n", sizeof(Animal::sex), &(((Animal*)(0))->sex),&(ps->sex));
	printf("                     .age    %lu\t+%d\t%p\n", sizeof(Animal::age), &(((Animal*)(0))->age),&(ps->age));
	printf("                     .name   %lu\t+%d\t%p\n", sizeof(Animal::name), &(((Animal*)(0))->name),&(ps->name));
	printf("-------------------------------------\n");

	printf("sizeof derived class Sheep  =%lu\t+.\t%p\n", sizeof(Sheep), &s);
	if(&(((Sheep*)(0))->sex))
	printf("                     .vptr?  %lu\t+0\n", &(((Sheep*)(0))->sex));
	printf("                     .sex    %lu\t+%d\t%p\n", sizeof(Sheep::sex),&(((Sheep*)(0))->sex), &(s.sex));
	printf("                     .age    %lu\t+%d\t%p\n", sizeof(Sheep::age),&(((Sheep*)(0))->age), &(s.age));
	printf("                     .name   %lu\t+%d\t%p\n", sizeof(Sheep::name),&(((Sheep*)(0))->name), &(s.name));
	printf("                     .type   %lu\t+%d\t%p\n", sizeof(Sheep::type),&(((Sheep*)(0))->type), &(s.type));

	printf("\n========= virtual function call derived class ==========\n");
	printf("through original-class:\n");
	printf("    s.eat():"); s.eat();
	printf("    s.veat():"); s.veat();
	printf("through base-class pointer:\n");
	printf("    ps->eat():"); ps->eat();
	printf("    ps->veat():"); ps->veat();
	printf("\n");

	int (Animal::*func)(void) = &Animal::get_age;

	//no way to access address of function 'a.get_age()'
	//so we know the member function is belong to class only
	//and we have no way to use the funcion
	printf("Animal::get_age=%p, func=%p\n",&Animal::get_age, &func);

}
