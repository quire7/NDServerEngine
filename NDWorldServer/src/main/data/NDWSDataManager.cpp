#include "main/data/NDWSDataManager.h"



NDWSDataManager::NDWSDataManager()
{
	clear();
}

NDWSDataManager::~NDWSDataManager()
{
	release();
	clear();
}

void NDWSDataManager::clear()
{
	m_wsDBAccountInfoVec.clear();
}

NDBool NDWSDataManager::init()
{
	return NDTrue;
}

void NDWSDataManager::release()
{

}


