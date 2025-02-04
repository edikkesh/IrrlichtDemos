#pragma once

#include "desteer/entity/IBaseEntity.hpp"
#include "desteer/entity/IEntitySpace.hpp"

//Predefining stuff to avoid circular reference.
namespace desteer { namespace controller {
class ISteeringController;
}}

namespace desteer {
namespace entity {

//TODO: change this to be an actual abstract class and make MobileEntity, which has the stuff to derive from.
class IMobileEntity : public IBaseEntity, public IEntitySpace
{
protected:
	float _mass;
	float _maxForce;
	float _maxSpeed;
	//float _maxTurnRate;

	irr::core::vector3df _velocity;

public:
    float Mass() const { return _mass; };
	void SetMass(float mass) { _mass = mass; };

	float MaxForce() const { return _maxForce; };
	void SetMaxForce(float maxForce) { _maxForce = maxForce; };

	float MaxSpeed() const { return _maxSpeed; };
	void SetMaxSpeed(float maxSpeed) { _maxSpeed = maxSpeed; };

	//inline float MaxTurnRate() const { return _maxTurnRate; };
	//inline void SetMaxTurnRate(float maxTurnRate) { _maxTurnRate = maxTurnRate; };

    virtual irr::core::vector3df Velocity() const {return _velocity;};

    virtual void SetSteering(controller::ISteeringController * controller) = 0;
};

}//end namespace entity
}//end namespace desteer

