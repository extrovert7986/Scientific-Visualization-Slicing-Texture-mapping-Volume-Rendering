#include <stdlib.h>
#include <stdio.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "GLSLSetup.h"

/*******************************
	create shader
*/
GLuint Create_Shader(char* shaderName, GLenum target) {
	//Create shader object
	GLuint shader = glCreateShader(target);
	if (shader == 0) {
		fprintf(stderr, "Error createing shader.\n");
		exit(0);
	}

	//Open shader context
	FILE *fp = fopen(shaderName, "r");
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	rewind(fp);
	GLchar* buf = (GLchar*)calloc(size+1, sizeof(GLchar));
	fread(buf, sizeof(GLchar), size, fp);
	fclose(fp);
	buf[size] = '\0';
	printf("%s\n",buf);
	const GLchar* codeArray[] = { buf };
	
	//Copy the source code into the shader object
	glShaderSource(shader, 1, codeArray, NULL);
	free(buf);
	//Compile the shader
	glCompileShader(shader);

	//Verify the compilation status
	GLint result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		fprintf(stderr, "shader compilation failed!\n");
		GLint logLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char*log = new char[logLen];
			GLsizei written;
			glGetShaderInfoLog(shader, logLen, &written, log);
			fprintf(stderr, "shader log:\n%s", log);
			delete[] log;
		}
	}
	
	return shader;
}

/*******************************
	create program
*/
GLuint Create_Program(GLuint vertShader, GLuint fragShader) {
	//Create teh program object
	GLuint programHandle = glCreateProgram();
	if (programHandle == 0) {
		fprintf(stderr, "Error creating program object.\n");
		exit(0);
	}
	//Attach the shaders to the program object
	glAttachShader(programHandle, vertShader);
	glAttachShader(programHandle, fragShader);
	//Link the program
	glLinkProgram(programHandle);
	//Verify the link status
	GLint status;
	glGetProgramiv(programHandle, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		fprintf(stderr, "Failed to link shader progam!\n");
		GLint logLen;
		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &logLen);
		if (logLen > 0) {
			char*log = new char[logLen];
			GLsizei written;
			glGetProgramInfoLog(programHandle, logLen, &written, log);
			fprintf(stderr, "Program log: \n%s", log);
			delete[] log;
		}
	}
	return programHandle;
}