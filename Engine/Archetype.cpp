#include "Archetype.h"
#include <Windows.h>
#include <memoryapi.h>

using namespace Engine::Archetype;

Chunk::Chunk():
	data{ (char*)(VirtualAlloc(NULL, MAX_CHUNK_SIZE, MEM_RESERVE, PAGE_READWRITE)) }
{
	assert(data);
}

Chunk::~Chunk()
{
	VirtualFree(data, 0, MEM_RELEASE);
}
