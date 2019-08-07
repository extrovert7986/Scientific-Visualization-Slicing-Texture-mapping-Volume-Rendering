#version 120

varying vec3 L;
varying vec3 E;

void main(void){
	gl_TexCoord[0] = gl_MultiTexCoord0;
	vec4 vertexPos = gl_ModelViewMatrix * gl_Vertex;

	E = normalize(vec3( -vertexPos ));
	L = normalize(vec3 (0,0,-1));
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}