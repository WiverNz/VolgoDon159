#pragma once

#include <CryRenderer/IRenderer.h>
#include <CryRenderer/IShader.h>

#include <CrySchematyc/ResourceTypes.h>
#include <CrySchematyc/MathTypes.h>
#include <CrySchematyc/Utils/IString.h>
#include <CrySchematyc/Env/IEnvRegistrar.h>

#include <CryPhysics/physinterface.h>

namespace TestComponents
{
template<int i> constexpr int const_ilog10() { return 1 + const_ilog10<i / 10>(); }
template<> constexpr int const_ilog10<0>() { return 0; }

#define DECL_POWERS P(0) P(1) P(2) P(3) P(4) P(5) P(6) P(7) P(8) P(9) P(10)
#define P(i) ePow_##i = 1<<i,
enum EPowerOf2 { DECL_POWERS };
#undef P

static void ReflectType(Schematyc::CTypeDesc<EPowerOf2>& desc)
{
	desc.SetGUID("{BC03415A-8006-4518-8B79-A79E0AB2A35D}"_cry_guid);
	desc.SetLabel("Power of 2");
#define P(i) \
			static char label##i[const_ilog10<(1<<i)>()+1]; ltoa(1<<i, label##i, 10); \
			desc.AddConstant(EPowerOf2::ePow_##i, #i, label##i);
	DECL_POWERS
#undef P
}


class CTestGridComponent : public IEntityComponent
{
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
	CTestGridComponent() {}
	virtual ~CTestGridComponent();

	static void Register(Schematyc::CEnvRegistrationScope& componentScope) {}
	static void ReflectType(Schematyc::CTypeDesc<CTestGridComponent>& desc)
	{
		desc.SetGUID("{3DD77E9F-3215-4ADD-AC99-444A45A6FEC1}"_cry_guid);
		desc.SetEditorCategory("Test");
		desc.SetLabel("Test Grid");
		desc.SetDescription("Local Grid for test");
		desc.SetIcon("icons:ObjectTypes/object.ico");
		desc.SetComponentFlags({ IEntityComponent::EFlags::Socket, IEntityComponent::EFlags::Attach });

		desc.AddMember(&CTestGridComponent::m_sizex, 'sizx', "SizeX", "Number Of Cells X", "Number of cells in X dimension", ePow_7);
		desc.AddMember(&CTestGridComponent::m_sizey, 'sizy', "SizeY", "Number Of Cells Y", "Number of cells in Y dimension", ePow_7);
		desc.AddMember(&CTestGridComponent::m_cellSize, 'clsz', "CellSize", "Cell Size", "Grid Cell Dimensions", Vec2(10, 10));
		desc.AddMember(&CTestGridComponent::m_height, 'hght', "Height", "Height", "Height of the grid area", 16.0f);
		desc.AddMember(&CTestGridComponent::m_accThresh, 'acct', "AccThresh", "Acceleration Threshold", "Minimal host acceleration that is applied to objects inside the grid", 3.0f);
		desc.AddMember(&CTestGridComponent::m_thickness, 'thic', "Thickness", "Thickness", "Thickness of the grid", 5.0f);
		desc.AddMember(&CTestGridComponent::m_colorLeft, 'crlt', "ColorLeft", "Color left", "Left color", ColorB(0, 0, 0, 255));
		desc.AddMember(&CTestGridComponent::m_colorRight, 'crrt', "ColorRight", "Color right", "Right color", ColorB(50, 50, 50, 255));
	}
	void Physicalize();

	EPowerOf2 m_sizex = ePow_7;
	EPowerOf2 m_sizey = ePow_7;
	Vec2  m_cellSize = Vec2(10, 10);
	float m_height = 16.0f;
	float m_accThresh = 3.0f;
	float m_thickness = 5.0f;
	ColorB m_colorLeft = ColorB(0, 0, 0, 255);
	ColorB m_colorRight = ColorB(50, 50, 50, 255);
protected:
	_smart_ptr<IPhysicalEntity> m_pGrid;
	std::vector<_smart_ptr<IPhysicalEntity>> m_physAreas;
};
} // namespace TestComponents