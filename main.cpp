#include "volumerendering.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	VolumeRendering w;
	w.show();
	return a.exec();
}
