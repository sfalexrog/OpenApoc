#include "framework/framework.h"
#include "game/state/gamestate.h"
#include "game/state/ufopaedia.h"
#include "library/strings_format.h"
#include "tools/extractors/common/ufo2p.h"
#include "tools/extractors/extractors.h"

namespace OpenApoc
{

void InitialGameStateExtractor::extractOrganisations(GameState &state) const
{
	auto &data = this->ufo2p;
	LogInfo("Number of org strings: %zu", data.organisation_names->readStrings.size());
	LogInfo("Number of orgs: %u", (unsigned)data.organisation_data->count());

	// Organisations

	for (unsigned i = 0; i < data.organisation_data->count(); i++)
	{
		auto o = mksp<Organisation>();

		auto odata = data.organisation_data->get(i);
		auto rdata = data.organisation_starting_relationships_data->get(i);

		UString id = data.getOrgId(i);

		o->name = data.organisation_names->get(i);
		o->id = id;

		auto ped = format("%s%s", UfopaediaEntry::getPrefix(),
		                  canon_string(data.organisation_names->get(i)));
		o->ufopaedia_entry = {&state, ped};

		o->balance = odata.starting_funds;
		o->income = odata.starting_funding;
		o->tech_level = odata.starting_tech_level + 1;
		o->average_guards = odata.average_guards;

		// "Civilian" organisation has no loot entry
		if (i == data.organisation_data->count() - 1)
		{
			for (int k = 0; k < 3; k++)
			{
				Organisation::LootPriority priority;
				switch (k)
				{
					case 0:
						priority = Organisation::LootPriority::A;
						break;
					case 1:
						priority = Organisation::LootPriority::B;
						break;
					case 2:
						priority = Organisation::LootPriority::C;
						break;
				}
				for (int j = 0; j < 5; j++)
				{
					o->loot[priority].push_back(nullptr);
				}
			}
		}
		else
		{
			auto ldata = data.organisation_raid_loot_data->get(i);

			for (int k = 0; k < 3; k++)
			{
				Organisation::LootPriority priority;
				switch (k)
				{
					case 0:
						priority = Organisation::LootPriority::A;
						break;
					case 1:
						priority = Organisation::LootPriority::B;
						break;
					case 2:
						priority = Organisation::LootPriority::C;
						break;
				}
				for (int j = 0; j < 5; j++)
				{
					if (ldata.loot_idx[k][j] == 0)
					{
						o->loot[priority].push_back(nullptr);
					}
					else
					{
						o->loot[priority].emplace_back(
						    &state, format("%s%s", AEquipmentType::getPrefix(),
						                   canon_string(data.agent_equipment_names->get(
						                       ldata.loot_idx[k][j]))));
					}
				}
			}
		}

		// infiltration
		auto idata = data.infiltration_speed_org->get(i);
		o->infiltrationSpeed = idata.speed;

		// relationship
		for (int j = 0; j < 28; j++)
		{
			StateRef<Organisation> o2 = {&state, data.getOrgId(j)};

			o->current_relations[o2] = (float)rdata.relationships[j];
		}

		state.organisations[id] = o;
	}
	state.player = {&state, "ORG_X-COM"};
	state.aliens = {&state, "ORG_ALIEN"};
	state.civilian = {&state, "ORG_CIVILIAN"};
}

} // namespace OpenApoc
