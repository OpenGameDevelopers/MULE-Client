#ifndef __GUNSLINGERED_ORTHOGONALVIEWPORT_H__
#define __GUNSLINGERED_ORTHOGONALVIEWPORT_H__

#include <EditorViewport.h>

class OrthographicViewport : public EditorViewport
{
	Q_OBJECT
public:
	OrthographicViewport( QWidget *p_pParent = 0 );
	~OrthographicViewport( );

private:
};

#endif // __GUNSLINGERED_ORTHOGOONALVIEWPORT_H__

