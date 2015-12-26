varying vec3 fPosition;
varying vec2 fTexCoord;
varying vec3 fNormal;
varying vec3 fTangent;
varying vec3 fBinormal;

uniform int Mode;
uniform bool UseNormalMap;

uniform sampler2D ColorMap;
uniform sampler2D NormalMap;
uniform sampler2D SpecularMap;
uniform sampler2D DecalMap;

struct Material {
	vec3 diffuse;
	vec3 ambient;
	int shininess;
};

struct Light
{
	vec3 position;
	vec3 diffuse;
	vec3 specular;
};

uniform Material FrontMaterial;

const Light lightSource = Light(
	vec3(1.0, 1.0, 1.0),
	vec3(0.4),
	vec3(0.8)
);

vec3 fragNormal;
mat3 tbn;

vec3 GetDiffuse(vec3 lightDir, vec3 lightDiffuse) {
	float diffuseCoeff = max(0.0, dot(fragNormal, lightDir));
	return diffuseCoeff * lightDiffuse;
}

vec3 GetSpecular(vec3 lightDir, vec3 lightSpecular, int shininess)
{
	vec3 viewDir = normalize(-fPosition);
	vec3 halfDir = normalize(lightDir + viewDir);
	float specAngle = max(0.0, dot(fragNormal, halfDir));
	float specCoeff = pow(specAngle, 0.3*shininess);
	return specCoeff * lightSpecular;
}

vec4 PhongLight(Light l)
{
	vec3 lightDir = normalize(l.position);
	vec3 matDiffuse;

	bool corner = false;
	if (Mode == 0) {
		vec3 texel = texture(ColorMap, fTexCoord);
		if (texel == vec3(0)) {
			matDiffuse = vec3(0.35);
			corner = true;
		}
		else matDiffuse = FrontMaterial.diffuse;
		//else matDiffuse = mix(FrontMaterial.diffuse*1.3, FrontMaterial.diffuse * vec3(texture(DecalMap, fTexCoord)), 0.5);
	}
	else matDiffuse = FrontMaterial.diffuse;
	
	vec3 c = ((corner ? vec3(0) : FrontMaterial.ambient) + GetDiffuse(lightDir, l.diffuse)) * matDiffuse;
	if (Mode == 1) {
		c += GetSpecular(lightDir, l.specular, FrontMaterial.shininess) * vec3(texture(SpecularMap, fTexCoord));
	}
	else if (corner) {
		c += GetSpecular(lightDir, l.specular, 200) * vec3(texture(SpecularMap, fTexCoord));
	}
	return vec4(c, 1);
}

void main()
{
	if (Mode == 3) {
		gl_FragColor = vec4(FrontMaterial.diffuse, 1);
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

		gl_FragColor = PhongLight(lightSource);
	}
}