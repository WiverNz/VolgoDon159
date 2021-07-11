#include "StdAfx.h"
#include "NavigationTypes.h"

#include <CrySerialization/Decorators/Resources.h>
#include <CrySerialization/Enum.h>
#include <CryCore/Assert/CryAssert.h>

namespace Navigation
{

// Temporary patch for void MemoryWriter::appendAsString(double value, bool allowTrailingPoint)
// An issue of serializing float values from -0.99f to +0.99f e.g. 0.012345 (The string ends after a float value)
// Possible it works in binary and not in gitlab version?
template <typename T>
void SmallValueSerializationFixer(Serialization::IArchive& ar, const T& value, const char* name)
{
	const T zeroVal = 0.0f;
	if ((value > 0.0f && value < 1.0f) || (value < 0.0f && value > -1.0f))
	{
		ar(zeroVal, name);
	}
	else
	{
		ar(value, name);
	}
}

bool SStatusShip::Serialize(Serialization::IArchive& ar)
{
	/*
	"RateOfTurn"
	"CourseDegree"
	"AccelerationValue"
	"longCoord"
	"latCoord"
	"EnginesMaxRpm"
	"EnginesCurrRpm"
	"DepthValue"
	*/
	ar.openBlock("SStatusShip", "main");
	SmallValueSerializationFixer(ar, steer, "Steer");
	pfx2::SFloat courseDegree = CryTransform::CAngle::FromRadians(steer).ToDegrees();
	SmallValueSerializationFixer(ar, courseDegree, "CourseDegree");
	SmallValueSerializationFixer(ar, speed, "Speed");
	// CryTransform::CAngle::FromDegrees(steerDegrees).ToRadians();
	SmallValueSerializationFixer(ar, rudder, "RudderValue");
	// SmallValueSerializationFixer(ar, velocity, "SpeedValue");
	// SmallValueSerializationFixer(ar, leftEngineRPM, "EnginesCurrRpm");
	SmallValueSerializationFixer(ar, rightEngineRPM, "EnginesCurrRpm");
	
	ar.closeBlock();

	return true;
}

string SStatusShip::GetJsonString() const
{
	DynArray<char> outBuffer;
	if (!Serialization::SaveJsonBuffer(outBuffer, *this))
	{
		return "{}";
	}
	outBuffer.push_back('\0');
	string result = string(outBuffer.data(), outBuffer.size());

	if (result.rfind('}') == -1)
	{
		const string errorText = 
			"The JSON string is invalid." \
			" Possibly a problem due to pure float value" \
			" (problem with serializing values from -0.99f to +0.99f))" \
			" Please use types from StubFeatures/ParamTraits.h" \
			" String: '" + result + "'";
		CryLogAlways(errorText);
		CRY_ASSERT(false, errorText);
		result = "{}";
	}

	return result;
}

} // namespace Common
