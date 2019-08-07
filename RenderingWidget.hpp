#pragma once
#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QKeyEvent>
#include "RawDataType.h"

class RenderingWidget : public QOpenGLWidget {
	Q_OBJECT

public:
	RenderingWidget(QWidget * parent = Q_NULLPTR);
	~RenderingWidget();

	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

	void mousePressEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void mouseReleaseEvent(QMouseEvent* e);

	void keyPressEvent(QKeyEvent* e);

public slots:
	void Rcv_tex3D(RawInf_t* rawinf,float* tex3D);
	void Rcv_TransferFunction(float tex[4][256]);
	
private:
	//shader object
	GLuint shadingVert;
	GLuint shadingFrag;
	//program handler
	GLuint shadingProgram;
	GLuint curProgram;

	//control unit
	//back ground color
	float bkgndColor;
	float farClip, nearClip;
	float viewBound;
	float theta, phi,r;
	float eyePos[3];
	float focus[3];
	float mtx[16];

	//lihgt parameter
	float kd;
	float ks;
	float ka;

	//boundary enhancement and facing subtract
	int boundary;
	int facing;
	
	//
	bool effect;
	//data input
	RawInf_t* rawinf;
	
	bool tex1DChanged,tex1DExsist, tex3DChanged;

	float* tex1D;
	float* tex3D;

	GLuint texName[2];

};
