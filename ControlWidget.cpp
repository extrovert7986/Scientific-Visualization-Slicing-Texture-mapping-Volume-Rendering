#include<GL\glew.h>
#include<GL\freeglut.h>
#include"ControlWidget.hpp"
#include"LinearInterpolation.h"
#include<stdlib.h>
#include<math.h>
#include<glm\gtc\matrix_transform.hpp>
#include<glm\gtc\type_ptr.hpp>

ControlWidget::ControlWidget(QWidget * parent)
	:QOpenGLWidget(parent), rawinf(nullptr), rawdata(nullptr), counter(nullptr), histogram(nullptr)//, vao(0), vbo(0), newData(false)
{
	Initial_TransferFunction(&tf);
	Interpolate_Channel_By_LinearInterpolation(tf->r, curColorBar[CHANNEL_R]);
	Interpolate_Channel_By_LinearInterpolation(tf->g, curColorBar[CHANNEL_G]);
	Interpolate_Channel_By_LinearInterpolation(tf->b, curColorBar[CHANNEL_B]);
	Interpolate_Channel_By_LinearInterpolation(tf->a, curColorBar[CHANNEL_A]);
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
	curPoint = nullptr;
	prePoint = nullptr;
	avPoint = nullptr;
	tex3D = nullptr;
	curColorStat = 15;
}

ControlWidget::~ControlWidget() {
	
}

void ControlWidget::initializeGL() {
	/*if (glewInit() != GLEW_OK) {
		printf("Glew initial failed.\n");
		system("pause");
		exit(0);
	}
	GLuint shader[5];
	shader[0] = set_shader(GL_VERTEX_SHADER, "histogram_vert.glsl");
	shader[1] = set_shader(GL_FRAGMENT_SHADER, "histogram_frag.glsl");
	program = set_program(shader);
	glUseProgram(program);*/
}

void ControlWidget::paintGL() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
/*
	if (newData) {
		create_vao();
		newData = false;
	}
	if (counter) {
		glm::mat4 mat = glm::ortho(-50, 50, 0, 100);
		glUniformMatrix4fv(glGetUniformLocation(program, "mat"), 1, GL_FALSE, glm::value_ptr(mat));
		glUseProgram(program);
		glBindVertexArray(vao);
		glDrawArrays(GL_LINES, 0, 2 * counter->range);
	}*/
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 255, 0, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (counter) {
		glLineWidth(1.0);
		for (int i = 0; i < counter->range; i++) {
			glColor3f(0.3, 0.3, 0.3);
			glBegin(GL_LINES);
			glVertex2f(histogram[i * 4], histogram[i * 4 + 1]);
			glVertex2f(histogram[i * 4 + 2], histogram[i * 4 + 3]);
			glEnd();
		}
	}
	glEnable(GL_POINT_SMOOTH);
	if (curColorStat & 1) {
		glColor3f(1.0, 0.0, 0.0);
		Draw_Channel(tf->r, curColorBar[CHANNEL_R]);
	}
	if (curColorStat & 2) {
		glColor3f(0.0, 1.0, 0.0);
		Draw_Channel(tf->g, curColorBar[CHANNEL_G]);
	}
	if (curColorStat & 4) {
		glColor3f(0.0, 0.0, 1.0);
		Draw_Channel(tf->b, curColorBar[CHANNEL_B]);
	}
	if (curColorStat & 8) {
		glColor3f(1.0, 1.0, 1.0);
		Draw_Channel(tf->a, curColorBar[CHANNEL_A]);
	}
	glDisable(GL_POINT_SMOOTH);
	update();
}

void ControlWidget::resizeGL(int width, int height) {
	glViewport(0, 0, width, height);
	this->width = width;
	this->height = height;
}

void ControlWidget::mousePressEvent(QMouseEvent* e) {
	QImage img = ControlWidget::grabFramebuffer();
	QColor picked = img.pixelColor(e->pos());
	float xpos = e->x()*(255.0 / width);
	float ypos = 1.0 - e->y()*(1.0 / height);
	float bound = 10 * (255.0 / width);
	curPoint = nullptr;
	curColor = BLACK;
	//selected current clor and current point
	if (picked == QColor(255, 0, 0, 255)) {
		printf("red ");
		curColor = CHANNEL_R;
		curPoint = prePoint = Select_SamplePoint_from_TransferFunction(xpos, ypos, bound, tf->r);
		if (!curPoint) curPoint = prePoint = Insert_SamplePoint_Into_Channel(xpos, ypos, &avPoint, tf->r);
		for (int i = 0; i < tf->r->N - 1; i++)prePoint = prePoint->next;

	}
	else if (picked == QColor(0, 255, 0, 255)) {
		printf("green ");
		curColor = CHANNEL_G;
		curPoint = prePoint = Select_SamplePoint_from_TransferFunction(xpos, ypos, bound, tf->g);
		if (!curPoint) curPoint = prePoint = Insert_SamplePoint_Into_Channel(xpos, ypos, &avPoint, tf->g);
		for (int i = 0; i < tf->g->N - 1; i++)prePoint = prePoint->next;
	}
	else if (picked == QColor(0, 0, 255, 255)) {
		printf("blue ");
		curColor = CHANNEL_B;
		curPoint = prePoint = Select_SamplePoint_from_TransferFunction(xpos, ypos, bound, tf->b);
		if (!curPoint) curPoint = prePoint = Insert_SamplePoint_Into_Channel(xpos, ypos, &avPoint, tf->b);
		for (int i = 0; i < tf->b->N - 1; i++)prePoint = prePoint->next;
	}
	else if (picked == QColor(255, 255, 255, 255)) {
		printf("alpha ");
		curColor = CHANNEL_A;
		curPoint = prePoint = Select_SamplePoint_from_TransferFunction(xpos, ypos, bound, tf->a);
		if (!curPoint)  curPoint = prePoint = Insert_SamplePoint_Into_Channel(xpos, ypos, &avPoint, tf->a);
		for (int i = 0; i < tf->a->N - 1; i++)prePoint = prePoint->next;
	}
	else curColor = BLACK;
}

void ControlWidget::mouseMoveEvent(QMouseEvent* e) {
	//move current point
	if (!rawdata)return;
	float xpos = e->x()* 255.0 / width;
	if (e->buttons() == Qt::LeftButton) {
		if (curColor != BLACK) {
			if ((int)curPoint->x == 0) {
				xpos = 0;
			}
			else if ((int)curPoint->x == 255) {
				xpos = 255;
			}
			else {
				xpos = (xpos <= prePoint->x + 1) ? prePoint->x + 1 : xpos;
				xpos = (xpos >= curPoint->next->x - 1) ? curPoint->next->x - 1 : xpos;
			}
			float ypos = 1.0 - e->y()* 1.0 / height;
			ypos = (ypos <= 0) ? 0 : ypos;
			ypos = (ypos >= 1) ? 1 : ypos;

			Modify_SamplePoint(curPoint, xpos, ypos);
			Interpolate_Range_From_Start2End(prePoint, curPoint, curColorBar[curColor]);
			Interpolate_Range_From_Start2End(curPoint, curPoint->next, curColorBar[curColor]);
		}
	}
	int index;
	if (xpos < histogram[0])
		index = xpos < (histogram[0] / 2.0) ? rawdata->minInt - 1 : rawdata->minInt;
	else if (xpos > histogram[(counter->range - 1)*4])
		index = xpos > ((histogram[(counter->range - 1) * 4] + 255) / 2.0) ? rawdata->maxInt + 1 : rawdata->maxInt;
	else {
		for (int i = 0; i < counter->range - 1; i++) {
			if (xpos >= histogram[i * 4] && xpos <= histogram[(i + 1) * 4]) {
				index = xpos < (histogram[i * 4] + histogram[(i + 1) * 4]) / 2.0 ? i : i + 1;
				index = index + rawdata->minInt;
				break;
			}
		}
	}
	emit(Send_CurInt_ToUI(index));
	
}

void ControlWidget::mouseReleaseEvent(QMouseEvent* e) {
	
	emit(Send_CurCol_To_CurColorWidget(curColorBar));
	/*emit(Send_Color_To_RenderingWidget(curColorBar));*/
}

void ControlWidget::keyPressEvent(QKeyEvent* e) {
	if (!curPoint)return;
	if (e->key() == 'd' || e->key() == 'D') {
		switch (curColor) {
		case CHANNEL_R:
			Delete_SamplePoint_from_Channel(&curPoint, &avPoint, tf->r);
			break;
		case CHANNEL_G:
			Delete_SamplePoint_from_Channel(&curPoint, &avPoint, tf->g);
			break;
		case CHANNEL_B:
			Delete_SamplePoint_from_Channel(&curPoint, &avPoint, tf->b);
			break;
		case CHANNEL_A:
			Delete_SamplePoint_from_Channel(&curPoint, &avPoint, tf->a);
			break;
		}
	}
}

void ControlWidget::Rcv_Data_From_MainWindow(RawInf_t* rawinf, RawData_t* rawdata, Counter_t* counter) {
	this->rawinf = rawinf;
	this->rawdata = rawdata;
	this->counter = counter;
	
	if (histogram)
		free(histogram);
	//                       num of intensity * 2points * 2 coords
	histogram = (float*)calloc(counter->range * 2 * 2, sizeof(float));
	Populate_Normal_Histogram(false);
	
	if (tex3D)free(tex3D);
	tex3D = (float*)calloc(rawinf->size * 4, sizeof(float));
	Populate_Tex3D(tex3D, rawinf, rawdata, counter, false);


	emit(Send_tex3D(rawinf, tex3D));
}

void ControlWidget::rcv_colorbarStat(char stat) {
	curColorStat = stat;
}

void ControlWidget::rcv_hisStat(char stat) {
	if (!rawdata)return;
	if (stat & 2) { // equalize
		if (stat & 1) // log
			Populate_Equalize_Histogram(true);
		else Populate_Equalize_Histogram(false);
		Populate_Tex3D(tex3D, rawinf, rawdata, counter, true);
		emit(Send_tex3D(rawinf, tex3D));
	}
	else {
		if (stat & 1) Populate_Normal_Histogram(true);
		else Populate_Normal_Histogram(false);
	}
	
}

void ControlWidget::Populate_Normal_Histogram(bool log) {
	for (int curIntensity = 0; curIntensity < counter->range;curIntensity++)
		for (int points = 0; points < 2; points++)
			for (int coord = 0; coord < 2; coord++) {
				int index = coord + points * 2 + curIntensity * 4;
				//x coordinate : follows the current intensity
				if (coord == 0) {
					histogram[index] = (curIntensity + 1)*255 / float(counter->range + 1);
				}
				//y coordinate :
				else {
					//first point of lines
					if (points == 0) {
						histogram[index] = 0.0;
					}
					//height of histogram
					else {
						int y = ((counter->count[curIntensity] == 0) ? -1.0 : counter->count[curIntensity]);
						if (!log) {
							histogram[index] = ((y == -1) ? -1 : (y / (float)counter->max));
						}
						else {
							histogram[index] = ((y == -1) ? -1 : (log10(y) / log10(counter->max)));
						}
					}
				}
			}

}

void ControlWidget::Populate_Equalize_Histogram(bool log) {
	int curX = 0;
	for (int curIntensity = 0; curIntensity < counter->range; curIntensity++) {
		for (int points = 0; points < 2; points++) {
			for (int coord = 0; coord < 2; coord++) {
				int index = coord + points * 2 + curIntensity * 4;
				//x coordinate : follows the current intensity
				if (coord == 0) {
					histogram[index] = curX * 255 / (float)rawinf->size;
				}
				//y coordinate :
				else {
					//first point of lines
					if (points == 0) {
						histogram[index] = 0.0;
					}
					//height of histogram
					else {
						int y = ((counter->count[curIntensity] == 0) ? -1.0 : counter->count[curIntensity]);
						if (!log) {
							histogram[index] = ((y == -1) ? -1 : (y / (float)counter->max));
						}
						else {
							histogram[index] = ((y == -1) ? -1 : (log10(y) / log10(counter->max)));
						}
					}
				}
			}
		}
		curX += counter->count[curIntensity];
	}
}

//void ControlWidget::create_vao() {
//	//this method may have some bugs
//	glGenBuffers(1, &vbo);
//	glBindBuffer(GL_ARRAY_BUFFER, vbo);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*counter->range * 2 * 2, histogram, GL_STATIC_DRAW);
//	glGenVertexArrays(1, &vao);
//	glBindVertexArray(vao);
//	glEnableVertexAttribArray(0);
//	glBindVertexBuffer(0, vbo, 0, sizeof(float) * 2);
//	glVertexAttribFormat(0, 2, GL_FLOAT, GL_FALSE, 0);
//	glVertexAttribBinding(0, 0);
//}

void ControlWidget::Draw_Channel(Channel_t* channel, float tex[256]) {
	Point_t* temp;
	temp = channel->first;
	//sample points
	glPointSize(10);
	glBegin(GL_POINTS);
	for (int i = 0; i < channel->N; i++) {
		glVertex2f(temp->x, temp->y);
		temp = temp->next;
	}
	glEnd();
	//transferfunction texture
	glLineWidth(6);
	glBegin(GL_LINES);
	for (int i = 0; i < 255; i++) {
		glVertex2f(i, tex[i]);
		glVertex2f(i + 1, tex[i + 1]);
	}
	glEnd();
}

void ControlWidget::Populate_Tex3D(float* tex3D, RawInf_t* rawinf, RawData_t* rawdata, Counter_t* counter, bool equalized) {
	float maxGra;
	switch (rawinf->sampleType) {
	case UNSIGNED_CHAR:
		maxGra = 255;
		break;
	case UNSIGNED_SHORT:
		maxGra = 65535;
		break;
	}
	int sum = 0;
	int* accCounter = (int*)calloc(counter->range, sizeof(int));
	if (equalized) {
		for (int i = 0; i < counter->range; i++) {
			accCounter[i] = sum;
			sum += counter->count[i];
		}
	}
	for (int k = 0; k < rawinf->res.z; k++) {
		for (int j = 0; j < rawinf->res.y; j++) {
			for (int i = 0; i < rawinf->res.x; i++) {
				int index = i + j*rawinf->res.x + k*rawinf->res.x*rawinf->res.y;
				tex3D[index * 4] = rawdata->gradient[index].x / maxGra;
				tex3D[index * 4 + 1] = rawdata->gradient[index].y / maxGra;
				tex3D[index * 4 + 2] = rawdata->gradient[index].z / maxGra;
				if (equalized) {
					float xpos = (accCounter[int(rawdata->intensity[index])] * 255.0) / sum;
					tex3D[index * 4 + 3] = (xpos + 1.0) / 257.0;
				}
				else {
					float xpos = (rawdata->intensity[index] - rawdata->minInt)*255.0 / (rawdata->maxInt - rawdata->minInt);
					tex3D[index * 4 + 3] = (xpos + 1.0) / 257.0;
				}

			}
		}
	}
}