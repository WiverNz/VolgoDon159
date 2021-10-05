#pragma once

#include "common/CryCommon.h"

#include <CryPhysics/physinterface.h>
#include <CrySerialization/Decorators/Resources.h>

// This stub fixed the issue of serializing float values from -0.99f to +0.99f (The string ends after a float value)
#include "common/StubFeatures/ParamTraits.h"

//! \namespace Navigation
//! \brief The ship navigation namespace. It shouldn't have any dependencies with other modules
namespace Navigation
{
//! \struct SStatusShip
//! \brief The structure with ship movement state (optional values?)
struct SStatusShip : /*pe_status*/ public Serialization::SStruct
{
	//enum entype { type_id = ePE_status_vehicle };
	SStatusShip() { /*type = type_id;*/ }

	//! \brief Serialization of the structure
	//! \param {Serialization::IArchive} archive for serialization/deserialization
	//! \return {bool} true if successful
	bool Serialize(Serialization::IArchive& ar);

	//! \brief Get Json string
	//! \return {string} Json string with all parameters
	string GetJsonString() const;

	pfx2::SFloat steer = 0.0f;													//!< current steering angle
	pfx2::SFloat rudder = 0.0f;													//!< rudder angle
	CryTransform::CAngle course = CryTransform::CAngle::FromDegrees(0.0f);		//!< ship course
	Vec3 velocity;																//!< velocity
	bool anchorBrake = false;													//!< nonzero if handbrake is on
	pfx2::SFloat leftEngineRPM = 0.0f;											//!< left engine rpm
	pfx2::SFloat rightEngineRPM = 0.0f;											//!< right engine rpm
	pfx2::SFloat speed = 0.0f;													//!< speed
	int leftGear = 0;															//!< left gear
	int rightGear = 0;															//!< right gear
};

//! \struct SShipParameters
//! \brief The structure with ship parameters
struct SShipParameters
{
	string shipType;				//!< ship type
	string resourcePath;			//!< path to the asset (e.g. "Assets/props/.../*.cgf")
	string materialPath;			//!< path to the material (e.g. "Assets/textures/.../*.mtl.cryasset")
	float shipSize = 0.0f;			//!< size at scantling draught (mt)
	float lenght = 0.0f;			//!< length between perpendiculars (m)
	float breadth = 0.0f;			//!< breadth (m)
	float maxDraught = 0.0f;		//!< draught scantling (m)
	float boardHeight = 0.0f;		//!< board height (m)
	float maxSpeed = 0.0f;			//!< maximum speed (kn)
	float engineSpeed = 0.0f;		//!< engine speed at MCR (rpm)
	float deadWeight = 0.0f;		//!< dead weight (t)
	string description;				//!< description of the ship
};

//! Enum to indicate who controls the ship
enum class EShipControl
{
	AutoPilot,
	Player
};

//! Enum for state of the ship
enum class EShipState
{
	Normal,
	RanAground
};

} // namespace Navigation
