#pragma once
#include <QOpenGLWidget>
#include<QMouseEvent>
#include<QKeyEvent>
#include"CounterDataType.h"
#include"RawDataType.h"
#include"TransferFunction.h"

class ControlWidget : public QOpenGLWidget {
	Q_OBJECT

public:
	ControlWidget(QWidget * parent = Q_NULLPTR);
	~ControlWidget();

	void initializeGL();
	void paintGL();
	void resizeGL(int width,int height);

	void mousePressEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);

	void keyPressEvent(QKeyEvent* e);

signals:
	void Send_CurInt_ToUI(int intensity);
	void Send_CurCol_To_CurColorWidget(float tex[4][256]);
	void Send_tex3D(RawInf_t* rawinf, float* tex3D);
public slots:
	void Rcv_Data_From_MainWindow(RawInf_t* rawinf, RawData_t* rawdata, Counter_t* counter);
	void rcv_colorbarStat(char stat);
	void rcv_hisStat(char stat);
private:
	RawInf_t* rawinf;
	RawData_t* rawdata;
	Counter_t* counter;
	
	//bool newData;
	float* histogram;
	/*GLuint vao;
	GLuint vbo;
	GLuint program;*/
	//
	void Populate_Normal_Histogram(bool log);
	void Populate_Equalize_Histogram(bool log);
	//void Populate_Eq_Histogram(bool log);
	//void create_vao();
	//transfer function
	int width;
	int height;
	float* tex3D;
	float curColorBar[4][256];
	TransferFunction_t* tf;
	Point_t* curPoint;
	Point_t* prePoint;
	Point_t* avPoint;
	char curColor;
	char curColorStat;

	void Draw_Channel(Channel_t* channel, float tex[256]);
	void Populate_Tex3D(float* tex3D, RawInf_t* rawinf, RawData_t* rawdata, Counter_t* counter, bool equalized);

};
