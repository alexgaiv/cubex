varying vec3 fPosition;
varying vec2 fTexCoord;
varying vec3 fNormal;
varying vec3 fTangent;
varying vec3 fBinormal;

uniform int Mode;
uniform bool RenderBackSide;
uniform bool UseSpecularMap;

uniform sampler2D ColorMap;
uniform sampler2D NormalMap;
uniform sampler2D SpecularMap;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	int shininess;
};

struct Light
{
	vec3 direction;
	vec3 diffuse;
	vec3 specular;
};

uniform Material FrontMaterial;
uniform Material BorderMaterial;

const Light lightSource = Light(
	normalize(vec3(1.0, 1.0, 1.0)),
	vec3(0.4),
	vec3(0.8)
);

vec3 fragNormal;
mat3 tbn;

vec3 GetDiffuse(vec3 lightDir, vec3 lightDiffuse) {
	float diffuseCoeff = max(0.0, dot(fragNormal, lightDir));
	return diffuseCoeff * lightDiffuse;
}

vec3 GetSpecular(vec3 lightDir, vec3 lightSpecular)
{
	vec3 viewDir = normalize(-fPosition);
	vec3 halfDir = normalize(lightDir + viewDir);
	float specAngle = max(0.0, dot(fragNormal, halfDir));
	float specCoeff = pow(specAngle, 0.3*FrontMaterial.shininess);
	return specCoeff * lightSpecular;
}

vec4 PhongLight(const Light l)
{
	vec3 matDiffuse;

	bool border = false;
	if (RenderBackSide) {
		border = true;
		matDiffuse = FrontMaterial.diffuse;
	}
	else if (Mode == 0) {
		vec4 texel = texture2D(ColorMap, fTexCoord);
		if (texel.a == 0) {
			matDiffuse = BorderMaterial.diffuse;
			border = true;
		}
		else matDiffuse = FrontMaterial.diffuse;
	}
	else matDiffuse = FrontMaterial.diffuse;

	vec3 ambient = (border ? BorderMaterial : FrontMaterial).ambient;
	vec3 c = (ambient + GetDiffuse(l.direction, l.diffuse)) * matDiffuse;
	if (Mode == 1 || border) {
		vec3 specular = UseSpecularMap ? vec3(texture2D(SpecularMap, fTexCoord)) : FrontMaterial.specular;
		c += GetSpecular(l.direction, l.specular) * specular;
	}
	
	return vec4(c, 1);
}

void main()
{
	if (Mode == 4) {
		gl_FragColor = vec4(FrontMaterial.diffuse, texture2D(ColorMap, fTexCoord).a);
	}
	else
	if (Mode == 3) {
		gl_FragColor = vec4(FrontMaterial.diffuse, 1);
	}
	else {
		if (RenderBackSide) {
			fragNormal = normalize(fNormal);
		}
		else {
			vec3 t = vec3(texture2D(NormalMap, fTexCoord));
			fragNormal = t * 2 - vec3(1);
			tbn = mat3(fTangent, fBinormal, fNormal);
			fragNormal = normalize(tbn * fragNormal);
		}

		gl_FragColor = PhongLight(lightSource);
	}
}