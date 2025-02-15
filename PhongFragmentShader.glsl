#version 430

#define MAX_LIGHTS 10
#define PRECISION 3 //ideal

in vec4 shadowCoord[MAX_LIGHTS];

in vec3 varyingNormal;
in vec3 varyingLightDir[MAX_LIGHTS];
in vec3 varyingVertPos;

in vec2 tc;

out vec4 fragColor;

layout (binding=0) uniform sampler2DShadow shadowTex[MAX_LIGHTS]; // not used in vertex shader
layout (binding=1) uniform sampler2D skyBox_texture;

struct Light {   
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    vec3 position;
	vec3 direction;
    int type;       // 0 = direcional, 1 = pontual
	mat4 shadowMVP2; //shadow!!!
};

struct Material { 
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform vec4 globalAmbient;
uniform Light lights[MAX_LIGHTS];
uniform int numLights; // Número de luzes ativas
uniform Material material;
uniform int IsLight;
uniform bool IsSkyBox;

uniform mat4 norm_matrix; // for transforming normals

// Percent Closer Filtering
float PCF(sampler2DShadow shadowMap, vec4 shadowCoord) {
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0); // Calcula o tamanho de um texel

    // Amostras ao redor do ponto central
    for (int x = -PRECISION; x <= PRECISION; x++) {
        for (int y = -PRECISION; y <= PRECISION; y++) {
            vec2 offset = vec2(x, y) * texelSize;
            //shadow += textureProj(shadowMap, vec4(shadowCoord.xy + offset, shadowCoord.z, shadowCoord.w));
            shadow += textureProj(shadowMap, shadowCoord +
			vec4(x * 0.0008 * shadowCoord.w, y * 0.0008 * shadowCoord.w, -0.001, 0.0));
        }
    }
    
    return shadow / ( (2*PRECISION+1) * (2*PRECISION+1) ); // Média das amostras (3x3)
}

vec4 lighting_and_Shadow() {
// normalize the light, normal, and view vectors:
	//vec3 N = normalize(varyingNormal); normalizar no vextex é mais economico
	vec3 N = normalize(varyingNormal);
	vec3 V = normalize(-varyingVertPos); //esse precisa ser normalizado aqui?? por que??
	float notInShadow[MAX_LIGHTS];
	float Strength = 0.98;

	vec4 shadowColor = globalAmbient * material.ambient; 

	vec4 ambient = vec4(0.0);
	vec4 diffuse[MAX_LIGHTS];
	vec4 specular[MAX_LIGHTS];

	vec4 fragColor_saida = vec4(0.0);

	for(int i = 0; i < numLights; i++) {
	    vec3 L = normalize(varyingLightDir[i]);

		if (lights[i].type == 0) { //direcional
			L = -lights[i].direction;
		}
		
		vec3 R = normalize(reflect(-L, N));
		//vec3 R = reflect(-L, N);

		float cosTheta = dot(L,N);
		float cosPhi = dot(V,R);

		shadowColor += + lights[i].ambient * material.ambient; //ok

		if (lights[i].type == 0) { //direcional
			float diffuse_f = max(0.0, dot(varyingNormal, L));
			float specular_f = max(0.0, dot(varyingNormal, R));

			if (diffuse_f == 0.0)
				specular_f = 0.0;
			else
				specular_f = pow(specular_f * Strength, material.shininess); // sharpen the highlight

			diffuse[i] = lights[i].diffuse * material.diffuse * diffuse_f;
			specular[i] = lights[i].specular * specular_f * Strength;
		}
		else {
			diffuse[i] = lights[i].diffuse * material.diffuse * max(cosTheta,0.0);
		
			float shine = 0.0;
			if (material.shininess > 0.0) {
				shine = pow(max(cosPhi, 0.0), material.shininess);
			}
			specular[i] = lights[i].specular * material.specular * shine;
		}

		
		//TROCANDO VERIFICAÇÃO DIRETA DE SOMBRA POR PCF
		//verifico para cada luz se o ponto está na sombra ou não
		//notInShadow[i] = textureProj(shadowTex[i], shadowCoord[i]);

		//if (notInShadow[i] == 1.0) {
		//	fragColor_saida += (diffuse[i] + specular[i]);
		//}

		// Aplicação do PCF para suavizar as sombras
        float shadowFactor = PCF(shadowTex[i], shadowCoord[i]);

        fragColor_saida += (diffuse[i] + specular[i]) * shadowFactor;
	}

	fragColor_saida += shadowColor;
	return fragColor_saida;
}

void main(void)
{ 
	if (IsLight == 1)
		fragColor = vec4(1.0f,0.95,0.70,1.0);
	else if (IsSkyBox)
		fragColor = texture(skyBox_texture,tc);
	else
		fragColor = lighting_and_Shadow();
}