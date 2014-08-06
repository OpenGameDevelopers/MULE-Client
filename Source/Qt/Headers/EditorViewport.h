#ifndef __MULE_EDITORVIEWPORT_H__
#define __MULE_EDITORVIEWPORT_H__

#include <QColor>
#include <QImage>
#include <QSize>
#include <QWidget>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QMatrix4x4>

class QOpenGLFramebufferObject;
class QPaintEvent;
class QResizeEvent;
class QOpenGLShaderProgram;
class QOpenGLFunctions;

typedef enum __ViewportType
{
	ViewportOrthographic,
	ViewportPerspective
}ViewportType;

typedef enum __ViewportSide
{
	ViewportXZ,
	ViewportXY,
	ViewportYZ,
	ViewportFreeMoving
}ViewportSide;

class EditorViewport : public QWidget
{
	Q_OBJECT

public:
	EditorViewport( QWidget *p_pParent = 0 );
	~EditorViewport( );

	int Create( const ViewportType p_Type,
		QOpenGLFunctions * const &p_GLFunctions );
	int Resize( const int p_Width, const int p_Height );

	int SetType( const ViewportType p_Type );
	int SetClearColour( const float p_Red, const float p_Green,
		const float p_Blue );

	int GetImage( QImage *p_pImage );

	void Activate( );
	void Deactivate( );

	void Render( );

protected:
	QOpenGLFramebufferObject	*m_pFramebuffer;
	QSize						m_Size;
	ViewportType				m_Type;
	ViewportSide				m_Side;
	float						m_RedClear;
	float						m_GreenClear;
	float						m_BlueClear;

	GLuint					m_PositionAttribute;
	GLuint					m_ColourAttribute;
	GLuint					m_MatrixUniform;
	QOpenGLShaderProgram	*m_pProgram;
	QOpenGLFunctions		*m_pGLFunctions;
	float					m_Zoom;

	bool					m_Panning;
	float					m_PanX;
	float					m_PanY;
	float					m_StoredPanX;
	float					m_StoredPanY;
	float					m_Scale;
	QPoint					m_MousePosition;

	QMatrix4x4				m_ProjectionMatrix;
	QMatrix4x4				m_ViewMatrix;

	void RecreateProjectionMatrix( );
	void paintEvent( QPaintEvent *p_pPaintEvent );
	void resizeEvent( QResizeEvent *p_pResizeEvent );
	virtual void wheelEvent( QWheelEvent *p_pWheelEvent );
	virtual void mousePressEvent( QMouseEvent *p_pMouseEvent );
	virtual void mouseReleaseEvent( QMouseEvent *p_pMouseEvent );
	virtual void mouseMoveEvent( QMouseEvent *p_pMouseEvent );
};

char *GetNameFromViewport( const ViewportType p_Type,
	const ViewportSide p_Side );

#endif // __MULE_EDITORVIEWPORT_H__
