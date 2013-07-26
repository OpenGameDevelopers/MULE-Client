#ifndef __MULECLIENT_UIELEMENT_HPP__
#define __MULECLIENT_UIELEMENT_HPP__

typedef struct __tagPoint
{
	int X;
	int Y;
}Point;

typedef struct __tagDimension
{
	int Width;
	int Height;
}Dimension;

class UIElement
{
public:
	virtual ~UIElement( ) { }

	virtual void Position( const Point &p_Point )
		{ m_Point.X = p_Point.X; m_Point.Y = p_Point.Y; }
	virtual Point Position( ) const
		{ return m_Point; }

	virtual void Dimensions( const Dimension &p_Dimension )
		{ m_Dimension.Width = p_Dimension.Width;
		m_Dimension.Height = p_Dimension.Height; }
	virtual Dimension Dimensions( ) const
		{ return m_Dimension; }
	
	virtual int Initialise( ) = 0;
	virtual void Destroy( ) = 0;

protected:
	Point		m_Point;
	Dimension	m_Dimension;
};

#endif

