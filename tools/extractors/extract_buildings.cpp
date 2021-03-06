#include "framework/data.h"
#include "framework/framework.h"
#include "game/state/battle/battlemap.h"
#include "game/state/city/building.h"
#include "game/state/city/city.h"
#include "game/state/gamestate.h"
#include "game/state/ufopaedia.h"
#include "library/strings_format.h"
#include "tools/extractors/common/ufo2p.h"
#include "tools/extractors/extractors.h"

namespace OpenApoc
{

void InitialGameStateExtractor::extractBuildingFunctions(GameState &state) const
{
	auto &data = this->ufo2p;

	for (unsigned i = 0; i < data.building_functions->count(); i++)
	{
		auto f = mksp<BuildingFunction>();
		f->name = data.building_functions->get(i);
		if (i < data.infiltration_speed_building->count())
		{
			f->infiltrationSpeed = data.infiltration_speed_building->get(i).speed;
		}
		if (i < buildingFunctionDetectionWeights.size())
		{
			f->detectionWeight = buildingFunctionDetectionWeights[i];
		}
		auto id = format("%s%s", BuildingFunction::getPrefix(), canon_string(f->name));
		auto ped = format("%s%s", UfopaediaEntry::getPrefix(), canon_string(f->name));
		f->ufopaedia_entry = {&state, ped};
		state.building_functions[id] = f;
	}
}

void InitialGameStateExtractor::extractBuildings(GameState &state, UString bldFileName,
                                                 sp<City> city, bool alienBuilding) const
{
	auto &data = this->ufo2p;

	auto fileName = "xcom3/ufodata/" + bldFileName + ".bld";

	auto inFile = fw().data->fs.open(fileName);
	if (!inFile)
	{
		LogError("Failed to open \"%s\"", fileName);
	}
	auto fileSize = inFile.size();
	auto bldCount = fileSize / sizeof(struct BldFileEntry);

	LogInfo("Loading %lu buildings from %s", (unsigned long)bldCount, fileName);

	for (unsigned i = 0; i < bldCount; i++)
	{
		struct BldFileEntry entry;
		inFile.read((char *)&entry, sizeof(entry));

		auto b = mksp<Building>();
		if (alienBuilding)
		{
			b->name = data.alien_building_names->get(entry.function_idx);
			b->function = {&state,
			               format("%s%s", BuildingFunction::getPrefix(), canon_string(b->name))};
			LogInfo("Alien bld %d %s func %d %s", entry.name_idx, b->name, entry.function_idx,
			        b->function.id);

			// Load crew
			auto crew = ufo2p.crew_alien_building->get(entry.function_idx);
			UFO2P::fillCrew(state, crew, b->preset_crew);
		}
		else
		{
			b->name = data.building_names->get(entry.name_idx);
			b->function = {&state,
			               format("%s%s", BuildingFunction::getPrefix(),
			                      canon_string(data.building_functions->get(entry.function_idx)))};
		}
		int battle_map_index = entry.function_idx - 1 + (alienBuilding ? 39 : 0);
		// Fix battle index for buildings that use other maps
		switch (battle_map_index)
		{
			// 24 Car Factory uses 25 Flying Car Factory
			case 23:
				battle_map_index = 24;
				break;
			// 13 Car Park is not used in vanilla
			// Still, we should provide a reasonable substitute
			case 12:
				battle_map_index = 10; // 11 Procreation Park
				break;
			// 16 Hotel is not used in vanilla
			// Still, we should provide a reasonable substitute
			case 15:
				battle_map_index = 14; // 15 Luxury Appartments
				break;
			// 17 Atmosphere Processor is not used in vanilla
			// Still, we should provide a reasonable substitute
			case 16:
				battle_map_index = 19; // 20 Water Purifier
				break;
			// 29 Ruins is not used in vanilla
			// Still, we should provide a reasonable substitute
			case 28:
				battle_map_index = 10; // 11 Procreation Park
				break;
				// 31 Space Ship is not used in vanilla
				// Still, we should provide a reasonable substitutecase 31:
				battle_map_index = 7; // 08 Space Port
				break;
			// 34 Outdoor Parks is not used in vanilla
			// Still, we should provide a reasonable substitute
			case 33:
				battle_map_index = 10; // 11 Procreation Park
				break;
			default:
				break;
		}
		b->battle_map = {
		    &state, format("%s%s", BattleMap::getPrefix(), this->battleMapPaths[battle_map_index])};
		b->owner = {&state, data.getOrgId(entry.owner_idx)};
		// Our rects are exclusive of p2
		// Shift position by 20 tiles
		b->bounds = {entry.x0 + 20, entry.y0 + 20, entry.x1 + 21, entry.y1 + 21};
		auto id = format("%s%s", Building::getPrefix(), canon_string(b->name));

		city->buildings[id] = b;
	}
}

} // namespace OpenApoc
