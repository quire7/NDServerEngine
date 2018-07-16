#include "main/data/NDWSPlayer.h"



NDWSPlayer::NDWSPlayer()
{
	clear();
}

NDWSPlayer::~NDWSPlayer()
{
	release();
	clear();
}

void NDWSPlayer::clear()
{

}

NDBool NDWSPlayer::init( const NDPlayerMain& refPlayerMain )
{
	m_playerMain = refPlayerMain;

	return NDTrue;
}

void NDWSPlayer::release()
{
	
}
