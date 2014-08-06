#include <MainWindow.h>
#include <QBoxLayout>
#include <QStatusBar>
#include <QAction>
#include <QMenuBar>
#include <QSplitter>
#include <QContextMenuEvent>
#include <GitVersion.h>
#include <cstring>
#include <QtGui/QOpenGLContext>
#include <QOffscreenSurface>
#include <Utility.h>
#include <EditorViewportManager.h>
#include <EditorViewport.h>
#include <QOpenGLFunctions>

MainWindow::MainWindow( )
{

}

MainWindow::~MainWindow( )
{
	SafeDelete( m_pOpenGLSurface );
	SafeDelete( m_pSplitter );
}

int MainWindow::Initialise( )
{
	m_pOpenGLSurface = new QOffscreenSurface( );
	m_pOpenGLSurface->create( );

	m_pGLContext = new QOpenGLContext( );
	m_pGLContext->create( );
	m_pGLContext->makeCurrent( m_pOpenGLSurface );

	m_pOpenGLFunctions = new QOpenGLFunctions( m_pGLContext );
	m_pOpenGLFunctions->initializeOpenGLFunctions( );

	if( m_pGLContext->isValid( ) == false )
	{
		return 1;
	}
    QWidget *pCentral = new QWidget( );
    setCentralWidget( pCentral );

    m_pLayout = new QVBoxLayout( );
    m_pLayout->setMargin( 0 );
	m_pLayout->setSpacing( 0 );

	this->Initialise3DSplitter( );

	pCentral->setLayout( m_pLayout );

    this->CreateActions( );
    this->CreateMenus( );

    QString Message = tr( "Waiting..." );
    statusBar( )->showMessage( Message );

	char Title[ 1000 ];
	memset( Title, '\0', sizeof( Title ) );
	sprintf( Title, "MULE [%s] %s //Build date: %s%s",
		GIT_BUILD_VERSION, GIT_COMMITHASH, GIT_COMMITTERDATE,
#if defined BUILD_DEBUG
		" [DEBUG]"
#else
		""
#endif // BUILD_DEBUG
		);
    setWindowTitle( tr( Title ) );
    setMinimumSize( 640, 480 );
    resize( 1024, 768 );

	return 0;
}

void MainWindow::CreateActions( )
{
    m_pQuitAction = new QAction( tr( "&Quit" ), this );
    m_pQuitAction->setShortcuts( QKeySequence::Quit );
    connect( m_pQuitAction, SIGNAL( triggered( ) ), this, SLOT( close( ) ) );
}

void MainWindow::CreateMenus( )
{
    m_pFileMenu = menuBar( )->addMenu( tr( "&File" ) );
    m_pFileMenu->addAction( m_pQuitAction );
}

void MainWindow::Initialise3DSplitter( )
{
	m_pSplitter = new QSplitter;
	m_pViewportContainer = new QWidget( );
	m_pViewportManager = new EditorViewportManager( m_pGLContext );
	EditorViewport *pTmpViewport = new EditorViewport( );
	EditorViewport *pTmpViewport2 = new EditorViewport( );
//	EditorViewport *pTmpViewport3 = new EditorViewport( );

	m_pSplitter->addWidget( pTmpViewport );
	m_pSplitter->addWidget( pTmpViewport2 );
	m_pLayout->addWidget( m_pSplitter );
/*
	m_pLayout->addWidget( m_pSplitter );
	m_pViewportContainer->setLayout( m_pLayout );

	QSplitter *Splitter2 = new QSplitter;
	Splitter2->setOrientation( Qt::Vertical );
	Splitter2->addWidget( m_pViewportContainer );
	Splitter2->addWidget( pTmpViewport3 );

	m_pLayout->addWidget( Splitter2 );*/

	pTmpViewport->Create( ViewportPerspective, m_pOpenGLFunctions );
	pTmpViewport2->Create( ViewportOrthographic, m_pOpenGLFunctions );
//	pTmpViewport3->Create( ViewportOrthographic, m_pOpenGLFunctions );
}

