#include <iostream>
#include <Pane.hpp>
#include <unistd.h>

int main( int p_Argc, char **p_ppArgv )
{
	Pane MainPane;

	MainPane.Initialise( );
	sleep( 3 );
	
	return 0;
}

