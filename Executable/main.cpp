#include <iostream>
#include <Windows.h>
#include "MetaHelpers.h"
#include "EngineManager.h"

struct A
{
	int a;
	int b;
};
struct B
{
	char a;
	char b;
	float c;
	int d;
};
struct C
{
	double a;
	int b;
};
struct D
{
	int a[200];
};

struct Sys1
{
	int count = 0;
	void operator()(A& a, B& b)
	{
		std::cout << "sys1 " << ++count << std::endl;
		a.a += 1;
		b.d += 1;
	}
};

struct Sys2
{
	int count = 0;
	void operator()(B& b, C& c)
	{
		std::cout << "sys2 " << ++count << std::endl;
		b.a += 1;
		c.b += 1;
	}
};

struct Sys3
{
	int count = 0;
	void operator()(B& b, D& d)
	{
		std::cout << "sys3 " << ++count << std::endl;
		b.a += 1;
		d.a[0] += 1;
	}
};
struct Sys4
{
	int count = 0;
	void operator()()
	{
		std::cout << "sys4 " << ++count << std::endl;
	}
};
struct Sys5
{
	void Execute(Engine::EntityManager::EntityManager&)
	{
		std::cout << "custom execute" << std::endl;
	}
};
#include <set>

int main(int argc, char* argv[])
{
	std::cout << "Engine Start" << std::endl;
	Engine::EngineManager engineMan;


	engineMan.RegisterComponent<A>();
	engineMan.RegisterComponent<B>();
	engineMan.RegisterComponent<C>();
	engineMan.RegisterComponent<D>();

	engineMan.RegisterSystem<Sys1>();
	engineMan.RegisterSystem<Sys2>();
	engineMan.RegisterSystem<Sys3>();
	engineMan.RegisterSystem<Sys4>();
	engineMan.RegisterSystem<Sys5>();

	Entity entity[20];
	std::set<Entity> test;

	for (int i = 0; i < 20; ++i)
	{
		if(i % 2)
			entity[i] = engineMan.CreateEntity<B, A, C>();
		else
			entity[i] = engineMan.CreateEntity<B, A, C, D>();
		test.insert(entity[i]);
	}

	B type1_B[20];
	B type2_B[20];
	A type1_A[20];
	A type2_A[20];
	C type1_C[20];
	C type2_C[20];
	D type2_D[20];
	B* Bs[2] = { type1_B, type2_B };
	A* As[2] = { type1_A, type2_A };
	C* Cs[2] = { type1_C, type2_C };

	for(int i = 0; i < 20; ++i)
	{
		A& a = engineMan.GetComponent<A>(entity[i]);
		B& b = engineMan.GetComponent<B>(entity[i]);
		C& c = engineMan.GetComponent<C>(entity[i]);
		D* d = engineMan.TryGetComponent<D>(entity[i]);
		int index = i % 2;

		As[index][i].a = a.a = i;
		As[index][i].b = a.b = 20 - i;
		
		Bs[index][i].a = b.a = i;
		Bs[index][i].b = b.b = 20 - i;
		Bs[index][i].c = b.c = i * 0.5f;
		Bs[index][i].d = b.d = i;

		Cs[index][i].a = c.a = i * 0.5f;
		Cs[index][i].b = c.b = i;
		
		for (int j = 0; j < 200; ++j)
		{
			if (d)
			{
				type2_D[i].a[j] = d->a[j] = j + i;
			}
		}
	}

	//engineMan.DeleteEntity(entity[4]);
	//engineMan.DeleteEntity(entity[5]);
	//engineMan.DeleteEntity(entity[10]);

	engineMan.RunSystemOnce();
	{
		Sys1 s1;
		Sys2 s2;
		Sys3 s3;
		for (int i = 0; i < 10; ++i)
		{
			s1(As[0][i * 2], Bs[0][i * 2]);
			s1(As[1][i * 2 + 1], Bs[1][i * 2 + 1]);

			s2(Bs[0][i * 2], Cs[0][i * 2]);
			s2(Bs[1][i * 2 + 1], Cs[1][i * 2 + 1]);

			s3(Bs[0][i * 2], type2_D[i * 2]);
		}
	}
	auto check = [&](int index)->bool {

		A& a = engineMan.GetComponent<A>(entity[index]);
		B& b = engineMan.GetComponent<B>(entity[index]);
		C& c = engineMan.GetComponent<C>(entity[index]);
		D* d = engineMan.TryGetComponent<D>(entity[index]);

		if (d)
		{
			for (int j = 0; j < 200; ++j)
			{
				if (d->a[j] != type2_D[index].a[j])
				{
					std::cout << "error with D " << index << " " << "a.a\n";
					return true;
				}
			}
		}
		if (a.a != As[index % 2][index].a)
		{
			std::cout << "error with A " << index << " " << "a.a\n";
			return true;
		}
		if (a.b != As[index % 2][index].b)
		{
			std::cout << "error with A " << index << " " << "a.b\n";
			return true;
		}
		if (b.a != Bs[index % 2][index].a)
		{
			std::cout << "error with B " << index << " " << "b.a\n";
			return true;
		}
		if (b.b != Bs[index % 2][index].b)
		{
			std::cout << "error with B " << index << " " << "b.b\n";
			return true;
		}
		if (b.c != Bs[index % 2][index].c)
		{
			std::cout << "error with B " << index << " " << "b.c\n";
			return true;
		}
		if (b.d != Bs[index % 2][index].d)
		{
			std::cout << "error with B " << index << " " << "b.d\n";
			return true;
		}

		return
			c.a != Cs[index % 2][index].a ||
			c.b != Cs[index % 2][index].b;
	};
	for (int i = 0; i < 20; ++i)
	{
		A& a = engineMan.GetComponent<A>(entity[i]);
		B& b = engineMan.GetComponent<B>(entity[i]);
		C& c = engineMan.GetComponent<C>(entity[i]);
		D* d = engineMan.TryGetComponent<D>(entity[i]);

		if (check(i))
		{
			std::cout << "entity " << i << " A: " << a.a << " " << a.b << std::endl;
			std::cout << "entity " << i << " B: " << int(b.a) << " " << int(b.b) << " " << b.c << " " << b.d << std::endl;
			std::cout << "entity " << i << " C: " << c.a << " " << c.b << std::endl;
			if (d)
				std::cout << "entity " << i << " D: " << d->a[0] << " " << d->a[13] << " " << d->a[199] << std::endl;
			std::cout << std::endl;
		}
	}


 
	return 0;
} 
