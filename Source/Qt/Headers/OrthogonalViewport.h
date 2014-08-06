#ifndef __MULE_ORTHOGONALVIEWPORT_H__
#define __MULE_ORTHOGONALVIEWPORT_H__

#include <EditorViewport.h>

class OrthographicViewport : public EditorViewport
{
	Q_OBJECT
public:
	OrthographicViewport( QWidget *p_pParent = 0 );
	~OrthographicViewport( );

private:
};

#endif // __MULE_ORTHOGOONALVIEWPORT_H__

