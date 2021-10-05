#pragma once

#include "common/CryCommon.h"
#include "common/NavigationTypes.h"

//! \namespace Calculations
//! \brief The calculations namespace. Here only dependence with the Common is possible
namespace Calculations
{
//! \struct IMovementCalculator
//! \brief Interface for {CMovementCalculator}
struct IMovementCalculator
{
	virtual void Calculate(const float& frameTime) = 0;
	virtual void AccelerationCalculation() = 0;
	virtual void RotateCalculation() = 0;
	virtual Vec3 GetNextPos(const Vec3& currPos) const = 0;
	virtual Vec3 GetNextDiffPos(const Vec3& currPos) const = 0;

	virtual void LeftClickHandler() = 0;
	virtual void RightClickHandler() = 0;
	virtual void ForwardClickHandler() = 0;
	virtual void BackClickHandler() = 0;
};

DECLARE_SHARED_POINTERS(IMovementCalculator);

//! \class CMovementCalculator
//! \brief This is where all magic has to happen.
class CMovementCalculator : public IMovementCalculator
{
	CRYINTERFACE_SIMPLE(IMovementCalculator)
public:
	explicit CMovementCalculator(const float frameTime, const Navigation::SShipParameters* shipParameters, Navigation::SStatusShip* statusShip);
	virtual ~CMovementCalculator() = default;

	void Calculate(const float& frameTime) override;
	Vec3 GetNextPos(const Vec3& currPos) const override;
	Vec3 GetNextDiffPos(const Vec3& currPos) const override;
	void AccelerationCalculation() override;
	void RotateCalculation() override;

	void LeftClickHandler() override;
	void RightClickHandler() override;
	void ForwardClickHandler() override;
	void BackClickHandler() override;
protected:
	float m_frameTime = 0;
	float m_nextPosVecLenght = 0;
	const Navigation::SShipParameters* m_shipParameters = nullptr;
	Navigation::SStatusShip* m_statusShip = nullptr;
};

} // namespace Calculations
