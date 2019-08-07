#include<GL\glew.h>
#include<GL\freeglut.h>
#include "RenderingWidget.hpp"
#include "GLSLSetup.h"
#include "Slicing.h"
#define _USE_MATH_DEFINES
#include "math.h"

RenderingWidget::RenderingWidget(QWidget * parent) : QOpenGLWidget(parent) {
	setFocusPolicy(Qt::StrongFocus);
	
	bkgndColor = 0.3;
	farClip = 600.0;
	nearClip = 200.0;
	viewBound = 50;
	focus[0] = focus[1] = focus[2] = 0.0;
	r = 300;
	theta = M_PI / 2.0;
	phi = M_PI / 2.0;
	eyePos[0] = r*sin(theta)*cos(phi)+focus[0];
	eyePos[1] = r*sin(theta)*sin(phi)+focus[1];
	eyePos[2] = r*cos(theta)+focus[2];
	
	kd = 0.8;
	ks = 0.7;
	ka = 0.7;

	boundary = 0;
	facing = 0;

	effect = false;

	tex1DChanged = true;
	tex3DChanged = false;
	tex1DExsist = false;

	this->tex1D = (float*)calloc(256 * 4, sizeof(float));
	this->rawinf = nullptr;
	this->tex3D = nullptr;
	
}

RenderingWidget::~RenderingWidget() {
	
}

void RenderingWidget::initializeGL(){
	glewInit();
	printf("vertex shader:\n");
	shadingVert = Create_Shader("PhongShadingVertexShader.glsl", GL_VERTEX_SHADER);
	printf("Fragment shader:\n");
	shadingFrag = Create_Shader("PhongShadingFragmentShader.glsl", GL_FRAGMENT_SHADER);
	//
	shadingProgram = Create_Program(shadingVert, shadingFrag);
	glUseProgram(shadingProgram);

	//prepare texture
	glGenTextures(2, texName);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, texName[0]);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glUniform1i(glGetUniformLocation(shadingProgram, "tex3D"), 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, texName[1]);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glUniform1i(glGetUniformLocation(shadingProgram, "tex1D"), 1);
	
	//light parameter
	glProgramUniform1f(shadingProgram, glGetUniformLocation(shadingProgram, "myLight.kd"), kd);
	glProgramUniform1f(shadingProgram, glGetUniformLocation(shadingProgram, "myLight.ks"), ks);
	glProgramUniform1f(shadingProgram, glGetUniformLocation(shadingProgram, "myLight.ka"), ka);

	glProgramUniform1i(shadingProgram, glGetUniformLocation(shadingProgram, "myRender.boundary"), boundary);
	glProgramUniform1i(shadingProgram, glGetUniformLocation(shadingProgram, "myRender.facing"), facing);
	
}

void RenderingWidget::paintGL() {
	glClearColor(bkgndColor, bkgndColor, bkgndColor, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	if (!rawinf)return;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-viewBound, viewBound, -viewBound, viewBound, nearClip, farClip);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eyePos[0], eyePos[1], eyePos[2], focus[0], focus[1], focus[2], 0, 0, -1);
	glGetFloatv(GL_MODELVIEW_MATRIX, this->mtx);
	
	if (tex3DChanged) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, this->texName[0]);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, rawinf->res.x, rawinf->res.y, rawinf->res.z,0, GL_RGBA, GL_FLOAT, this->tex3D);
		tex3DChanged = false;
	}
	if (tex1DChanged) {
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_1D, this->texName[1]);
		glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 256, 0, GL_RGBA, GL_FLOAT, this->tex1D);
		tex1DChanged = false;
	}

	ViewAlignedSlicing(eyePos, mtx, 1200, this->rawinf);

	update();
}

void RenderingWidget::resizeGL(int width, int height) {
	glViewport(0, 0, width, height);
}

void RenderingWidget::mousePressEvent(QMouseEvent* e) {

}

void RenderingWidget::mouseMoveEvent(QMouseEvent* e) {

}

void RenderingWidget::mouseReleaseEvent(QMouseEvent* e) {

}

void RenderingWidget::keyPressEvent(QKeyEvent* e) {
	if (e->key() == '+') {
		bkgndColor += 0.1;
		bkgndColor = (bkgndColor > 1.0) ? 1.0 : bkgndColor;
	}
	else if (e->key() == '-') {
		bkgndColor -= 0.1;
		bkgndColor = (bkgndColor < 0.0) ? 0.0 : bkgndColor;
	}
	else if (e->key() == 'z' || e->key() == 'Z') {
		viewBound *= 0.9;
	}
	else if (e->key() == 'x' || e->key() == 'X') {
		viewBound *= (10 / 9.0);
	}
	else if (e->key() == 'w' || e->key() == 'W') {
		theta = ((theta - 0.01) <= 0) ? theta : theta - 0.01;
		
		eyePos[0] = r*sin(theta)*cos(phi) + focus[0];
		eyePos[1] = r*sin(theta)*sin(phi) + focus[1];
		eyePos[2] = r*cos(theta) + focus[2];
	}
	else if (e->key() == 's' || e->key() == 'S') {
		theta = ((theta + 0.01) >= M_PI) ? theta : theta + 0.01;
		eyePos[0] = r*sin(theta)*cos(phi) + focus[0];
		eyePos[1] = r*sin(theta)*sin(phi) + focus[1];
		eyePos[2] = r*cos(theta) + focus[2];
	}
	else if (e->key() == 'a' || e->key() == 'A') {
		phi = ((phi += 0.01) >= M_PI) ? phi -= (M_PI * 2) : phi;
		eyePos[0] = r*sin(theta)*cos(phi) + focus[0];
		eyePos[1] = r*sin(theta)*sin(phi) + focus[1];
	}
	else if (e->key() == 'd' || e->key() == 'D') {
		phi = ((phi -= 0.01) <= -M_PI) ? phi += (M_PI * 2) : phi;
		eyePos[0] = r*sin(theta)*cos(phi) + focus[0];
		eyePos[1] = r*sin(theta)*sin(phi) + focus[1];
	}
	else if (e->key()=='f'|| e->key() == 'F') {
		nearClip += 5;
	}
	
	else if (e->key() == 'n'|| e->key() == 'N') {
		r += 20;
	}
	else if (e->key() == 'j' || e->key() == 'J') {
		kd -= 0.05;
		kd = ((kd <= 0) ? 0 : kd);
		glProgramUniform1f(shadingProgram, glGetUniformLocation(shadingProgram, "myLight.kd"), kd);

	}
	else if (e->key() == 'k' || e->key() == 'K') {
		ks -= 0.05;
		ks = ((ks <= 0) ? 0 : ks);
		glProgramUniform1f(shadingProgram, glGetUniformLocation(shadingProgram, "myLight.ks"), ks);
		
	}
	else if (e->key() == 'l' || e->key() == 'L') {
		ka -= 0.05;
		ka = ((ka <= 0) ? 0 : ka);
		glProgramUniform1f(shadingProgram, glGetUniformLocation(shadingProgram, "myLight.ka"), ka);

	}
	else if (e->key() == 'u' || e->key() == 'U') {
		kd += 0.05;
		kd = ((kd >= 1) ? 1 : kd);
		glProgramUniform1f(shadingProgram, glGetUniformLocation(shadingProgram, "myLight.kd"), kd);

	}
	else if (e->key() == 'i' || e->key() == 'I') {
		ks += 0.05;
		ks = ((ks >= 1) ? 1 : ks);
		glProgramUniform1f(shadingProgram, glGetUniformLocation(shadingProgram, "myLight.ks"), ks);
	}
	else if (e->key() == 'o' || e->key() == 'O') {
		ka += 0.05;
		ka = ((ka >= 1) ? 1 : ka);
		glProgramUniform1f(shadingProgram, glGetUniformLocation(shadingProgram, "myLight.ka"), ka);

	}
	else if (e->key() == '1') {
		boundary = 0;
		glProgramUniform1i(shadingProgram, glGetUniformLocation(shadingProgram, "myRender.boundary"), boundary);

	}
	else if (e->key() == '2') {
		boundary = 1; 
		glProgramUniform1i(shadingProgram, glGetUniformLocation(shadingProgram, "myRender.boundary"), boundary);

	}
	else if (e->key() == '4') {
		facing = 0;
		glProgramUniform1i(shadingProgram, glGetUniformLocation(shadingProgram, "myRender.facing"), facing);

	}
	else if (e->key() == '5') {
		facing = 1;
		glProgramUniform1i(shadingProgram, glGetUniformLocation(shadingProgram, "myRender.facing"), facing);

	}
	else if (e->key() == 'e' || e->key() == 'E') {
		glBindTexture(GL_TEXTURE_3D, texName[0]);
		if (!effect) {
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}
		else {
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		}
		effect = !effect;

	}
	update();
}

void RenderingWidget::Rcv_tex3D(RawInf_t* rawinf, float* tex3D) {
	
	this->rawinf = rawinf;
	this->tex3D = tex3D;
	tex3DChanged = true;
}

void RenderingWidget::Rcv_TransferFunction(float tex[4][256]) {
	for (int j = 0; j < 256; j++) {
		for (int i = 0; i < 4; i++) {
			this->tex1D[j * 4 + i] = tex[i][j];
		}
	}
	tex1DExsist = true;
	tex1DChanged = true;
}


