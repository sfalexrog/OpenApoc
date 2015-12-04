#pragma once
#include "library/sp.h"

#include "game/rules/vequipment.h"

#include "library/vec.h"

namespace OpenApoc
{
class Vehicle;
class Projectile;
class Framework;
class Weapon
{
  public:
	enum class State
	{
		Ready,
		Disabled,
		Reloading,
		OutOfAmmo,
	};

  private:
	State state;
	const VWeaponType &type;
	std::weak_ptr<Vehicle> owner;
	int ammo;
	int reloadTime;

  public:
	Weapon(const VWeaponType &type, sp<Vehicle> owner, int initialAmmo,
	       State initialState = State::Ready);

	const VWeaponType &getWeaponType() const { return type; }
	float getRange() const { return this->type.range; }
	bool canFire() const { return state == State::Ready; }
	void update(int ticks);
	// Reload uses up to 'ammoAvailable' to reload the weapon. It returns the amount
	// actually used.
	int reload(int ammoAvailable);
	sp<Projectile> fire(Framework &fw, Vec3<float> target);
};
}; // namespace OpenApoc
