#include "game/state/rules/scenery_tile_type.h"
#include "game/state/city/city.h"
#include "game/state/gamestate.h"

namespace OpenApoc
{

sp<SceneryTileType> SceneryTileType::get(const GameState &state, const UString &id)
{
	for (auto &pair : state.cities)
	{
		auto it = pair.second->tile_types.find(id);
		if (it != pair.second->tile_types.end())
			return it->second;
	}
	LogError("No scenery tile type matching ID \"%s\"", id);
	return nullptr;
}

const UString &SceneryTileType::getPrefix()
{
	static UString prefix = "CITYTILE_";
	return prefix;
}
const UString &SceneryTileType::getTypeName()
{
	static UString name = "SceneryTileType";
	return name;
}

} // namespace OpenApoc
