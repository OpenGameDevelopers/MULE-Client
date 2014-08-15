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
class QOpenGLFunctions_3_0;

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

#pragma pack( 1 )
typedef struct __tagDATA_PACKET
{
	unsigned int	ID;
	unsigned char	Data[ 1020 ];
}DATA_PACKET;

typedef struct __tagIMAGE_LAYOUT
{
	int	Width;
	int	Height;
	int	Compression;
	int	ViewID;
}IMAGE_LAYOUT;

typedef struct __tagIMAGE_DATA_STREAM
{
	int				Offset;
	uint64_t		SequenceNumber;
	unsigned char	Data[ 1008 ];
}IMAGE_DATA_STREAM;

#pragma pack( )

class EditorViewport : public QWidget
{
	Q_OBJECT

public:
	EditorViewport( QWidget *p_pParent = 0 );
	~EditorViewport( );

	int Create( const ViewportType p_Type,
		QOpenGLFunctions_3_0 * const &p_GLFunctions );
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
	int							m_ViewID;

	GLuint					m_PositionAttribute;
	GLuint					m_TextureSamplerUniform;
	QOpenGLShaderProgram	*m_pProgram;
	QOpenGLFunctions_3_0	*m_pGLFunctions;
	float					m_Zoom;
	GLuint					m_TextureID;

	GLuint					m_VertexBuffer;
	GLuint					m_VertexArrayObject;

	bool					m_Panning;
	float					m_PanX;
	float					m_PanY;
	float					m_StoredPanX;
	float					m_StoredPanY;
	float					m_Scale;
	QPoint					m_MousePosition;

	QMatrix4x4				m_ProjectionMatrix;
	QMatrix4x4				m_ViewMatrix;

	int	m_Socket;

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

