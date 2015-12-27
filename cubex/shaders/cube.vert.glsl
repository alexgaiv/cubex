attribute vec3 Vertex;
attribute vec2 TexCoord;
attribute vec3 Normal;
attribute vec3 Tangent;
attribute vec3 Binormal;

varying vec3 fPosition;
varying vec2 fTexCoord;
varying vec3 fNormal;
varying vec3 fTangent;
varying vec3 fBinormal;

uniform mat4 ModelView;
uniform mat4 NormalMatrix;
uniform mat4 ModelViewProjection;

void main()
{
	fPosition = (ModelView * vec4(Vertex, 1.0)).xyz;
	fTexCoord = TexCoord;

	fNormal   = normalize((NormalMatrix * vec4(Normal, 1.0)).xyz);
	fTangent  = normalize((NormalMatrix * vec4(Tangent, 1.0)).xyz);
	fBinormal = normalize((NormalMatrix * vec4(Binormal, 1.0)).xyz);

	gl_Position = ModelViewProjection * vec4(Vertex, 1.0);
}