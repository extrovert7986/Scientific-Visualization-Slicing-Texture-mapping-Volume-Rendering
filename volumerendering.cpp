#include "volumerendering.h"
#include<QFileDialog>
#include<QMessageBox>
#include<stdio.h>

VolumeRendering::VolumeRendering(QWidget *parent)
	: QMainWindow(parent), rawinf(nullptr), rawdata(nullptr), counter(nullptr), curStat(15), hisStat(0)
{
	ui.setupUi(this);
	QObject::connect(this, &VolumeRendering::Send_Data_To_ControlWidget, ui.controlWidget, &ControlWidget::Rcv_Data_From_MainWindow);
	QObject::connect(ui.controlWidget, &ControlWidget::Send_CurInt_ToUI, this, &VolumeRendering::rcv_cur_int);
	QObject::connect(ui.controlWidget, &ControlWidget::Send_CurCol_To_CurColorWidget, ui.transferFunctionWidget, &CurrentColorWidget::Rcv_curColor_From_TransferFunctionWidget);
	QObject::connect(ui.controlWidget, &ControlWidget::Send_CurCol_To_CurColorWidget, ui.renderingWidget, &RenderingWidget::Rcv_TransferFunction);
	QObject::connect(ui.controlWidget, &ControlWidget::Send_tex3D, ui.renderingWidget, &RenderingWidget::Rcv_tex3D);
	QObject::connect(this, &VolumeRendering::Send_ColorBarStat, ui.controlWidget, &ControlWidget::rcv_colorbarStat);
	QObject::connect(this, &VolumeRendering::Send_hisStat, ui.controlWidget, &ControlWidget::rcv_hisStat);
	ui.checkRed->setChecked(true);
	ui.checkGreen->setChecked(true);
	ui.checkBlue->setChecked(true);
	ui.checkAlpha->setChecked(true);
}

VolumeRendering::~VolumeRendering()
{

}

void VolumeRendering::rcv_cur_int(int intensity) {
	ui.current->setText("Current Intensity: "+QString::number(intensity));
}

void VolumeRendering::on_actionOpen_triggered()
{
	QString filePath = QFileDialog::getOpenFileName(this, tr("Open Information File"), "\RawInformation", tr("Inf file(*.inf)"));
	//check if open data
	if (filePath.length() != 0) {
		std::string str = filePath.toStdString();
		const char* file = str.data();
		const char* fileExtension = strrchr(file, '.');
		//check file extension is correct or not
		if (!strcmp(fileExtension, ".inf")) {
			//first time open data or not
			if (rawinf || rawdata || counter) {
				Clear_Data();
			}
			rawinf = (RawInf_t*)calloc(1, sizeof(RawInf_t));
			rawdata = (RawData_t*)calloc(1, sizeof(RawData_t));
			counter = (Counter_t*)calloc(1, sizeof(Counter_t));
			Read_RawInf(file, rawinf);
			Read_Raw_Data(rawdata, rawinf, counter);
			//initial and reset the ui
			Resetting_UI();
			if (counter->range <= 0) {
				QMessageBox::critical(this, tr("Error"), tr("data range error"));
				return;
			}
			emit(Send_Data_To_ControlWidget(rawinf, rawdata, counter));
		}
	}
}

void VolumeRendering::Clear_Data() {
	if (rawinf) {
		free(rawinf);
		rawinf = nullptr;
	}
	if (rawdata) {
		free(rawdata->intensity);
		free(rawdata->gradient);
		free(rawdata);
		rawdata = nullptr;
	}
	if (counter) {
		free(counter->count);
		free(counter);
		counter = nullptr;
	}

}

void VolumeRendering::Resetting_UI() {
	ui.checkLog->setChecked(false);
	ui.checkEqualize->setChecked(false);
	ui.checkRed->setChecked(true);
	ui.checkGreen->setChecked(true);
	ui.checkBlue->setChecked(true);
	ui.checkAlpha->setChecked(true);
	ui.min->setText(QString::number(rawdata->minInt));
	ui.max->setText(QString::number(rawdata->maxInt));
	curStat = 15;
	emit(Send_ColorBarStat(15));
	hisStat = 0;
	emit(Send_hisStat(hisStat));
}
void VolumeRendering::on_checkRed_clicked()
{
	if (ui.checkRed->isChecked()) {
		curStat |= 1;
	}
	else curStat &= 14;
	emit(Send_ColorBarStat(curStat));
}

void VolumeRendering::on_checkGreen_clicked()
{
	if (ui.checkGreen->isChecked()) {
		curStat |= 2;
	}
	else curStat &= 13;
	emit(Send_ColorBarStat(curStat));
}

void VolumeRendering::on_checkBlue_clicked()
{
	if (ui.checkBlue->isChecked()) {
		curStat |= 4;
	}
	else curStat &= 11;
	emit(Send_ColorBarStat(curStat));
}

void VolumeRendering::on_checkAlpha_clicked()
{
	if (ui.checkAlpha->isChecked()) {
		curStat |= 8;
	}
	else curStat &= 7;
	emit(Send_ColorBarStat(curStat));
}

void VolumeRendering::on_checkLog_clicked()
{
	if (ui.checkLog->isChecked()) {
		hisStat |= 1;
	}
	else hisStat &= 2;
	emit(Send_hisStat(hisStat));
}

void VolumeRendering::on_checkEqualize_clicked()
{
	if (ui.checkEqualize->isChecked()) {
		hisStat |= 2;
	}
	else hisStat &= 1;
	emit(Send_hisStat(hisStat));
}
