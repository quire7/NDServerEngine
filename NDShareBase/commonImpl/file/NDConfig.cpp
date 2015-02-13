#include "file/NDConfig.h"

#include <assert.h>
#include <stdarg.h>
#include "NDCLibFun.h"



//#define _CONFIG_DEBUG

_NDSHAREBASE_BEGIN

NDConfigFile::NDConfigFile()
{
}


NDConfigFile::~NDConfigFile()
{
	
}

void remove_spaces(string& str)
{
	while(str.size() && *str.begin() == ' ')
		str.erase(str.begin());
}

void remove_all_spaces(string& str)
{
	string::size_type off = str.find(" ");
	while(off != string::npos)
	{
		str.erase(off, 1);
		off = str.find(" ");
	}
}

NDBool is_comment(const string& str, NDBool * in_multiline_quote)
{
	string stemp = str;
	remove_spaces(stemp);
	if(stemp.length() == 0)
		return NDFalse;

	if(stemp[0] == '/')
	{
		if(stemp.length() < 2)
			return NDFalse;

		if(stemp[1] == '*')
		{
			*in_multiline_quote = NDTrue;
			return NDTrue;
		}
		else if(stemp[2] == '/')
		{
			return NDTrue;
		}
	}

	if(stemp[0] == '#')
		return NDTrue;

	return NDFalse;
}

void apply_setting(string & str, NDConfigFile::NDConfigSetting & setting)
{
	const char* pStrContent = str.c_str();

	setting.AsString= str;
	setting.AsInt	= (NDInt32)atoi( pStrContent );
	setting.AsBool	= (setting.AsInt > 0);
	setting.AsFloat = (NDFloat32)atof( pStrContent );

	/* check for verbal yes/no answers */
	if(str.length() > 1)
	{
		// this might be a yes/no?
		if(str.size() >= 3 && !ND_STRNICMP("yes", pStrContent, 3))
		{
			setting.AsBool = NDTrue;
			setting.AsInt = 1;
		}
		else if(str.size() >= 2 && !ND_STRNICMP("no", pStrContent, 2))
		{
			setting.AsBool = NDFalse;
			setting.AsInt = 0;
		}
	}
}

NDUint32 ahash(const char * str)
{
	register size_t len		= strlen(str);
	register NDUint32 ret	= 0;
	register size_t i		= 0;

	for(; i < len; ++i)
		ret += 5 * ret + (tolower(str[i]));

	/*printf("%s : %u\n", str, ret);*/
	return ret;
}

NDUint32 ahash(string& str)
{
	return ahash(str.c_str());
}

NDBool NDConfigFile::setSource(const char *file, NDBool ignorecase)
{
	ignorecase;
	/* wipe any existing settings. */
	m_settings.clear();

	/* open the file */
	if(file != 0)
	{
		FILE * f = fopen(file, "r");
		char * buf;
		int length;
		if(!f)
		{
			//sLog.outError("Could not open %s.", file);
			return NDFalse;
		}

		/* get the length of the file */
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		buf = new char[length + 1];
		memset( buf, 0, length + 1 );
		fseek(f, 0, SEEK_SET);

		fread(buf, length, 1, f);
		buf[length] = '\0';
		string buffer = string(buf);
		delete [] buf;
		buf = NULL;
		
		/* close the file, it is no longer needed */
		fclose(f);

		/* let's parse it. */
		string line;
		string::size_type end;
		string::size_type offset;
		NDBool in_multiline_comment = NDFalse;
		NDBool in_multiline_quote = NDFalse;
		NDBool in_block = NDFalse;
		string current_setting = "";
		string current_variable = "";
		string current_block = "";
		NDConfigBlock current_block_map;
		NDConfigSetting current_setting_struct;

		/* oh god this is awful */
		try {

		for(;;)
		{
			/* grab a line. */
			end = buffer.find("\n");
			//if(end == string::npos)
			//	break;
			if(end == string::npos)
			{	//这个括号内的东西时我改的,或许原来就是就是一种格式(必须在文件末尾加一空行);
				//而改过之后就不需要了!;
				end = buffer.length();
				if ( end > 0 )
				{
					line = buffer;
					buffer.clear();
					goto parse;
				}
				else
				{
					break;
				}
			}

			line = buffer.substr(0, end);
			buffer.erase(0, end+1);
			goto parse;

parse:
			if(!line.size())
				continue;

			/* are we a comment? */
			if(!in_multiline_comment && is_comment(line, &in_multiline_comment))
			{
				/* our line is a comment. */
				if(!in_multiline_comment)
				{
					/* the entire line is a comment, skip it. */
					continue;
				}
			}

			/* handle our cases */
			if(in_multiline_comment)
			{
				// we need to find a "*/".
				offset = line.find("*/", 0);
				
				/* skip this entire line, eh? */
				if(offset == string::npos)
					continue;

				/* remove up to the end of the comment block. */
				line.erase(0, offset + 2);
				in_multiline_comment = NDFalse;
			}
		
			if(in_block)
			{
				/* handle settings across multiple lines */
				if(in_multiline_quote)
				{
					/* attempt to find the end of the quote block. */
					offset = line.find("\"");

					if(offset == string::npos)
					{
						/* append the whole line to the quote. */
						current_setting += line;
						current_setting += "\n";
						continue;
					}
	                
					/* only append part of the line to the setting. */
					current_setting.append(line.c_str(), offset+1);
					line.erase(0, offset + 1);
				
					/* append the setting to the config block. */
					if(current_block == "" || current_variable == "")
					{
						//sLog.outError("Quote without variable.");
						return NDFalse;
					}

					/* apply the setting */
					apply_setting(current_setting, current_setting_struct);

					/* the setting is done, append it to the current block. */
                    current_block_map[ahash(current_variable)] = current_setting_struct;
#ifdef _CONFIG_DEBUG
					printf("Block: '%s', Setting: '%s', Value: '%s'\n", current_block.c_str(), current_variable.c_str(), current_setting_struct.AsString.c_str());
#endif
					/* no longer doing this setting, or in a quote. */
					current_setting = "";
					current_variable = "";
                    in_multiline_quote = NDFalse;					
				}

				/* remove any leading spaces */
				remove_spaces(line);

				if(!line.size())
					continue;

				/* our target is a *setting*. look for an '=' sign, this is our seperator. */
                offset = line.find("=");
				if(offset != string::npos)
				{
					assert(current_variable == "");
					current_variable = line.substr(0, offset);

					/* remove any spaces from the end of the setting */
					remove_all_spaces(current_variable);

					/* remove the directive *and* the = from the line */
					line.erase(0, offset + 1);
				}

				/* look for the opening quote. this signifies the start of a setting. */
				offset = line.find("\"");
				if(offset != string::npos)
				{
					assert(current_setting == "");
					assert(current_variable != "");

					/* try and find the ending quote */
					end = line.find("\"", offset + 1);
					if(end != string::npos)
					{
						/* the closing quote is on the same line, oh goody. */
						current_setting = line.substr(offset+1, end-offset-1);

						/* erase up to the end */
						line.erase(0, end + 1);
						
						/* apply the setting */
						apply_setting(current_setting, current_setting_struct);

						/* the setting is done, append it to the current block. */
						current_block_map[ahash(current_variable)] = current_setting_struct;

#ifdef _CONFIG_DEBUG
						printf("Block: '%s', Setting: '%s', Value: '%s'\n", current_block.c_str(), current_variable.c_str(), current_setting_struct.AsString.c_str());
#endif
						/* no longer doing this setting, or in a quote. */
						current_setting = "";
						current_variable = "";
						in_multiline_quote = NDFalse;		

						/* attempt to grab more settings from the same line. */
						goto parse;
					}
					else
					{
						/* the closing quote is not on the same line. means we'll try and find it on
						   the next. */
						current_setting.append(line.c_str(), offset);

						/* skip to the next line. (after setting our condition first, of course :P */
						in_multiline_quote = NDTrue;
						continue;
					}
				}

				/* are we at the end of the block yet? */
				offset = line.find(">");
				if(offset != string::npos)
				{
					line.erase(0, offset+1);

					// freeeee!;
					in_block = NDFalse;
					
					/* assign this block to the main "big" map. */
					m_settings[ahash(current_block)] = current_block_map;

					/* erase all data for this so it doesn't seep through */
					current_block_map.clear();
					current_setting = "";
					current_variable = "";
					current_block = "";
				}
			}
			else
			{
				/* we're not in a block. look for the start of one. */
				offset = line.find("<");

				if(offset != string::npos)
				{
					in_block = NDTrue;

					/* whee, a block! let's cut the string and re-parse. */
					line.erase(0, offset + 1);

					/* find the name of the block first, though. */
					offset = line.find(" ");
					if(offset != string::npos)
					{
						current_block = line.substr(0, offset);
						line.erase(0, offset + 1);
					}
					else
					{
						//sLog.outError("Block without name.");
						return NDFalse;
					}

					/* skip back */
					goto parse;
				}
			}
		}

		}catch(...)
			{
				//sLog.outError("Exception in config parsing.");
				return NDFalse;
			}

		/* handle any errors */
		if(in_block)
		{
			//sLog.outError("Unterminated block.");
			return NDFalse;
		}

		if(in_multiline_comment)
		{
			//sLog.outError("Unterminated comment.");
			return NDFalse;
		}

		if(in_multiline_quote)
		{
			//sLog.outError("Unterminated quote.");
			return NDFalse;
		}

		/* we're all good :) */
		return NDTrue;
	}

	return NDFalse;
}

NDConfigFile::NDConfigSetting * NDConfigFile::getSetting( const char* Block, const char* Setting )
{
	NDUint32 block_hash		= ahash(Block);
	NDUint32 setting_hash	= ahash(Setting);

	/* find it in the big map */
	map<NDUint32, NDConfigBlock>::iterator itr = m_settings.find(block_hash);
	if(itr != m_settings.end())
	{
		NDConfigBlock::iterator it2 = itr->second.find(setting_hash);
		if(it2 != itr->second.end())
			return &(it2->second);

		return 0;
	}

	return 0;
}

NDBool NDConfigFile::getString(const char * block, const char* name, string *value)
{
	NDConfigSetting * Setting = getSetting(block, name);
	if(Setting == 0)
		return NDFalse;

	*value = Setting->AsString;
	return NDTrue;
}


string NDConfigFile::getStringDefault(const char * block, const char* name, const char* def)
{
	string ret;
	return getString(block, name, &ret) ? ret : def;
}


NDBool NDConfigFile::getBool(const char * block, const char* name, NDBool *value)
{
	NDConfigSetting * Setting = getSetting(block, name);
	if(Setting == 0)
		return NDFalse;

	*value = Setting->AsBool;
	return NDTrue;
}


NDBool NDConfigFile::getBoolDefault(const char * block, const char* name, const NDBool def /* = NDFalse */)
{
	NDBool val;
	return getBool(block, name, &val) ? val : def;
}

NDBool NDConfigFile::getInt(const char * block, const char* name, NDInt32 *value)
{
	NDConfigSetting * Setting = getSetting(block, name);
	if(Setting == 0)
		return NDFalse;

	*value = Setting->AsInt;
	return NDTrue;
}

NDBool NDConfigFile::getFloat(const char * block, const char* name, NDFloat32 *value)
{
	NDConfigSetting * Setting = getSetting(block, name);
	if(Setting == 0)
		return NDFalse;

	*value = Setting->AsFloat;
	return NDTrue;
}

NDInt32 NDConfigFile::getIntDefault(const char * block, const char* name, const NDInt32 def)
{
	NDInt32 val;
	return getInt(block, name, &val) ? val : def;
}

NDFloat32 NDConfigFile::getFloatDefault(const char * block, const char* name, const NDFloat32 def)
{
	NDFloat32 val;
	return (getFloat(block, name, &val) ? val : def);
}

NDInt32 NDConfigFile::getIntVA(const char * block, NDInt32 def, const char* name, ...)
{
	va_list ap;
	va_start(ap, name);
	char str[150]={0};
	ND_VSNPRINTF(str, 149, name, ap);
	va_end(ap);
	NDInt32 val;
	return getInt(str, block, &val) ? val : def;
}

NDFloat32 NDConfigFile::getFloatVA(const char * block, NDFloat32 def, const char* name, ...)
{
	va_list ap;
	va_start(ap, name);
	char str[150]={0};
	ND_VSNPRINTF(str, 149, name, ap);
	va_end(ap);
	NDFloat32 val;
	return getFloat(str, block, &val) ? val : def;
}

string NDConfigFile::getStringVA(const char * block, const char* def, const char * name, ...)
{
	va_list ap;
	va_start(ap, name);
	char str[150]={0};
	ND_VSNPRINTF(str, 149, name, ap);
	va_end(ap);

	return getStringDefault(str, block, def);
}

NDBool NDConfigFile::getString(const char * block, char * buffer, const char * name, const char * def, NDUint32 len)
{
	string val = getStringDefault(block, name, def);
	size_t blen = val.length();
	if(blen > len)
		blen = len;

	memcpy(buffer, val.c_str(), blen);
	buffer[blen] = 0;
    
	return NDTrue;
}


_NDSHAREBASE_END

