/********************************************************************************
 * How to use this config file:                                                 *
 * Config files are in a block->variable->setting format.                       *
 * A < signifies the opening of a block, the block name is expected after that. * 
 * After the block name, a space and then the settings sperated by = / ' '.     *
 *                                                                              *
 * e.g.                                                                         *
 * <block setting="0" someval = "1">                                            *
 *                                                                              *
 *******************************************************************************/

#ifndef __SHARE_BASE_ND_CONFIG_H__
#define __SHARE_BASE_ND_CONFIG_H__

#include <string>
using std::string;

#include <map>
using std::map;

#include "../NDShareBase/include/common/NDTypes.h"

class NDConfigFile
{
public:
	struct NDConfigSetting
	{
		NDBool		AsBool;
		NDInt32		AsInt;
		NDFloat32	AsFloat;
		string		AsString;
	};

	typedef map< NDUint32, NDConfigSetting > NDConfigBlock;

public:
	NDConfigFile();
	~NDConfigFile();

	NDBool setSource( const char* file, NDBool ignorecase = NDTrue );
	

	NDBool getString( const char* block, const char* name, string* value );
	string getStringDefault( const char* block, const char* name, const char* def );
	string getStringVA( const char* block, const char* def, const char* name, ... );
	NDBool getString( const char * block, char* buffer, const char* name, const char* def, NDUint32 len );

	NDBool getBool( const char* block, const char* name, NDBool *value);
	NDBool getBoolDefault( const char* block, const char* name, const NDBool def);

	NDBool	getInt( const char* block, const char* name, NDInt32* value );
	NDInt32 getIntDefault( const char* block, const char* name, const NDInt32 def );
	NDInt32 getIntVA( const char* block, NDInt32 def, const char* name, ... );

	NDBool	  getFloat( const char* block, const char* name, NDFloat32* value );
	NDFloat32 getFloatDefault( const char* block, const char* name, const NDFloat32 def );
	NDFloat32 getFloatVA( const char* block, NDFloat32 def, const char* name, ... );

private:

	NDConfigSetting* getSetting( const char* Block, const char* Setting );

private:
	map<NDUint32, NDConfigBlock> m_settings;
};


#endif
