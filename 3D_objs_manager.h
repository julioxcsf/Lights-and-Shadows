#ifndef OBJ_MANAGER_H
#define OBJ_MANAGER_H

#include <vector>
#include <string>
#include <map>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "OpenGL_Utils.h"
#include "Materials.h"
#include "esfera.h"
#include "Torus.h"
#include "ImportedModel.h"
#include "light_manager.h"


// Tipos de objetos que podem ser criados
enum TipoObjeto3D {
    CUBE,
    SKYBOX,
    PYRAMID,
    SPHERE,
    FLOOR,
    TORUS,
    LOAD_MODEL_3D,
};

enum estiloDesenho {
    TEXTURA,
    TRIANGULOS,
    LINHAS,
    PONTOS
};

enum ShaderType {
    ADS_GOURAUD_SHADER,
    ADS_PHONG_SHADER,
    ADS_BLINN_PHONG_SHADER,
    RESET_SHADER
};

struct ObjMaterial {
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float shininess;
};

struct Object3D {
    GLuint VAO;
    GLuint VBO[3];
    GLuint EBO; // Opcional, se usar índices
    GLuint texture;
    ObjMaterial material;
    int numIndices;
    int numVertices = 0;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 mMat;
    TipoObjeto3D type;
    std::string stringType;
    int ID;
    bool isLight = false;
    bool isSkyBox = false;
};



struct ShaderData {
    GLuint programID;
    GLuint mvLoc;
    GLuint projLoc;
    GLuint nLoc;
    GLuint sLoc;
    GLuint isLightLoc;
};

class ObjectManager {
public:
    ObjectManager();
    bool init();
    
    //void drawAll(); // old method withou shadow
    
    // uptade methods
    void updateModelMatrix(int objID);
    void updateObjectMaterials(int objID, Material newMaterial);
    void updateViewMatrix(glm::mat4 vMat);
    void updateProjectionMatrix(glm::mat4 pMat);
    void updateObjectTransform(int objID, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
        bool stateLight = false);
    void resetShaders();
    
    void updateSkyBoxesModelMatrix(glm::mat4 new_mMat);
    bool updateSkyBoxTexture(int objID, const char* path = "Texture/skybox1.jpg", bool path_is_folder = false);
    
    // select methods
    GLuint selectShaderProgram(ShaderType shaderSelected);
    GLuint selectShaderProgram();
    void selectStyle(estiloDesenho estilo);

    // getObject parametrer methods
    std::string getObjtectType(int objID);
    glm::vec3& getObjectPos(int objID);
    glm::vec3& getObjectRotation(int objID);
    glm::vec3& getObjectScale(int objID);

    // Add object 3D methods
    int add(TipoObjeto3D type, const glm::vec3& position, const glm::vec3& rotation = glm::vec3(0.0f),
                    const glm::vec3& scale = glm::vec3(1.0f), const char* modelPath = "dragon.obj");
    Object3D createSphere(const glm::vec3& position = { 0.0f , 0.0f, 0.0f }, const glm::vec3& rotation = { 0.0f , 0.0f, 0.0f }, const glm::vec3& scale = { 1.0f, 1.0f, 1.0f });
    Object3D createTorus(const glm::vec3& position = { 0.0f , 0.0f, 0.0f }, const glm::vec3& rotation = { 0.0f , 0.0f, 0.0f }, const glm::vec3& scale = { 1.0f, 1.0f, 1.0f });
    Object3D createFloor(const glm::vec3& position = { 0.0f , 0.0f, 0.0f }, const glm::vec3& rotation = { 0.0f , 0.0f, 0.0f }, const glm::vec3& scale = { 1.0f, 1.0f, 1.0f });
    Object3D createCube(const glm::vec3& position = { 0.0f , 0.0f, 0.0f }, const glm::vec3& rotation = { 0.0f , 0.0f, 0.0f }, const glm::vec3& scale = { 1.0f, 1.0f, 1.0f });
    Object3D createCubeSkyBox(const glm::vec3& position = { 0.0f , 0.0f, 0.0f }, const glm::vec3& rotation = { 0.0f , 0.0f, 0.0f }, const glm::vec3& scale = { 1.0f, 1.0f, 1.0f });
    Object3D createPyramid(const glm::vec3& position = { 0.0f , 0.0f, 0.0f }, const glm::vec3& rotation = { 0.0f , 0.0f, 0.0f }, const glm::vec3& scale = { 1.0f, 1.0f, 1.0f });
    Object3D loadModel(const char* modelPath = "dragon.obj", const glm::vec3& position = { 0.0f , 0.0f, 0.0f }, const glm::vec3& rotation = { 0.0f , 0.0f, 0.0f }, const glm::vec3& scale = { 1.0f, 1.0f, 1.0f });
    
    // render pethods
    void passOne();
    void passTwo();

    // light methods
    void addlight(typeLight tipo, int largura, int altura);

    int getNumObjects() const { return objects.size(); }
    std::vector<Object3D> objects;
    std::vector<Object3D> skyBoxes;
    LightManager lightManager;

private:
    int numObjs, numSkyBoxes;
    bool checkObjID(int ID);
    //std::vector<Light> lights; // Dados da luz usados no Pass 1 e Pass 2    REMOVER
    ShaderData renderingProgram; // salva o ID do renderingProgram
    ShaderType shaderUsing;
    GLuint renderingProgramShadow, cubeMapShaderProgram; //renderingPrograms auxiliares
    estiloDesenho estiloUsado; // salva o estilo usado
    glm::mat4 viewMatrix, projectionMatrix, mvMat, invTrMat, shadowMVP1 , shadowMVP2;
    glm::mat4 mMat_skyBox;
    std::map<ShaderType, ShaderData> shaders; // dicionario de tipo com o ID do respectivo renderinProgram
    void initializeShaders(bool debug = true);
    
};

#endif // OBJ_MANAGER_H
