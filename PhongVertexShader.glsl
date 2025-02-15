#version 430

#define MAX_LIGHTS 10

layout (location = 0) in vec3 vertPos;      // model space
layout (location = 1) in vec2 tex_coord;   // model space
layout (location = 2) in vec3 vertNormal;   // model space

out vec4 shadowCoord[MAX_LIGHTS];

out vec3 varyingNormal;                     // eye space vertex normal
out vec3 varyingLightDir[MAX_LIGHTS];       // vector pointing to the light
out vec3 varyingVertPos;                    // eye space vertex position

out vec2 tc;

struct Light {   
    vec4 ambient;   
    vec4 diffuse;   
    vec4 specular;
    vec3 position;  
    vec3 direction; 
    int type;       // 0 = direcional, 1 = pontual
    mat4 shadowMVP2;                                //shadow!!!
};

struct Material { 
    vec4 ambient;   
    vec4 diffuse;   
    vec4 specular;
    float shininess;
};

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 norm_matrix; // for transforming normals

uniform vec4 globalAmbient;
uniform Light lights[MAX_LIGHTS];
uniform int numLights; // Número de luzes ativas
uniform Material material;

//uniform int drawMode; // 0 para GL_LINE_STRIP, 1 para GL_TRIANGLES, etc.

void main(void)
{   // output vertex position, light direction, and normal to the rasterizer for interpolation
    varyingVertPos = (mv_matrix * vec4(vertPos,1.0)).xyz; //esse precisa ser normalizado no fragment

    for (int i = 0; i < numLights; i++) {
    // é a resultande de variação da direção de uma luz para CADA vertice
        varyingLightDir[i] = (lights[i].position - varyingVertPos);
        
        shadowCoord[i] = (lights[i].shadowMVP2 * vec4(vertPos,1.0) + vec4(0.0, 0.0, -0.002, 0.0));
    }
    
    varyingNormal = normalize((norm_matrix * vec4(vertNormal,1.0)).xyz);

    tc = tex_coord;
    gl_Position = proj_matrix * mv_matrix * vec4(vertPos,1.0);

}