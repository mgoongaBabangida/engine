#include "dbGLWindiw.h"
#include "SharedData.h"

dbGLWindow::dbGLWindow(SharedData* data) 
	:mainContext(), 
	inputController(&mainContext), 
	m_sData(data)
{
	QGLWidget::setFormat(QGLFormat(QGL::SampleBuffers));

	dTimer.reset(new dbb::Timer([this]()->bool {this->update(); return true;}));
	dTimer->start(100);
}

dbGLWindow::~dbGLWindow()
{}
////======================================
////dbGLWindow::initializeGL
////---------------------------------------
void dbGLWindow::initializeGL()
{
	mainContext.InitializeGL();
	//QWidget 
	setMinimumSize(1200, 600);
	setMouseTracking(false);
	this->move(100, 70);
	resize(1200, 600);
}
//======================================
//dbGLWindow::paintGL
//---------------------------------------
void dbGLWindow::paintGL()
{
	this->setFocus();
	updateSharedData();
	mainContext.PaintGL();
}
//======================================
//dbGLWindow::updateSharedData
//---------------------------------------
void dbGLWindow::updateSharedData()
{
	mainContext.UpdateLight(m_sData->vec_data[0].x, m_sData->vec_data[0].y, m_sData->vec_data[0].z);
	this->setFocus();
}