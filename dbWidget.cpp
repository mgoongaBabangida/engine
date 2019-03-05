#include "dbWidget.h"
#include <QtWidgets\QVBoxLayout>
#include <QtWidgets\QHBoxLayout>
#include <QtWidgets\qslider.h>
#include "dbGLWindiw.h"
#include "DebugSlider.h"

dbWidget::dbWidget()
{
	this->setMinimumSize(1300, 650);
	QVBoxLayout* mainLayout;
	setLayout(mainLayout = new QVBoxLayout);
	QVBoxLayout* controlsLayout;
	mainLayout->addLayout(controlsLayout = new QVBoxLayout);
	mainLayout->addWidget(m_window=new dbGLWindow(&m_data));

	QHBoxLayout* lightLayout;
	controlsLayout->addLayout(lightLayout = new QHBoxLayout);

	lightLayout->addWidget(slider1=new DebugSlider);
	lightLayout->addWidget(slider2 = new DebugSlider);
	lightLayout->addWidget(slider3 = new DebugSlider);
	lightLayout->addWidget(slider4 = new DebugSlider);
	lightLayout->addWidget(slider5 = new DebugSlider);
	lightLayout->addWidget(slider6 = new DebugSlider);
	
	connect(slider1, SIGNAL(valueChanged(float)), this, SLOT(sliderValueChanged()));
	connect(slider2, SIGNAL(valueChanged(float)), this, SLOT(sliderValueChanged()));
	connect(slider3, SIGNAL(valueChanged(float)), this, SLOT(sliderValueChanged()));
	connect(slider4, SIGNAL(valueChanged(float)), this, SLOT(sliderValueChanged()));
	connect(slider5, SIGNAL(valueChanged(float)), this, SLOT(sliderValueChanged()));
	connect(slider6, SIGNAL(valueChanged(float)), this, SLOT(sliderValueChanged()));

	slider1->setValue(2);
	slider2->setValue(5.5);
	slider3->setValue(1);
	slider4->setValue(0.5);
	slider5->setValue(3);
	slider6->setValue(2.5);
}

void dbWidget::sliderValueChanged()
{
	m_data.vec_data[0].x = slider1->value();
	m_data.vec_data[0].y = slider2->value();
	m_data.vec_data[0].z = slider3->value();
	m_data.vec_data[1].x = slider4->value();
	m_data.vec_data[1].y = slider5->value();
	m_data.vec_data[1].z = slider6->value();
}


dbWidget::~dbWidget()
{

}
