#ifndef VOLUMERENDERING_H
#define VOLUMERENDERING_H

#include <QtWidgets/QMainWindow>
#include "ui_volumerendering.h"
#include"FileReader.h"

class VolumeRendering : public QMainWindow
{
	Q_OBJECT

public:
	VolumeRendering(QWidget *parent = 0);
	~VolumeRendering();

signals:
	void Send_Data_To_ControlWidget(RawInf_t* rawinf, RawData_t* rawdata, Counter_t* counter);
	void Send_ColorBarStat(char stat);
	void Send_hisStat(char stat);

public slots:
	void rcv_cur_int(int intensity);

private slots:
    void on_actionOpen_triggered();
    void on_checkRed_clicked();
    void on_checkGreen_clicked();
    void on_checkBlue_clicked();
    void on_checkAlpha_clicked();

    void on_checkLog_clicked();

    void on_checkEqualize_clicked();

private:
	Ui::VolumeRenderingClass ui;
	RawInf_t* rawinf;
	RawData_t* rawdata;
	Counter_t* counter;

	char hisStat;
	char curStat;
	void Clear_Data();
	void Resetting_UI();
};

#endif // VOLUMERENDERING_H
