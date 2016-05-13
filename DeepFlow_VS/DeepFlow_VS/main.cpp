#include "deepflow_vs.h"
#include <QtWidgets/QApplication>

int main1(int argc, char *argv[])
{
	QApplication a(argc, argv);
	DeepFlow_VS w;
	w.show();
	return a.exec();
}
