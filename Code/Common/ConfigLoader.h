#pragma once

#include <CryExtension/ICryFactory.h>
#include <CryExtension/ClassWeaver.h>
#include <CryExtension/CryCreateClassInstance.h>

#include "NavigationTypes.h"

//! \namespace Common
//! \brief The common namespace. It shouldn't have any dependencies with other modules
namespace Common
{
//! \struct IConfigLoader
//! \brief Interface for {CConfigLoader}
struct IConfigLoader : public ICryUnknown
{
	CRYINTERFACE_DECLARE_GUID(IConfigLoader, "A5470F31-3325-4F31-A32E-47C4EE744180"_cry_guid);
public:
	//! \brief Loading ship type data
	//! \param shipParameters {SShipParameters} structure for loading data
	//! \param shipType {string} ship type to load
	//! \return {bool} true if loaded correctly
	virtual bool LoadShipParameters(Navigation::SShipParameters& shipParameters, const string& shipType) const = 0;
	//! \brief Get ship types xml path
	//! \return {string} path to the xml config file with ships parameters
	virtual string GetShipTypesXmlPath() const = 0;
	//! \brief Get main menu background path
	//! \return {string} path to background tif file
	virtual string GetMenuBackgroundPath() const = 0;
};

DECLARE_SHARED_POINTERS(IConfigLoader);

//! \class CConfigLoader
//! \brief To load various configuration files
class CConfigLoader : public IConfigLoader
{
	// Start declaring the inheritance hierarchy for this config loader
	// This is followed by any number of CRYINTERFACE_ADD, passing an interface implementing ICryUnknown that has declared its own GUID using CRYINTERFACE_DECLARE_GUID
	CRYINTERFACE_BEGIN()
	// Indicate that we implement IConfigLoader, this is mandatory in order for the instance to be detected after the config has been loaded
	CRYINTERFACE_ADD(IConfigLoader)
	// End the declaration of inheritance hierarchy
	CRYINTERFACE_END()
	// CRYINTERFACE_SIMPLE(IConfigLoader)

	// Set the GUID for the config loader
	// Can be generated in Visual Studio under Tools -> Create GUID
	CRYGENERATE_SINGLETONCLASS_GUID(CConfigLoader, "CConfigLoader", "3663665D-FCB5-44C5-80E4-A99EBA829E33"_cry_guid)

	CConfigLoader() = default;
	virtual ~CConfigLoader() = default;
public:
	bool LoadShipParameters(Navigation::SShipParameters& shipParameters, const string& shipType) const override;
	inline string GetShipTypesXmlPath() const override
	{
		return s_shipTypesXmlPath;
	}
	inline string GetMenuBackgroundPath() const
	{
		return s_menuBackgroundPath;
	}

protected:
	static XmlNodeRef LoadXmlFromFile(const char* sFilename, bool bReuseStrings = false);
	static XmlNodeRef FindXmlNodeByType(
		const XmlNodeRef xmlRootNode,
		const string& typeToFind,
		const string& typeTag,
		const string& valueAttr = "value");
	static string GetStringAttr(XmlNodeRef xmlNode, const string& attr);

protected:
	static const string s_shipTypesXmlPath;
	static const string s_menuBackgroundPath;

	static const string s_descriptionTag;
	static const string s_versionAttr;
	static const string s_valueAttr;
};

//! \brief Structure storing the description of the ship types xml file
struct SShipTypesXml
{
	static const string s_shipTypesTag;
	static const string s_shipTypeTag;

	static const string s_resourcePathTag;
	static const string s_materialPathTag;

	// Size tag:
	static const string s_sizeParamsTag;
	static const string s_shipSizeAttr;
	static const string s_lenghtAttr;
	static const string s_breadthAttr;
	static const string s_maxDraughtAttr;
	static const string s_boardHeightAttr;

	// Speed tag:
	static const string s_speedParamsTag;
	static const string s_maxSpeedAttr;
	static const string s_engineSpeedAttr;

	// Weight tag:
	static const string s_weightParamsTag;
	static const string s_grossTonnageAttr;
	static const string s_deadWeightAttr;
	static const string s_displacementAttr;
};

DECLARE_SHARED_POINTERS(CConfigLoader);

//! \brief Get config loader interface pointer
static IConfigLoaderPtr GetIConfigLoaderPtr()
{
	IConfigLoaderPtr pConfigLoader;
	CryCreateClassInstanceForInterface(cryiidof<IConfigLoader>(), pConfigLoader);

	return pConfigLoader;
}

//! \brief Get config loader pointer
ILINE CConfigLoader* GetPConfigLoader()
{
	static IConfigLoaderPtr pConfigLoader(GetIConfigLoaderPtr());

	return static_cast<CConfigLoader*>(pConfigLoader.get());
};

} // namespace Common
