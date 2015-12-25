varying vec3 fPosition;
varying vec3 fNormal;
varying vec2 fTexCoord;
varying vec3 fTangent;
varying vec3 fBinormal;

uniform bool UseNormalMap;
uniform bool NoLighting;
uniform bool NoSpecular;
uniform int lightMode;
uniform bool UseTexture;
uniform sampler2D ColorMap;
uniform sampler2D NormalMap;
uniform vec3 Color;

uniform struct {
	vec4 diffuse;
	vec4 specular;
	vec4 ambient;
	int shininess;
} FrontMaterial;

vec3 fragNormal;
mat3 tbn;

struct Light
{
	vec4 position;
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;
};

Light lightSources[4] = {
	Light(
		vec4(1.0, 1.0, 1.0, 0.0),
		vec4(0.4),
		vec4(1),
		vec4(0.8, 0.8, 0.8, 1.0)),
	Light(
		vec4(-1.0, 1.0, -0.5, 0.0),
		vec4(0), vec4(0),
		vec4(1)),
	Light(
		vec4(0.0, 1.0, 1.0, 0.0),
		vec4(0.8, 0.8, 0.8, 1.0),
		vec4(1),
		vec4(0)),
	Light(
		vec4(-4.0, 4.0, 0.0, 0.0),
		vec4(0),
		vec4(0),
		vec4(0.5,1,0.5,1))
};

vec4 GetDiffuse(vec3 lightDir, vec4 lightDiffuse) {
	float diffuseCoeff = max(0.0, dot(fragNormal, lightDir));
	return diffuseCoeff * lightDiffuse;
}

vec4 GetSpecular(vec3 lightDir, vec4 lightSpecular)
{
	float specAngle = 0.0;
	vec3 viewDir = normalize(-fPosition);
	if (false) {
		vec3 ref = normalize(reflect(-lightDir, fragNormal));
		specAngle = max(0.0, dot(ref, viewDir));
	}
	else {
		vec3 halfDir = normalize(lightDir + viewDir);
		specAngle = max(0.0, dot(fragNormal, halfDir));
	}
	float specCoeff = pow(specAngle, 0.3*FrontMaterial.shininess);
	return specCoeff * lightSpecular;
}

vec4 PhongLight(Light l)
{
	vec3 lightDir = normalize(vec3(l.position));
	vec3 color;
	if (UseTexture) {
		vec3 texel = texture(ColorMap, fTexCoord);
		if (texel == vec3(0)) color = vec3(0.08);
		else color = Color;
	}
	else color = Color;
	return (FrontMaterial.ambient + GetDiffuse(lightDir, l.diffuse)) * vec4(color, 1)
		+ (NoSpecular ? vec4(0) : GetSpecular(lightDir, l.specular) * FrontMaterial.specular);
		//+ GetSpecular(lightDir, l.specular);
}

void main()
{
	if (NoLighting) {
		gl_FragColor = vec4(Color, 1);
	}
	else {
		if (UseNormalMap) {
			vec3 t = texture(NormalMap, fTexCoord);
			fragNormal = t * 2 - vec3(1);
			tbn = mat3(fTangent, fBinormal, fNormal);
			fragNormal = normalize(tbn * fragNormal);
		}
		else {
			fragNormal = normalize(fNormal);
		}

		gl_FragColor = vec4(0);
		if (lightMode == 0) {
			for (int i = 0; i < 1; i++)
				gl_FragColor += PhongLight(lightSources[i]);
		}
		else {
			for (int i = 0; i < 1; i++)
				gl_FragColor += PhongLight(lightSources[i]);
		}
	}
}