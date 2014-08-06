#ifndef __MULE_GRID_H__
#define __MULE_GRID_H__

#include <QColor>
#include <QVector3D>
#include <QMatrix4x4>

typedef enum
{
	PLANE_AXIS_XY,
	PLANE_AXIS_XZ,
	PLANE_AXIS_YZ,
	PLANE_AXIS_UNKNOWN
}PLANE_AXIS;

class Grid
{
public:
	Grid( );
	~Grid( );

	int Initialise( const int p_Rows, const int p_Columns,
		const PLANE_AXIS p_Plane, const QColor &p_Colour, const float p_Offset,
		const float p_Stride );

	void Render( const QMatrix4x4 &p_ProjectionView ) const;

private:
	int			m_Rows;
	int			m_Columns;
	PLANE_AXIS	m_Plane;
	QColor		m_Colour;
	float		m_Offset;
	float		m_Stride;

	unsigned char		*m_pVertices;
	unsigned short int	*m_pIndices;
};

#endif // __MULE_GRID_H__

