#include "framework/framework.h"
#include "game/state/agent.h"
#include "game/state/rules/aequipment_type.h"
#include "game/state/rules/damage.h"
#include "tools/extractors/common/tacp.h"
#include "tools/extractors/extractors.h"
#include <limits>

namespace OpenApoc
{

void InitialGameStateExtractor::extractAgentEquipment(GameState &state, Difficulty difficulty)
{
	auto &data_t = this->tacp;
	auto &data_u = this->ufo2p;

	auto gameObjectSpriteTabFileName = UString("xcom3/tacdata/gameobj.tab");
	auto gameObjectSpriteTabFile = fw().data->fs.open(gameObjectSpriteTabFileName);
	if (!gameObjectSpriteTabFile)
	{
		LogError("Failed to open dropped item sprite TAB file \"%s\"",
		         gameObjectSpriteTabFileName.cStr());
		return;
	}
	size_t gameObjectSpriteCount = gameObjectSpriteTabFile.size() / 4;

	auto heldSpriteTabFileName = UString("xcom3/tacdata/unit/equip.tab");
	auto heldSpriteTabFile = fw().data->fs.open(heldSpriteTabFileName);
	if (!heldSpriteTabFile)
	{
		LogError("Failed to open held item sprite TAB file \"%s\"", heldSpriteTabFileName.cStr());
		return;
	}
	// size_t heldSpriteCount = heldSpriteTabFile.size() / 4 / 8;

	std::map<int, sp<AEquipmentType>> weapons;
	UString tracker_gun_clip_id = "";

	for (unsigned i = 0; i < data_t.damage_type_names->count(); i++)
	{
		auto d = mksp<DamageType>();

		d->name = data_t.damage_type_names->get(i);
		UString id = data_t.getDTypeId(i);

		d->id = id;

		d->ignore_shield =
		    (i < data_t.damage_types->count()) && (data_t.damage_types->get(i).ignore_shield == 1);

		// FIXME: Read damage type icons

		state.damage_types[id] = d;
	}

	for (unsigned i = 0; i < data_t.damage_modifiers->count(); i++)
	{
		auto d = mksp<DamageModifier>();
		auto ddata = data_t.damage_modifiers->get(i);

		d->name = data_t.damage_modifier_names->get(i);
		UString id = data_t.getDModId(i);

		d->id = id;

		state.damage_modifiers[id] = d;

		for (unsigned j = 0; j < 18; j++)
		{
			state.damage_types[data_t.getDTypeId(j)]->modifiers[{&state, id}] =
			    ddata.damage_type_data[j];
		}
	}

	for (unsigned i = 0; i < data_t.agent_equipment->count(); i++)
	{
		auto e = mksp<AEquipmentType>();
		auto edata = data_t.agent_equipment->get(i);

		// Since we are reading strings from UFO2P.EXE, we'll be missing the last row
		// used for a bogus "Elerium Pod" duplicate
		// Fix this here for now, in future maybe remove alltogether as i think it's unused
		if (i == data_t.agent_equipment->count() - 1)
			e->name = data_u.agent_equipment_names->get(i - 2);
		else
			e->name = data_u.agent_equipment_names->get(i);
		UString id = UString::format("%s%s", AEquipmentType::getPrefix(), canon_string(e->name));

		e->id = id;

		unsigned payload_idx = std::numeric_limits<unsigned>::max();
		switch (edata.type)
		{
			case AGENT_EQUIPMENT_TYPE_ARMOR:
			{
				auto adata = data_t.agent_armor->get(edata.data_idx);
				e->type = AEquipmentType::Type::Armor;
				e->damage_modifier = {&state, data_t.getDModId(adata.damage_modifier)};
				e->armor = adata.armor;
				switch (adata.body_part)
				{
					case AGENT_ARMOR_BODY_PART_LEGS:
						e->body_part = AgentType::BodyPart::Legs;
						break;
					case AGENT_ARMOR_BODY_PART_BODY:
						e->body_part = AgentType::BodyPart::Body;
						// Vanilla decides if flight is enabled by checking if a "marsec" damage mod
						// body armor is equipped
						if (adata.damage_modifier == 18)
							e->provides_flight = true;
						break;
					case AGENT_ARMOR_BODY_PART_LEFT_ARM:
						e->body_part = AgentType::BodyPart::LeftArm;
						break;
					case AGENT_ARMOR_BODY_PART_RIGHT_ARM:
						e->body_part = AgentType::BodyPart::RightArm;
						break;
					case AGENT_ARMOR_BODY_PART_HELMET:
						e->body_part = AgentType::BodyPart::Helmet;
						break;
					default:
						LogError("Unexpected body part type %d for ID %s", (int)adata.body_part,
						         id.cStr());
				}
			}
			break;
			case AGENT_EQUIPMENT_TYPE_WEAPON:
			{
				auto wdata = data_t.agent_weapons->get(edata.data_idx);
				if (wdata.ammo_effect[0] == 255)
				{
					e->type = AEquipmentType::Type::Grenade;
					payload_idx = wdata.grenade_effect;
					e->max_ammo = 1;
					e->recharge = 0;
				}
				else
				{
					e->type = AEquipmentType::Type::Weapon;
					bool has_ammo = false;
					// Entry #0 is "EMPTY" so we skip it
					for (unsigned j = 1; j < data_t.agent_general->count(); j++)
					{
						if (has_ammo)
							continue;
						auto gdata = data_t.agent_general->get(j);
						// Fix for buggy tracer gun ammo
						if (gdata.ammo_type == 0xffff && gdata.ammo_type_duplicate == 1)
						{
							if (wdata.ammo_type == 8)
								has_ammo = true;
							continue;
						}
						// Look for a clip with matching ammo type
						if ((gdata.ammo_type != 0xffff
						         ? gdata.ammo_type
						         : gdata.ammo_type_duplicate) == wdata.ammo_type)
							has_ammo = true;
					}

					if (has_ammo)
					{
						// All proper weapons have ammo types defined after them, so we can add ammo
						// when we parse it
						weapons[wdata.ammo_type] = e;
					}
					else
					{
						payload_idx = wdata.ammo_effect[0];
						e->max_ammo = wdata.ammo_rounds[0];
						e->recharge = wdata.ammo_recharge[0];
					}
				}
			}
			break;
			case AGENT_EQUIPMENT_TYPE_GENERAL:
			{
				auto gdata = data_t.agent_general->get(edata.data_idx);
				switch (gdata.type)
				{
					case AGENT_GENERAL_TYPE_AMMO_OR_LOOT:
						if (gdata.ammo_effect == 255)
						{
							e->type = AEquipmentType::Type::Loot;
						}
						else
						{
							e->type = AEquipmentType::Type::Ammo;
							payload_idx = gdata.ammo_effect;
							e->max_ammo = gdata.ammo_rounds;
							e->recharge = gdata.ammo_recharge;
							if (gdata.ammo_type == 65535 && gdata.ammo_type_duplicate == 1)
								// Buggy tracker gun clip has incorrect values in these fields AND
								// comes before it's weapon!
								tracker_gun_clip_id = id;
							else
								// Behave normally
								weapons[gdata.ammo_type != 0xffff ? gdata.ammo_type
								                                  : gdata.ammo_type_duplicate]
								    ->ammo_types.emplace(&state, id);
						}
						break;
					case AGENT_GENERAL_TYPE_MOTION_SCANNER:
						e->type = AEquipmentType::Type::MotionScanner;
						break;
					case AGENT_GENERAL_TYPE_STRUCTURE_PROBE:
						e->type = AEquipmentType::Type::StructureProbe;
						break;
					case AGENT_GENERAL_TYPE_VORTEX_ANALYZER:
						e->type = AEquipmentType::Type::VortexAnalyzer;
						break;
					case AGENT_GENERAL_TYPE_MULTI_TRACKER:
						e->type = AEquipmentType::Type::MultiTracker;
						break;
					case AGENT_GENERAL_TYPE_MIND_SHIELD:
						e->type = AEquipmentType::Type::MindShield;
						break;
					case AGENT_GENERAL_TYPE_MIND_BENDER:
						e->type = AEquipmentType::Type::MindBender;
						break;
					case AGENT_GENERAL_TYPE_ALIEN_DETECTOR:
						e->type = AEquipmentType::Type::AlienDetector;
						break;
					case AGENT_GENERAL_TYPE_DISRUPTOR_SHIELD:
						e->type = AEquipmentType::Type::DisruptorShield;
						e->max_ammo = 100;
						e->recharge = 1;
						break;
					case AGENT_GENERAL_TYPE_TELEPORTER:
						e->type = AEquipmentType::Type::Teleporter;
						e->max_ammo = gdata.ammo_rounds;
						e->recharge = gdata.ammo_recharge;
						break;
					case AGENT_GENERAL_TYPE_CLOAKING_FIELD:
						e->type = AEquipmentType::Type::CloakingField;
						break;
					case AGENT_GENERAL_TYPE_DIMENSION_FORCE_FIELD:
						e->type = AEquipmentType::Type::DimensionForceField;
						break;
					case AGENT_GENERAL_TYPE_MEDI_KIT:
						e->type = AEquipmentType::Type::MediKit;
						break;
					default:
						LogError("Unexpected general type %d for ID %s", (int)gdata.type,
						         id.cStr());
				}
			}
			break;
			default:
				LogWarning("Encountered empty item in ID %s, moving on", id.cStr());
				continue;
		}

		e->weight = edata.weight;

		if (i < gameObjectSpriteCount)
			e->dropped_sprite =
			    fw().data->loadImage(UString::format("PCK:xcom3/tacdata/gameobj.pck:xcom3/tacdata/"
			                                         "gameobj.tab:%d:xcom3/tacdata/tactical.pal",
			                                         i));

		// Held sprites begin from 0, which corresponds to item 1, Megapol AP Grenade
		// Armor pieces go last, and held sprites for every single item after the first armor piece
		// are identical
		// There is a total 60 of them
		int held_sprite_index = std::min((int)i - 1, 59);
		for (int j = 0; j < 8; j++)
			e->held_sprites.push_back(fw().data->loadImage(
			    UString::format("PCK:xcom3/tacdata/unit/equip.pck:xcom3/tacdata/"
			                    "unit/equip.tab:%d:xcom3/tacdata/tactical.pal",
			                    held_sprite_index * 8 + j)));

		e->equipscreen_sprite = fw().data->loadImage(UString::format(
		    "PCK:xcom3/ufodata/pequip.pck:xcom3/ufodata/pequip.tab:%d:xcom3/tacdata/tactical.pal",
		    (int)edata.sprite_idx));
		e->equipscreen_size = {edata.size_x, edata.size_y};

		e->manufacturer = {&state, data_u.getOrgId(edata.manufacturer)};

		e->store_space = edata.store_space;
		e->armor = edata.armor;
		e->score = edata.score;

		// Read payload information
		// Only for Ammo, or weapons and grenades with built-in ammo
		if (payload_idx != std::numeric_limits<unsigned>::max())
		{
			if (payload_idx >= data_t.agent_payload->count())
				LogError("Invalid payload index %u for ID %s", payload_idx, id.cStr());

			auto pdata = data_t.agent_payload->get(payload_idx);

			e->speed = pdata.speed;
			e->damage = pdata.damage;
			e->accuracy = pdata.accuracy;
			e->fire_delay = pdata.fire_delay;
			e->guided = pdata.guided != 0 ? true : false;
			e->turn_rate = pdata.turn_rate;
			e->range = pdata.range;
			e->explosion_graphic = pdata.explosion_graphic;
			if (pdata.fire_sfx != 0xffff)
				e->fire_sfx =
				    fw().data->loadSample(UString::format("RAWSOUND:%d", (int)pdata.fire_sfx));
			if (pdata.impact_sfx != 0xffff)
				e->impact_sfx =
				    fw().data->loadSample(UString::format("RAWSOUND:%d", (int)pdata.impact_sfx));

			e->damage_type = {&state, data_t.getDTypeId(pdata.damage_type)};
			switch (pdata.trigger_type)
			{
				case AGENT_GRENADE_TRIGGER_TYPE_NORMAL:
					e->trigger_type = AEquipmentType::TriggerType::Normal;
					break;
				case AGENT_GRENADE_TRIGGER_TYPE_PROXIMITY:
					e->trigger_type = AEquipmentType::TriggerType::Proximity;
					break;
				case AGENT_GRENADE_TRIGGER_TYPE_BOOMEROID:
					e->trigger_type = AEquipmentType::TriggerType::Boomeroid;
					break;
				default:
					LogError("Unexpected grenade trigger type %d for ID %s",
					         (int)pdata.trigger_type, id.cStr());
			}
			e->explosion_depletion_rate = pdata.explosion_depletion_rate;

			auto projectile_sprites = data_t.projectile_sprites->get(pdata.projectile_image);
			// Determine tail size
			e->tail_size = 0;
			for (int i = 0; i < 36; i++)
			{
				if (projectile_sprites.sprites[i] != 255)
				{
					e->tail_size = i + 1;
				}
			}

			for (int i = 0; i < e->tail_size; i++)
			{
				UString sprite_path = "";
				if (projectile_sprites.sprites[i] != 255)
				{
					sprite_path = UString::format("bulletsprites/battle/%02u.png",
					                              (unsigned)projectile_sprites.sprites[i]);
				}
				else
				{
					sprite_path = ""; // a 'gap' in the projectile trail
				}
				e->projectile_sprites.push_back(fw().data->loadImage(sprite_path));
			}
		}

		state.agent_equipment[id] = e;
	}

	// Fix the tracker gun ammo
	weapons[8]->ammo_types.emplace(&state, tracker_gun_clip_id);

	// Equipment sets - score - alien
	{
		if (data_t.agent_equipment_set_score_requirement->count() != 1)
			LogError("Incorrect amount of alien score requirement structures: encountered %u, "
			         "expected 1",
			         (unsigned)data_t.agent_equipment_set_score_requirement->count());
		auto sdata = data_t.agent_equipment_set_score_requirement->get(0);

		if (data_t.agent_equipment_set_score_alien->count() != 1)
			LogError("Incorrect amount of alien score equipment set structures: encountered %u, "
			         "expected 1",
			         (unsigned)data_t.agent_equipment_set_score_alien->count());
		auto data = data_t.agent_equipment_set_score_alien->get(0);
		for (unsigned i = 0; i < 8; i++)
		{
			auto es = mksp<EquipmentSet>();

			UString id = UString::format("%sALIEN_%d", EquipmentSet::getPrefix(), (int)i + 1);
			es->id = id;

			for (unsigned j = 0; j < 10; j++)
			{
				if (data.weapons[j][i].weapon_idx > 0)
				{
					if (data.weapons[j][i].clip_idx > 0)
					{
						es->weapons.push_back(
						    {{&state,
						      UString::format("%s%s", AEquipmentType::getPrefix(),
						                      canon_string(data_u.agent_equipment_names->get(
						                          data.weapons[j][i].weapon_idx)))},
						     {&state,
						      UString::format("%s%s", AEquipmentType::getPrefix(),
						                      canon_string(data_u.agent_equipment_names->get(
						                          data.weapons[j][i].clip_idx)))},
						     std::max((int)data.weapons[j][i].clip_amount, 1)});
					}
					else
					{
						es->weapons.push_back(
						    {{&state,
						      UString::format("%s%s", AEquipmentType::getPrefix(),
						                      canon_string(data_u.agent_equipment_names->get(
						                          data.weapons[j][i].weapon_idx)))}});
					}
				}
				if (data.grenades[j][i].grenade_idx > 0 && data.grenades[j][i].grenade_amount > 0)
				{
					es->grenades.push_back(
					    {{&state, UString::format("%s%s", AEquipmentType::getPrefix(),
					                              canon_string(data_u.agent_equipment_names->get(
					                                  data.grenades[j][i].grenade_idx)))},
					     data.grenades[j][i].grenade_amount});
				}
				if (data.equipment[j][i][0] > 0 || data.equipment[j][i][1] > 0)
				{
					if (data.equipment[j][i][0] > 0 && data.equipment[j][i][1] > 0)
					{
						es->equipment.push_back(
						    {{&state,
						      UString::format("%s%s", AEquipmentType::getPrefix(),
						                      canon_string(data_u.agent_equipment_names->get(
						                          data.equipment[j][i][0])))},
						     {&state,
						      UString::format("%s%s", AEquipmentType::getPrefix(),
						                      canon_string(data_u.agent_equipment_names->get(
						                          data.equipment[j][i][1])))}});
					}
					else if (data.equipment[j][i][0] > 0)
					{
						es->equipment.push_back(
						    {{&state,
						      UString::format("%s%s", AEquipmentType::getPrefix(),
						                      canon_string(data_u.agent_equipment_names->get(
						                          data.equipment[j][i][0])))}});
					}
					else
					{
						es->equipment.push_back(
						    {{&state,
						      UString::format("%s%s", AEquipmentType::getPrefix(),
						                      canon_string(data_u.agent_equipment_names->get(
						                          data.equipment[j][i][1])))}});
					}
				}
			}

			int diff = 0;
			switch (difficulty)
			{
				case Difficulty::DIFFICULTY_1:
					diff = 0;
					break;
				case Difficulty::DIFFICULTY_2:
					diff = 1;
					break;
				case Difficulty::DIFFICULTY_3:
					diff = 2;
					break;
				case Difficulty::DIFFICULTY_4:
					diff = 3;
					break;
				case Difficulty::DIFFICULTY_5:
					diff = 4;
					break;
				default:
					LogError("Unknown difficulty");
			}

			es->min_score = i == 0 ? INT_MIN : (int)sdata.score[diff][i - 1];
			es->max_score = i == 7 ? INT_MAX : (int)sdata.score[diff][i];

			state.equipment_sets_by_score[id] = es;
		}
	}
	// Equipment sets - score (level) - human
	{
		if (data_t.agent_equipment_set_score_human->count() != 1)
			LogError("Incorrect amount of human score equipment set structures: encountered %u, "
			         "expected 1",
			         (unsigned)data_t.agent_equipment_set_score_human->count());
		auto data = data_t.agent_equipment_set_score_human->get(0);
		for (unsigned i = 0; i < 12; i++)
		{
			for (unsigned i = 0; i < 12; i++)
			{
				auto es = mksp<EquipmentSet>();

				UString id = UString::format("%sHUMAN_%d", EquipmentSet::getPrefix(), (int)i + 1);
				es->id = id;

				for (unsigned j = 0; j < 10; j++)
				{
					if (data.weapons[j][i].weapon_idx > 0)
					{
						if (data.weapons[j][i].clip_idx > 0)
						{
							es->weapons.push_back(
							    {{&state,
							      UString::format("%s%s", AEquipmentType::getPrefix(),
							                      canon_string(data_u.agent_equipment_names->get(
							                          data.weapons[j][i].weapon_idx)))},
							     {&state,
							      UString::format("%s%s", AEquipmentType::getPrefix(),
							                      canon_string(data_u.agent_equipment_names->get(
							                          data.weapons[j][i].clip_idx)))},
							     std::max((int)data.weapons[j][i].clip_amount, 1)});
						}
						else
						{
							es->weapons.push_back(
							    {{&state,
							      UString::format("%s%s", AEquipmentType::getPrefix(),
							                      canon_string(data_u.agent_equipment_names->get(
							                          data.weapons[j][i].weapon_idx)))}});
						}
					}
					if (data.grenades[j][i].grenade_idx > 0 &&
					    data.grenades[j][i].grenade_amount > 0)
					{
						es->grenades.push_back(
						    {{&state,
						      UString::format("%s%s", AEquipmentType::getPrefix(),
						                      canon_string(data_u.agent_equipment_names->get(
						                          data.grenades[j][i].grenade_idx)))},
						     data.grenades[j][i].grenade_amount});
					}
					if (data.equipment[j][i][0] > 0 || data.equipment[j][i][1] > 0)
					{
						if (data.equipment[j][i][0] > 0 && data.equipment[j][i][1] > 0)
						{
							es->equipment.push_back(
							    {{&state,
							      UString::format("%s%s", AEquipmentType::getPrefix(),
							                      canon_string(data_u.agent_equipment_names->get(
							                          data.equipment[j][i][0])))},
							     {&state,
							      UString::format("%s%s", AEquipmentType::getPrefix(),
							                      canon_string(data_u.agent_equipment_names->get(
							                          data.equipment[j][i][1])))}});
						}
						else if (data.equipment[j][i][0] > 0)
						{
							es->equipment.push_back(
							    {{&state,
							      UString::format("%s%s", AEquipmentType::getPrefix(),
							                      canon_string(data_u.agent_equipment_names->get(
							                          data.equipment[j][i][0])))}});
						}
						else
						{
							es->equipment.push_back(
							    {{&state,
							      UString::format("%s%s", AEquipmentType::getPrefix(),
							                      canon_string(data_u.agent_equipment_names->get(
							                          data.equipment[j][i][1])))}});
						}
					}
				}

				es->min_score = i == 0 ? INT_MIN : i;
				es->max_score = i == 12 ? INT_MAX : i + 1;

				state.equipment_sets_by_level[id] = es;
			}
		}
	}
}

} // namespace OpenApoc