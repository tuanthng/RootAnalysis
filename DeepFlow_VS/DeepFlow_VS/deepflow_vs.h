#ifndef DEEPFLOW_VS_H
#define DEEPFLOW_VS_H

#include <QtWidgets/QMainWindow>
#include "ui_deepflow_vs.h"

class DeepFlow_VS : public QMainWindow
{
	Q_OBJECT

public:
	DeepFlow_VS(QWidget *parent = 0);
	~DeepFlow_VS();

private:
	Ui::DeepFlow_VSClass ui;
};

#endif // DEEPFLOW_VS_H
