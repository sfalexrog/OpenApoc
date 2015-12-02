#include "library/sp.h"
#include "game/city/weapon.h"
#include "framework/logger.h"
#include "framework/framework.h"
#include "game/city/vehicle.h"
#include "game/city/projectile.h"
#include "game/tileview/tileobject_vehicle.h"

namespace OpenApoc
{

Weapon::Weapon(const VWeaponType &type, sp<Vehicle> owner, int initialAmmo, State initialState)
    : state(initialState), type(type), owner(owner), ammo(initialAmmo), reloadTime(0)
{
}
sp<Projectile> Weapon::fire(Framework &fw, Vec3<float> target)
{
	auto owner = this->owner.lock();
	if (!owner)
	{
		LogError("Called on weapon with no owner?");
	}
	auto vehicleTile = owner->tileObject;
	if (!vehicleTile)
	{
		LogError("Called on vehicle with no tile object?");
	}
	if (this->state != State::Ready)
	{
		LogWarning("Trying to fire weapon in state %d", this->state);
		return nullptr;
	}
	if (this->ammo <= 0)
	{
		LogWarning("Trying to fire weapon with no ammo");
		return nullptr;
	}
	this->reloadTime = this->type.fire_delay;
	this->state = State::Reloading;
	this->ammo--;

	if (this->type.fire_sfx)
	{
		fw.soundBackend->playSample(this->type.fire_sfx);
	}

	if (this->ammo == 0)
	{
		this->state = State::OutOfAmmo;
	}

	Vec3<float> velocity = target - vehicleTile->getPosition();
	velocity = glm::normalize(velocity);
	velocity *= this->type.speed;
	return std::make_shared<Projectile>(owner, vehicleTile->getPosition(), velocity,
	                                    static_cast<int>(this->getRange() / this->type.speed),
	                                    Colour{255, 0, 255, 255}, this->type.tail_size, 2.0f);

	return nullptr;
}

void Weapon::update(int ticks)
{
	if (this->reloadTime != 0)
	{
		if (ticks >= this->reloadTime)
			this->reloadTime = 0;
		else
			this->reloadTime -= ticks;
	}
	switch (this->state)
	{
		case State::Reloading:
			if (this->reloadTime == 0)
				this->state = State::Ready;
			return;
		case State::OutOfAmmo:
			if (this->ammo > 0)
				this->state = State::Ready;
			return;
		default:
			return;
	}
}

int Weapon::reload(int ammoAvailable)
{
	int ammoRequired = this->type.max_ammo - this->ammo;
	int reloadAmount = std::min(ammoRequired, ammoAvailable);
	this->ammo += reloadAmount;
	return reloadAmount;
}

}; // namespace OpenApoc
