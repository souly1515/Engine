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
	void operator()(A& a, B& b)
	{
		a.a += 1;
		b.d += 2;
	}
};

struct Sys2
{
	void operator()(B& b, C& c)
	{
		b.a += 5;
		c.b += 23;
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

	Entity entity[20];
	for (int i = 0; i < 20; ++i)
	{
		entity[i] = engineMan.CreateEntity<B, A, C, D>();
	}

	for(int i = 0; i < 20; ++i)
	{
		A& a = engineMan.GetComponent<A>(entity[i]);
		B& b = engineMan.GetComponent<B>(entity[i]);
		C& c = engineMan.GetComponent<C>(entity[i]);
		D& d = engineMan.GetComponent<D>(entity[i]);

		a.a = i * 5;
		a.b = 20 - i * 2;
		
		b.a = i;
		b.b = 20 - i;
		b.c = i * 1.2f;
		b.d = i * 15;

		c.a = i * 2.2f;
		c.b = i * 12;
		
		for (int j = 0; j < 200; ++j)
		{
			d.a[j] = j + i;
		}
	}

	engineMan.RunSystems();

	for (int i = 0; i < 20; ++i)
	{
		A& a = engineMan.GetComponent<A>(entity[i]);
		B& b = engineMan.GetComponent<B>(entity[i]);
		C& c = engineMan.GetComponent<C>(entity[i]);
		D& d = engineMan.GetComponent<D>(entity[i]);
		std::cout << "entity " << i << " A: " << a.a << " " << a.b << std::endl;
		std::cout << "entity " << i << " B: " << int(b.a) << " " << int(b.b) << " " << b.c << " " << b.d << std::endl;
		std::cout << "entity " << i << " C: " << c.a << " " << c.b << std::endl;
		std::cout << "entity " << i << " D: " << d.a[0] << " " << d.a[13] << " " << d.a[199] << std::endl;
		std::cout << std::endl;
	}


 
	return 0;
} 
