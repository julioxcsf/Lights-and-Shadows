#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "OpenGL_Utils.h"

// A quantidade deve ser a mesma do shaders
#define MAX_LIGHTS          10


enum typeLight {
    DIRECIONAL,
    PONTUAL
};

enum lightProperties {
    POSITION = 1,
    DIRECTION = 2,
    AMBIENT_COLOR = 4,
    DIFFUSE_COLOR = 8,
    SPECULAR_COLOR = 16,
    FOV_ASPECT = 32,
    NEAR_FAR_PLANE = 64,
    ORTHOSIZE = 128,
};

struct Light {
    typeLight type;
    glm::vec3 position = glm::vec3(0.0f, 10.0f, 0.0f);
    glm::vec3 position_eyeSpace = glm::vec3(0.0f, 10.0f, 0.0f);
    glm::vec3 direction = glm::vec3(1.0f, -1.0f, 0.0f);
    glm::vec3 direction_eyeSpace = glm::vec3(1.0f, -1.0f, 0.0f);
    
    // colors
    glm::vec4 Ambient = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    glm::vec4 Diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    glm::vec4 Specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    // atrubutos de atualização automatica
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    GLuint shadowBuffer;
    GLuint shadowTexture;
    GLuint ambLoc, diffLoc, specLoc; // das cores
    GLuint posLoc, dirLoc,  typeLoc; // posicao, direcao e tipo
    GLuint shadowMVP2Loc;

    // para a cosntrucao das View Matrix e Pespective Matrix
    float fov, aspect;
    float nearPlane, farPlane;
    float orthonSize;
};

class LightManager {
public:
    Light lightSources[MAX_LIGHTS];
    glm::vec4 globalAmbientColor = glm::vec4(0.05f, 0.05f, 0.05f, 1.0f);
    int numLights = 0;
    void addLight(typeLight tipo, int largura, int altura);    
    void updateLightViewMatrix(int ID);
    void updateLightProjectionMatrix(int ID);
    void installLights(GLuint renderingProgram);
    void applyLights();
    void bindShadowBuffers(int ID);
    void updateViewMatrix(glm::mat4 new_vMat);


    void updateLightEyeSpace(int ID);
    void uptadeLightPhisicalProperties(int ID, int sumLightProperties, glm::vec3 pos, glm::vec3 dir);
    void uptadeLightPhisicalProperties(int ID, int sumLightProperties, glm::vec3 pos_or_dir);
    void uptadeLightColorProperties(int ID, int sumLightProperties, glm::vec4 color1);
    void uptadeLightColorProperties(int ID, int sumLightProperties, glm::vec4 color1, glm::vec4 color2);
    void uptadeLightColorProperties(int ID, int sumLightProperties, glm::vec4 color1, glm::vec4 color2, glm::vec4 color3);
    void uptadeLightProjectionMatrixProperties(int ID, int sumLightProperties, float value1, float value2 = 0.0f);

private:
    
    GLuint globalAmbLoc, posNumLightsLoc; // iluminacao global e numero de luzes
    bool verificarID(int ID, std::string FuncName);
    GLuint renderingProgramUsed;
    glm::mat4 vMat;

};

#endif // !LIGHT_MANAGER_H

