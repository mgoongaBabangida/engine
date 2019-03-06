#ifndef DB_GL_WINDOW
#define DB_GL_WINDOW

#include <GL\glew.h>
#include <QtOpenGL\qglwidget>
#include "MainContext.h"
#include "InputController.h"
#include "Timer.h"

class QMouseEvent;
class SharedData;

class dbGLWindow : public QGLWidget
{
public:
	dbGLWindow(SharedData* data);
	virtual ~dbGLWindow();

	//Events
	virtual void mouseMoveEvent(QMouseEvent* e)		override;
	virtual void keyPressEvent(QKeyEvent* e)		override;
	virtual void mousePressEvent(QMouseEvent * e)	override;
	virtual void mouseReleaseEvent(QMouseEvent * e) override;

private:
	std::unique_ptr<dbb::Timer>			dTimer;
	eMainContext						mainContext;
	eInputController					inputController;
	SharedData*							m_sData; //ptr

protected:
	virtual void	initializeGL()  override;
	virtual void	paintGL()		override;

	void			updateSharedData();
};

#endif