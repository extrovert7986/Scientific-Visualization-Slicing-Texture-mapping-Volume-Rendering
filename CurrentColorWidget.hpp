#pragma once
#include <QOpenGLWidget>

class CurrentColorWidget : public QOpenGLWidget {
	Q_OBJECT

public:
	CurrentColorWidget(QWidget * parent = Q_NULLPTR);
	~CurrentColorWidget();

	void initializeGL();
	void paintGL();
	void resizeGL(int width, int height);

public slots:
	void Rcv_curColor_From_TransferFunctionWidget(float tex[4][256]);

private:
	GLuint texName;
	
};
