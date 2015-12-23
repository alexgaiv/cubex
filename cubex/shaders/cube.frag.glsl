varying vec3 fPosition;
/*varying vec3 fNormal;
varying vec2 fTexCoord;
varying vec3 fTangent;
varying vec3 fBinormal;*/

uniform sampler2D ColorMap;
uniform sampler2D NormalMap;
uniform sampler2D SpecularMap;
uniform vec3 Color;

int main()
{
	gl_FragColor = Color;
}