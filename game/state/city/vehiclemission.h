#pragma once

#include "game/state/stateobject.h"
#include "game/state/tileview/tile.h"
#include "library/strings.h"
#include "library/vec.h"
#include <list>
#include <map>

namespace OpenApoc
{

static const int TELEPORTER_SPREAD = 10;

class Vehicle;
class Tile;
class TileMap;
class Building;
class UString;

class FlyingVehicleTileHelper : public CanEnterTileHelper
{
  private:
	TileMap &map;
	Vehicle &v;

  public:
	FlyingVehicleTileHelper(TileMap &map, Vehicle &v);

	bool canEnterTile(Tile *from, Tile *to, bool ignoreStaticUnits = false,
	                  bool ignoreAllUnits = false) const override;

	float pathOverheadAlloawnce() const override;

	// Support 'from' being nullptr for if a vehicle is being spawned in the map
	bool canEnterTile(Tile *from, Tile *to, bool, bool &, float &cost, bool &, bool,
	                  bool) const override;

	float adjustCost(Vec3<int> nextPosition, int z) const override;

	float getDistance(Vec3<float> from, Vec3<float> to) const override;

	float getDistance(Vec3<float> from, Vec3<float> toStart, Vec3<float> toEnd) const override;

	bool canLandOnTile(Tile *to) const;

	Vec3<int> findTileToLandOn(GameState &, sp<TileObjectVehicle> vTile) const;

	Vec3<float> findSidestep(GameState &state, sp<TileObjectVehicle> vTile,
	                         sp<TileObjectVehicle> targetTile, float distancePref) const;
};

class VehicleMission
{
  private:
	// INTERNAL: Not to be used directly (Only works when in building)
	static VehicleMission *takeOff(Vehicle &v);
	// INTERNAL: Not to be used directly (Only works if directly above a pad)
	static VehicleMission *land(Vehicle &v, StateRef<Building> b);
	// INTERNAL: This checks if mission is actually finished. Called by isFinished.
	// If it is finished, update() is called by isFinished so that any remaining work could be done
	bool isFinishedInternal(GameState &state, Vehicle &v);

	bool takeOffCheck(GameState &state, Vehicle &v);
	bool teleportCheck(GameState &state, Vehicle &v);

  public:
	VehicleMission() = default;

	// Methods used in pathfinding etc.
	bool getNextDestination(GameState &state, Vehicle &v, Vec3<float> &destPos, float &destFacing);
	void update(GameState &state, Vehicle &v, unsigned int ticks, bool finished = false);
	bool isFinished(GameState &state, Vehicle &v, bool callUpdateIfFinished = true);
	void start(GameState &state, Vehicle &v);
	void setPathTo(GameState &state, Vehicle &v, Vec3<int> target, int maxIterations = 25,
	               bool checkValidity = true, bool giveUpIfInvalid = false);
	bool advanceAlongPath(GameState &state, Vehicle &v, Vec3<float> &destPos, float &destFacing);
	bool isTakingOff(Vehicle &v);

	// Methods to create new missions
	static VehicleMission *gotoLocation(GameState &state, Vehicle &v, Vec3<int> target,
	                                    bool allowTeleporter = false, bool pickNearest = false,
	                                    int reRouteAttempts = 10);
	static VehicleMission *gotoPortal(GameState &state, Vehicle &v);
	static VehicleMission *gotoPortal(GameState &state, Vehicle &v, Vec3<int> target);
	static VehicleMission *gotoBuilding(GameState &state, Vehicle &v, StateRef<Building> target,
	                                    bool allowTeleporter = false);
	static VehicleMission *infiltrateOrSubvertBuilding(GameState &state, Vehicle &v,
	                                                   StateRef<Building> target,
	                                                   bool subvert = false);
	static VehicleMission *attackVehicle(GameState &state, Vehicle &v, StateRef<Vehicle> target);
	static VehicleMission *followVehicle(GameState &state, Vehicle &v, StateRef<Vehicle> target);
	static VehicleMission *snooze(GameState &state, Vehicle &v, unsigned int ticks);
	static VehicleMission *restartNextMission(GameState &state, Vehicle &v);
	static VehicleMission *crashLand(GameState &state, Vehicle &v);
	static VehicleMission *patrol(GameState &state, Vehicle &v, unsigned int counter = 10);
	static VehicleMission *teleport(GameState &state, Vehicle &v, Vec3<int> target = {-1, -1, -1});
	UString getName();

	enum class MissionType
	{
		GotoLocation,
		GotoBuilding,
		FollowVehicle,
		AttackVehicle,
		AttackBuilding,
		RestartNextMission,
		Snooze,
		TakeOff,
		Land,
		Crash,
		Patrol,
		GotoPortal,
		InfiltrateSubvert,
		Teleport
	};

	MissionType type = MissionType::GotoLocation;

	// GotoLocation InfiltrateSubvert TakeOff GotoPortal Patrol
	Vec3<int> targetLocation = {0, 0, 0};
	// GotoLocation GotoBuilding
	bool allowTeleporter = false;
	// How many times will vehicle try to re-route until it gives up
	int reRouteAttempts = 0;
	// GotoLocation - should it pick nearest point or random point if destination unreachable
	bool pickNearest = false;
	// GotoBuilding AttackBuilding Land Infiltrate
	StateRef<Building> targetBuilding;
	// FollowVehicle AttackVehicle
	StateRef<Vehicle> targetVehicle;
	// Snooze
	unsigned int timeToSnooze = 0;
	// InfiltrateSubvert, Patrol: waypoints
	unsigned int missionCounter = 0;
	// InfiltrateSubvert: mode
	bool subvert = false;

	std::list<Vec3<int>> currentPlannedPath;
};
} // namespace OpenApoc
