#include "StdAfx.h"
#include "ConfigLoader.h"

#include <CryCore/Assert/CryAssert.h>

// Register the factory
// Note that this has to be done in a source file that is not included anywhere else.
CRYREGISTER_SINGLETON_CLASS(Common::CConfigLoader)

namespace Common
{
const string CConfigLoader::s_shipTypesXmlPath = "Assets/config/ShipTypes.xml";
const string CConfigLoader::s_menuBackgroundPath = "Assets/libs/UI/images/background.tif";
const string CConfigLoader::s_versionAttr = "version";
const string CConfigLoader::s_valueAttr = "value";
const string CConfigLoader::s_descriptionTag = "Description";

const string SShipTypesXml::s_shipTypesTag = "ShipTypes";
const string SShipTypesXml::s_shipTypeTag = "ShipType";
const string SShipTypesXml::s_resourcePathTag = "ResourcePath";
const string SShipTypesXml::s_materialPathTag = "MaterialPath";

const string SShipTypesXml::s_sizeParamsTag = "SizeParams";
const string SShipTypesXml::s_shipSizeAttr = "shipSize";
const string SShipTypesXml::s_lenghtAttr = "lenght";
const string SShipTypesXml::s_breadthAttr = "breadth";
const string SShipTypesXml::s_maxDraughtAttr = "maxDraught";
const string SShipTypesXml::s_boardHeightAttr = "boardHeight";

const string SShipTypesXml::s_speedParamsTag = "SpeedParams";
const string SShipTypesXml::s_maxSpeedAttr = "maxSpeed";
const string SShipTypesXml::s_engineSpeedAttr = "engineSpeed";

const string SShipTypesXml::s_weightParamsTag = "WeightParams";
const string SShipTypesXml::s_grossTonnageAttr = "grossTonnage";
const string SShipTypesXml::s_deadWeightAttr = "deadWeight";
const string SShipTypesXml::s_displacementAttr = "displacement";

XmlNodeRef CConfigLoader::LoadXmlFromFile(const char* sFilename, bool bReuseStrings)
{
	const bool isTypesFileExist = gEnv->pCryPak->IsFileExist(sFilename);
	CRY_ASSERT(isTypesFileExist, "Can't find the xml file '%s'", sFilename);
	if (!isTypesFileExist)
	{
		return nullptr;
	}

	XmlNodeRef xmlRootNode = gEnv->pSystem->LoadXmlFromFile(sFilename, bReuseStrings);
	CRY_ASSERT(xmlRootNode, "Can't load the xml file '%s'", sFilename);

	return xmlRootNode;
}

XmlNodeRef CConfigLoader::FindXmlNodeByType(const XmlNodeRef xmlRootNode, const string& typeToFind, const string& typeTag, const string& valueAttr)
{
	XmlNodeRef xmlResultNode = nullptr;

	for (int i = 0, childCount(xmlRootNode->getChildCount()); i < childCount; ++i)
	{
		const XmlNodeRef xmlChildNode = xmlRootNode->getChild(i);
		if (!xmlChildNode->isTag(typeTag))
		{
			continue;
		}
		if (!xmlChildNode->haveAttr(valueAttr))
		{
			CRY_ASSERT(false, "There is no '%s' attr for '%s' node", valueAttr, typeTag);
			continue;
		}

		const stack_string currentType = xmlChildNode->getAttr(valueAttr);
		CryLog("Found type '%s'", currentType.c_str());
		if (typeToFind == currentType.c_str())
		{
			xmlResultNode = xmlChildNode;
		}
	}

	return xmlResultNode;
}

string CConfigLoader::GetStringAttr(XmlNodeRef xmlNode, const string& attr)
{
	if (!xmlNode->haveAttr(attr))
	{
		return string();
	}
	const stack_string currAttr = xmlNode->getAttr(attr);
	
	return string(currAttr.c_str());
}

bool CConfigLoader::LoadShipParameters(Navigation::SShipParameters& shipParameters, const string& shipType) const
{
	const XmlNodeRef xmlRootNode = LoadXmlFromFile(s_shipTypesXmlPath);
	if (!xmlRootNode)
	{
		CRY_ASSERT("Can't load xml file %s", s_shipTypesXmlPath);
		CryLogAlways("Can't load xml file %s", s_shipTypesXmlPath);
		return false;
	}
	
	if (!xmlRootNode->isTag(SShipTypesXml::s_shipTypesTag))
	{
		CRY_ASSERT(false, "There is no ship types tag: '%s' in '%s'", SShipTypesXml::s_shipTypesTag, s_shipTypesXmlPath);
		CryLogAlways("There is no ship types tag: '%s' in '%s'", SShipTypesXml::s_shipTypesTag, s_shipTypesXmlPath);
	}

	CRY_ASSERT(!shipType.empty(), "Ship type is empty");

	if (shipType.empty())
	{
		CryLogAlways("Ship type is empty");
		return false;
	}

	int xmlFileFileVersion = 0;
	xmlRootNode->getAttr(s_versionAttr, xmlFileFileVersion);

	const XmlNodeRef xmlCurrentShipType = FindXmlNodeByType(xmlRootNode, shipType, SShipTypesXml::s_shipTypeTag, s_valueAttr);
	if (!xmlCurrentShipType)
	{
		CRY_ASSERT(xmlCurrentShipType, "Can't find ship parameters for type '%s' in '%s'", shipType, s_shipTypesXmlPath);
		CryLogAlways("Can't find ship parameters for type '%s' in '%s'", shipType, s_shipTypesXmlPath);
		return false;
	}

	for (int i = 0, childCount(xmlCurrentShipType->getChildCount()); i < childCount; ++i)
	{
		XmlNodeRef xmlParameterNode = xmlCurrentShipType->getChild(i);
		const stack_string currTag = xmlParameterNode->getTag();
		if (currTag == SShipTypesXml::s_resourcePathTag)
		{
			shipParameters.resourcePath = GetStringAttr(xmlParameterNode, s_valueAttr);
			CryLog("Found ship resource path '%s'", shipParameters.resourcePath);
		}
		else if (currTag == SShipTypesXml::s_materialPathTag)
		{
			shipParameters.materialPath = GetStringAttr(xmlParameterNode, s_valueAttr);
			CryLog("Found ship material path '%s'", shipParameters.materialPath);
		}
		else if (currTag == SShipTypesXml::s_sizeParamsTag)
		{
			xmlParameterNode->getAttr(SShipTypesXml::s_shipSizeAttr, shipParameters.shipSize);
			xmlParameterNode->getAttr(SShipTypesXml::s_lenghtAttr, shipParameters.lenght);
			xmlParameterNode->getAttr(SShipTypesXml::s_breadthAttr, shipParameters.breadth);
			xmlParameterNode->getAttr(SShipTypesXml::s_maxDraughtAttr, shipParameters.maxDraught);
			xmlParameterNode->getAttr(SShipTypesXml::s_boardHeightAttr, shipParameters.boardHeight);
		}
		else if (currTag == SShipTypesXml::s_speedParamsTag)
		{
			xmlParameterNode->getAttr(SShipTypesXml::s_maxSpeedAttr, shipParameters.maxSpeed);
			xmlParameterNode->getAttr(SShipTypesXml::s_engineSpeedAttr, shipParameters.engineSpeed);
		}
		else if (currTag == SShipTypesXml::s_weightParamsTag)
		{
			xmlParameterNode->getAttr(SShipTypesXml::s_deadWeightAttr, shipParameters.deadWeight);
		}
		else if (currTag == s_descriptionTag)
		{
			shipParameters.description = xmlParameterNode->getContent();
			shipParameters.description = shipParameters.description.Trim();
		}
	}

	shipParameters.shipType = shipType;

	return true;
}

} // namespace Common
