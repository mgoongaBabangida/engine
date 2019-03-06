#pragma once
#include <QtWidgets/qwidget.h>
#include <SharedData.h>

class QSlider;
class dbGLWindow;
class DebugSlider;

class dbWidget:public QWidget
{
    Q_OBJECT

	dbGLWindow* m_window;
	DebugSlider* slider1;
	DebugSlider* slider2;
	DebugSlider* slider3;
	DebugSlider* slider4;
	DebugSlider* slider5;
	DebugSlider* slider6;
	SharedData m_data;
private slots:
	void sliderValueChanged();
public:
	dbWidget();
	~dbWidget();
};

