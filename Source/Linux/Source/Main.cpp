#include <iostream>
#include <Application.hpp>

int main( int p_Argc, char **p_ppArgv )
{
	Application Client;

	if( Client.Initialise( ) == 0 )
	{
		std::cout << "Failed to initialise the client" << std::endl;
		return 0;
	}

	return Client.Execute( );
}

