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
	for (int i = 0; i < 20; ++i)
	{
		if(i % 2)
			entity[i] = engineMan.CreateEntity<B, A, C>();
		else
			entity[i] = engineMan.CreateEntity<B, A, C, D>();
	}

	for(int i = 0; i < 20; ++i)
	{
		A& a = engineMan.GetComponent<A>(entity[i]);
		B& b = engineMan.GetComponent<B>(entity[i]);
		C& c = engineMan.GetComponent<C>(entity[i]);
		D* d = engineMan.TryGetComponent<D>(entity[i]);

		a.a = i;
		a.b = 20 - i;
		
		b.a = i;
		b.b = 20 - i;
		b.c = i * 0.5f;
		b.d = i;

		c.a = i * 0.5f;
		c.b = i;
		
		for (int j = 0; j < 200; ++j)
		{
			if(d)
				d->a[j] = j + i;
		}
	}

	//engineMan.DeleteEntity(entity[4]);
	//engineMan.DeleteEntity(entity[5]);
	//engineMan.DeleteEntity(entity[10]);

	engineMan.RunSystemOnce();

	for (int i = 0; i < 20; ++i)
	{
		A& a = engineMan.GetComponent<A>(entity[i]);
		B& b = engineMan.GetComponent<B>(entity[i]);
		C& c = engineMan.GetComponent<C>(entity[i]);
		D* d = engineMan.TryGetComponent<D>(entity[i]);
		std::cout << "entity " << i << " A: " << a.a << " " << a.b << std::endl;
		std::cout << "entity " << i << " B: " << int(b.a) << " " << int(b.b) << " " << b.c << " " << b.d << std::endl;
		std::cout << "entity " << i << " C: " << c.a << " " << c.b << std::endl;
		if(d)
			std::cout << "entity " << i << " D: " << d->a[0] << " " << d->a[13] << " " << d->a[199] << std::endl;
		std::cout << std::endl;
	}


 
	return 0;
} 
