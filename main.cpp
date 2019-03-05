#include <QtWidgets/QApplication>
#include "dbWidget.h"
#include "dbGLWindiw.h"

int main(int arg,char* argv[])
{
	QApplication app(arg, argv);
	dbWidget widg;
	widg.show();
	return app.exec();
	//return 0;
}