#ifndef __MULECLIENT_APPLICAITON_HPP__
#define __MULECLIENT_APPLICATION_HPP__

#include <Pane.hpp>

class Application
{
public:
	Application( );
	~Application( );

	int Initialise( );
	void Update( );
	int Execute( );

private:
	Pane	m_Pane;
};

#endif

