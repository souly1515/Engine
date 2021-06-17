#pragma once
#include "Archetype.h"
#include <memory>

namespace Engine
{
	namespace Entity_details
	{
		struct EntityInfo
		{
			std::shared_ptr<Archetype::Archetype> archetype;
			Archetype::ChunkIndex index;
			// stores generation and my index
			Entity ent;
		};
	}
}

