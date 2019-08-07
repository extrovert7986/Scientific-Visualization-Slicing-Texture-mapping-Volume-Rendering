#version 120

uniform sampler3D tex3D;
uniform sampler1D tex1D;

struct light_t{
	float kd;
	float ks;
	float ka;
};
struct rendering_t{
	int boundary;
	int facing;
};

uniform light_t myLight;
uniform rendering_t myRender;

varying vec3 L;
varying vec3 E;

void main (void)
{
	
	vec3 normal;
	float tex1Dcoord;

	tex1Dcoord = texture3D(tex3D,gl_TexCoord[0].stp).a;
	
	//if(texture1D(tex1D,tex1Dcoord).a==0)discard;

	float x = texture3D(tex3D,gl_TexCoord[0].stp).r * 255.0;
	float y = texture3D(tex3D,gl_TexCoord[0].stp).g * 255.0;
	float z = texture3D(tex3D,gl_TexCoord[0].stp).b * 255.0;
	
	float magnitude = sqrt( x*x + y*y + z*z );
	normal = vec3(normalize(gl_ModelViewMatrix* normalize(vec4(x,y,z,0.0))));
	float ndotv = abs(dot(normal,normalize(E)));

	vec3 r = reflect(-L,normal);

	float angle1 = max(dot(normal , L),0.0);
	float angle2 = pow( max( dot(r,normal) , 0.0), 512.0);

	vec4 specular = angle2 * vec4(1.0 ,1.0,1.0,1.0);
	
	vec4 diffuse =  angle1 * (texture1D(tex1D,tex1Dcoord));
	
	vec4 ambient = (texture1D(tex1D,tex1Dcoord));
	
	gl_FragColor =((myLight.ks*specular +myLight.kd * diffuse ) + myLight.ka * ambient) ;
	
	gl_FragColor.a =  (texture1D(tex1D,tex1Dcoord)).a;
	
	if(myRender.boundary>0)gl_FragColor.a*=(1-exp(-magnitude));
	if(myRender.facing>0)gl_FragColor.a*=(1-ndotv);
	

}
