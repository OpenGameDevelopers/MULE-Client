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

	virtual void Position( const Point &p_Point ) = 0;
	virtual Point Position( ) const = 0;

	virtual void Dimensions( const Dimension &p_Dimension ) = 0;
	virtual Deimension Dimensions( ) const = 0;
};

#endif

