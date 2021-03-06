#pragma once

#include "game/state/stateobject.h"
#include "library/sp.h"
#include <list>
#include <map>
#include <vector>

namespace OpenApoc
{
class Sample;
class DamageType;

class CityCommonSampleList
{
  public:
	sp<Sample> teleport;
	std::list<sp<Sample>> alertSounds;
};
}
