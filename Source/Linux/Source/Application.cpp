#include <Application.hpp>
#include <iostream>

Application::Application( )
{
}

Application::~Application( )
{
}

int Application::Initialise( )
{
	if( m_Pane.Initialise( ) == 0 )
	{
		std::cout << "Failed to initialise pane" << std::endl;
		return 0;
	}

	return 1;
}

void Application::Update( )
{
	m_Pane.Update( );
}

int Application::Execute( )
{
	this->Update( );
	return 1;
}

