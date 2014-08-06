#include <MainWindow.h>
#include <QApplication>

int main( int p_Argc, char **p_ppArgv )
{
    QApplication Application( p_Argc, p_ppArgv );
    MainWindow Window;

	if( Window.Initialise( ) != 0 )
	{
		return 1;
	}

    Window.show( );
    return Application.exec( );
}

