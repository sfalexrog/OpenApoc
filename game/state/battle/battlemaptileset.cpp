#include "game/state/battle/battlemaptileset.h"
#include "framework/framework.h"

namespace OpenApoc
{

UString BattleMapTileset::getTilesetPath() { return fw().getDataDir() + "/tilesets"; }

} // namespace OpenApoc
