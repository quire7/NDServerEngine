#include "file/NDXmlConfigBase.h"

#include "tinyxml/tinyxml.h"

_NDSHAREBASE_BEGIN

NDXmlConfigBase::NDXmlConfigBase(void)
{
}

NDXmlConfigBase::~NDXmlConfigBase(void)
{
}

const char* NDXmlConfigBase::getElement( const TiXmlElement *pXmlElement, const char* pszFlag )
{
	if ((NULL == pXmlElement) || (NULL == pszFlag))	return NULL;

	const char* pElementName = pXmlElement->Value();
	if (0 == strncmp(pElementName, pszFlag, strlen(pElementName)))
	{
		const TiXmlNode* pElementNode = pXmlElement->FirstChild();
		if (NULL != pElementNode)
		{
			return pElementNode->Value();
		}
	}

	return NULL;
}

const char* NDXmlConfigBase::getElement2(const TiXmlElement *pParentXmlElement, const char* pszFlag)
{
	if ((NULL == pParentXmlElement) || (NULL == pszFlag))	return NULL;

	const TiXmlElement *pXmlElement = pParentXmlElement->FirstChildElement(pszFlag);
	if (NULL != pXmlElement)
	{
		const TiXmlNode* pElementNode = pXmlElement->FirstChild();
		if (NULL != pElementNode)
		{
			return pElementNode->Value();
		}
	}
	return NULL;
}

NDBool NDXmlConfigBase::getElement(const TiXmlElement *pXmlElement, const char* pszFlag, NDInt32& rIntValue)
{
	const char* pElementContent = getElement(pXmlElement, pszFlag);
	if (NULL != pElementContent)
	{
		rIntValue = atoi(pElementContent);
		return NDTrue;
	}
	return NDFalse;
}

NDBool NDXmlConfigBase::getElement2(const TiXmlElement *pParentXmlElement, const char* pszFlag, NDInt32& rIntValue)
{
	const char* pElementContent = getElement2(pParentXmlElement, pszFlag);
	if (NULL != pElementContent)
	{
		rIntValue = atoi(pElementContent);
		return NDTrue;
	}
	return NDFalse;
}


NDBool NDXmlConfigBase::getElement(const TiXmlElement *pXmlElement, const char* pszFlag, NDInt64& rLongValue)
{
	const char* pElementContent = getElement(pXmlElement, pszFlag);
	if (NULL != pElementContent)
	{
		rLongValue = atol(pElementContent);
		return NDTrue;
	}
	return NDFalse;
}

NDBool NDXmlConfigBase::getElement2(const TiXmlElement *pParentXmlElement, const char* pszFlag, NDInt64& rLongValue)
{
	const char* pElementContent = getElement2(pParentXmlElement, pszFlag);
	if (NULL != pElementContent)
	{
		rLongValue = atol(pElementContent);
		return NDTrue;
	}
	return NDFalse;
}

NDBool NDXmlConfigBase::getElement(const TiXmlElement *pXmlElement, const char* pszFlag, NDFloat64& rDoubleValue)
{
	const char* pElementContent = getElement(pXmlElement, pszFlag);
	if (NULL != pElementContent)
	{
		rDoubleValue = atof(pElementContent);
		return NDTrue;
	}
	return NDFalse;
}

NDBool NDXmlConfigBase::getElement2(const TiXmlElement *pParentXmlElement, const char* pszFlag, NDFloat64& rDoubleValue)
{
	const char* pElementContent = getElement2(pParentXmlElement, pszFlag);
	if (NULL != pElementContent)
	{
		rDoubleValue = atof(pElementContent);
		return NDTrue;
	}
	return NDFalse;
}

NDBool NDXmlConfigBase::getElement( const TiXmlElement *pXmlElement, const char* pszFlag, vector<const char*>& rFlagNameVec, vector<const char*>& rContentVec)
{
	if ((NULL == pXmlElement) || (NULL == pszFlag))	return NDFalse;

	const char* pElementName = pXmlElement->Value();
	if (0 == strncmp(pElementName, pszFlag, strlen(pElementName)))
	{
		size_t nSize = rFlagNameVec.size();
		for (size_t i = 0; i < nSize; ++i)
		{
			const char* pContent = pXmlElement->Attribute(rFlagNameVec[i]);
			if (NULL != pContent)
			{
				rContentVec.push_back(pContent);
			}
		}
		return NDTrue;
	}

	return NDFalse;
}


NDBool NDXmlConfigBase::getElement(const TiXmlElement *pXmlElement, const char* pszFlag, vector<const char*>& rFlagNameVec, vector<NDInt32>& rIntVec)
{
	if ((NULL == pXmlElement) || (NULL == pszFlag))	return NDFalse;

	const char* pElementName = pXmlElement->Value();
	if (0 == strncmp(pElementName, pszFlag, strlen(pElementName)))
	{
		size_t nSize = rFlagNameVec.size();
		for (size_t i = 0; i < nSize; ++i)
		{
			const char* pContent = pXmlElement->Attribute(rFlagNameVec[i]);
			if (NULL != pContent)
			{
				rIntVec.push_back( atoi(pContent) );
			}
		}
		return NDTrue;
	}

	return NDFalse;
}


NDBool NDXmlConfigBase::getElement2(const TiXmlElement *pParentXmlElement, const char* pszFlag, vector<const char*>& rFlagNameVec, vector<const char*>& rContentVec)
{
	if ((NULL == pParentXmlElement) || (NULL == pszFlag))	return NDFalse;

	const TiXmlElement *pXmlElement = pParentXmlElement->FirstChildElement(pszFlag);
	while (NULL != pXmlElement)
	{
		size_t nSize = rFlagNameVec.size();
		for (size_t i = 0; i < nSize; ++i)
		{
			const char* pContent = pXmlElement->Attribute(rFlagNameVec[i]);
			if (NULL != pContent)
			{
				rContentVec.push_back(pContent);
			}
		}

		pXmlElement = pParentXmlElement->NextSiblingElement(pszFlag);
	}

	return NDTrue;
}

NDBool NDXmlConfigBase::getElement2(const TiXmlElement *pParentXmlElement, const char* pszFlag, vector<const char*>& rFlagNameVec, vector<NDInt32>& rIntVec)
{
	if ((NULL == pParentXmlElement) || (NULL == pszFlag))	return NDFalse;

	const TiXmlElement *pXmlElement = pParentXmlElement->FirstChildElement(pszFlag);
	while (NULL != pXmlElement)
	{
		size_t nSize = rFlagNameVec.size();
		for (size_t i = 0; i < nSize; ++i)
		{
			const char* pContent = pXmlElement->Attribute(rFlagNameVec[i]);
			if (NULL != pContent)
			{
				rIntVec.push_back( atoi(pContent) );
			}
		}

		pXmlElement = pXmlElement->NextSiblingElement(pszFlag);
	}

	return NDTrue;
}


NDBool NDXmlConfigBase::readXmlConfig( const char *pFileName )
{
	if ( NULL == pFileName || '\0' == pFileName[0] )
	{
		return NDFalse;
	}
	//创建一个XML的文档对象;
	TiXmlDocument xmlDocument( pFileName );
	if ( !xmlDocument.LoadFile() )
	{
		return NDFalse;
	}

	//获得根元素;
	const TiXmlElement *pRootElement = xmlDocument.RootElement();
	if ( NULL == pRootElement )
	{
		return NDFalse;
	}

	return readXmlConfigContent( pRootElement );
}

_NDSHAREBASE_END


