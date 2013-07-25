#include <Pane.hpp>

Pane::Pane( )
{
	m_pDisplay = NULL;
	m_pVisualInfo = NULL;
	m_GLVersion[ 0 ] = 0;
	m_GLVersion[ 1 ] = 0;
}

Pane::~Pane( )
{
	this->Destroy( );
}

int Pane::Initialise( )
{
	return 1;
}

void Pane::Destroy( )
{
}

void Pane::Update( )
{
}

void Pane::Render( )
{
}

