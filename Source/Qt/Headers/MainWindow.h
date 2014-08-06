#ifndef MULE_MAINWINDOW_H
#define MULE_MAINWINDOW_H

#include <QMainWindow>

class QOpenGLContext;
class QOffscreenSurface;
class QSplitter;
class EditorViewportManager;
class QOpenGLFunctions;
class QVBoxLayout;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow( );
    ~MainWindow( );

	int Initialise( );

private:
    void CreateActions( );
    void CreateMenus( );
	void Initialise3DSplitter( );

    QMenu					*m_pFileMenu;
    QAction					*m_pQuitAction;
	QOpenGLContext			*m_pGLContext;
	QOffscreenSurface		*m_pOpenGLSurface;
	QOpenGLFunctions		*m_pOpenGLFunctions;
	QVBoxLayout				*m_pLayout;
	QSplitter				*m_pSplitter;
	QWidget					*m_pViewportContainer;
	EditorViewportManager	*m_pViewportManager;
};

#endif // MULE_MAINWINDOW_H

