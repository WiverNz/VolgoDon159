#pragma once

#include <CryRenderer/IRenderer.h>
#include <CryRenderer/IShader.h>

#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/MathTypes.h>
#include <CrySchematyc/Utils/IString.h>
#include <CrySchematyc/Utils/SharedString.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>
#include <CryCore/smartptr.h>

#include <CryPhysics/physinterface.h>

namespace TestComponents
{
class CTestTraceComponent : public IEntityComponent
{
	struct STestPoint
	{
		STestPoint() = default;

		STestPoint(const Vec3& pnt, const string& dscr) : point(pnt), description(dscr) {}

		bool operator==(const STestPoint& other) const
		{
			return point == other.point && description == other.description;
		}

		static void ReflectType(Schematyc::CTypeDesc<STestPoint>& desc)
		{
			desc.SetGUID("{8352FA05-3A67-4B50-943C-83ED720AB0F8}"_cry_guid);
		}

		void Serialize(Serialization::IArchive& ar)
		{
			ar(point, "Point", "Curr point"); ar.doc("Current point");
			ar(description, "Description", "Description"); ar.doc("Description");
		}

		Vec3 point = Vec3(0, 0, 0);
		Schematyc::CSharedString description;
	};

	struct STestPoints
	{
		inline bool operator==(const STestPoints& other) const { return points == other.points; }

		static void ReflectType(Schematyc::CTypeDesc<STestPoints>& desc)
		{
			desc.SetGUID("{77251060-B1A4-4E00-9FDB-EF4776CE610C}"_cry_guid);
		}

		uint version = 0;
		std::vector<STestPoint> points;
	};

protected:
	// IEntityComponent
	virtual void Initialize() final {}
	virtual void ProcessEvent(const SEntityEvent& event) final;
	virtual Cry::Entity::EventFlags GetEventMask() const override
	{
		Cry::Entity::EventFlags bitFlags = ENTITY_EVENT_UPDATE | ENTITY_EVENT_COMPONENT_PROPERTY_CHANGED | ENTITY_EVENT_START_GAME | ENTITY_EVENT_RESET | ENTITY_EVENT_ATTACH | ENTITY_EVENT_DETACH;

		return bitFlags;
	}
	// ~IEntityComponent

public:
	CTestTraceComponent();
	virtual ~CTestTraceComponent();

	static void Register(Schematyc::CEnvRegistrationScope& componentScope) {}
	static void ReflectType(Schematyc::CTypeDesc<CTestTraceComponent>& desc)
	{
		desc.SetGUID("{C3D586C4-41C8-4074-AB32-54C774615C12}"_cry_guid);
		desc.SetEditorCategory("Test");
		desc.SetLabel("Test Trace");
		desc.SetDescription("Test Trace for testing");
		desc.SetIcon("icons:ObjectTypes/object.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach });

		desc.AddMember(&CTestTraceComponent::m_lineThickness, 'lnth', "LineThickness", "Line thickness", "Thickness of lines", 4.0f);
		desc.AddMember(&CTestTraceComponent::m_pointRadius, 'pnth', "PointThickness", "Point thickness", "Thickness of points", 2.0f);
		desc.AddMember(&CTestTraceComponent::m_textSize, 'txsz', "TextSize", "Text size", "Size of text", 3.0f);
		desc.AddMember(&CTestTraceComponent::m_textOffset, 'txof', "TextOffset", "Text offset", "Offset of text", Vec3(-8, 10, 0));
		desc.AddMember(&CTestTraceComponent::m_lineColor, 'lncr', "LineColor", "Line color", "Color of lines", ColorB(255, 0, 0, 255));
		desc.AddMember(&CTestTraceComponent::m_pointColor, 'pncr', "PointColor", "Point color", "Color of points", ColorB(255, 0, 0, 255));
		desc.AddMember(&CTestTraceComponent::m_textColor, 'txcr', "TextColor", "Text color", "Color of text", ColorF(1, 1, 1, 1.f));
		desc.AddMember(&CTestTraceComponent::m_points, 'pnts', "Test points", "Test points", "Points", STestPoints());
	}

	void Physicalize();

	inline const STestPoints& GetPoints() { return m_points; }

	void AddAbsPoint(const Vec3& point, const string& description)
	{
		m_points.points.push_back(STestPoint(point, description));
	}

	void AddRelPoint(const Vec3& point, const string& description)
	{
		Vec3 lastPoint;
		if (m_points.points.size())
		{
			lastPoint = m_points.points.back().point;
		}
		else if (GetEntity())
		{
			lastPoint = GetEntity()->GetPos();
		}
		const Vec3 newPoint = lastPoint + point;
		const string currPointText = string().Format("AddRelPoint %s -> (%f, %f, %f)", description, newPoint.x, newPoint.y, newPoint.z);
		CryLog(currPointText);
		m_points.points.push_back(STestPoint(newPoint, description));
	}
	void Clear()
	{
		m_points.points.clear();
	}

protected:
	float m_lineThickness = 4.0f;
	float m_pointRadius = 2.0f;
	float m_textSize = 3.0f;
	Vec3 m_textOffset = Vec3(-8, 10, 0);
	ColorB m_lineColor = ColorB(255, 0, 0, 255);
	ColorB m_pointColor = ColorB(0, 255, 0, 255);
	ColorF m_textColor = ColorF(1, 1, 1, 1.f);
protected:
	STestPoints m_points;
};
} // namespace TestComponents