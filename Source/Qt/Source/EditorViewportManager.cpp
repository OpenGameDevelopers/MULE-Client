#include <EditorViewportManager.h>
#include <EditorViewport.h>

EditorViewportManager::EditorViewportManager(
	QOpenGLContext * const &p_pGLContext ) :
	m_pGLContext( p_pGLContext )
{
}

EditorViewportManager::~EditorViewportManager( )
{
}

int EditorViewportManager::Create( const ViewportType p_Type )
{
/*	EditorViewport *pViewport = new EditorViewport( );

	if( pViewport->Create( p_Type ) != 0 )
	{
		return 1;
	}*/

	return 0;
}

