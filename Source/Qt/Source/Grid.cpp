#include <Grid.h>
#include <Utility.h>

const char *pGridVertexShaderGL2 =
{
	"uniform highp mat4 u_ProjectionViewWorld;\n"
	"uniform lowp vec4 u_Colour;\n"
	"attribute highp vec3 a_Position;\n"
	"varying lowp vec4 v_Colour;\n"
	"void main( )\n"
	"{\n"
	"	v_Colour = u_Colour;\n"
	"	gl_Position = u_ProjectionViewWorld * vec4( a_Position, 1.0 );\n"
	"}\n"
};

const char *pGridFragmentShaderGL2 =
{
	"varying lowp vec4 v_Colour;\n"
	"void main( )\n"
	"{\n"
	"	gl_FragColor = v_Colour;\n"
	"}\n"
};

Grid::Grid( ) :
	m_Rows( 0 ),
	m_Columns( 0 ),
	m_Plane( PLANE_AXIS_UNKNOWN ),
	m_Offset( 0.0f ),
	m_Stride( 0.0f ),
	m_pVertices( nullptr ),
	m_pIndices( nullptr )
{
}

Grid::~Grid( )
{
	SafeDeleteArray( m_pVertices );
	SafeDeleteArray( m_pIndices );
}

