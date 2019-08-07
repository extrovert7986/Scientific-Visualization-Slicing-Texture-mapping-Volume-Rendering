#include <GL\freeglut.h>
#include "CurrentColorWidget.hpp"

CurrentColorWidget::CurrentColorWidget(QWidget * parent) : QOpenGLWidget(parent) {
	
}

CurrentColorWidget::~CurrentColorWidget() {
	
}

void CurrentColorWidget::initializeGL() {
	glGenTextures(1, &texName);
	glBindTexture(GL_TEXTURE_1D, texName);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void CurrentColorWidget::paintGL() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_TEXTURE_1D);
	glBindTexture(GL_TEXTURE_1D, texName);
	glBegin(GL_POLYGON);
	glTexCoord1f(0.0);
	glVertex2f(0.0, 0.0);
	glTexCoord1f(1.0);
	glVertex2f(255.0, 0.0);
	glTexCoord1f(1.0);
	glVertex2f(255.0, 1.0);
	glTexCoord1f(0.0);
	glVertex2f(0.0, 1.0);
	glEnd();
	glDisable(GL_TEXTURE_1D);

	update();
}

void CurrentColorWidget::resizeGL(int width, int height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 255, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
}

void CurrentColorWidget::Rcv_curColor_From_TransferFunctionWidget(float curColor[4][256]) {
	float tex[256 * 4];
	for (int j = 0; j < 256; j++) {
		for (int i = 0; i < 4; i++) {
			tex[j * 4 + i] = curColor[i][j];
		}
	}
	glBindTexture(GL_TEXTURE_1D, texName);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 256, 0, GL_RGBA, GL_FLOAT, tex);
}