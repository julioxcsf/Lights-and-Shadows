#include <iostream>
#include "3D_objs_manager.h"
#include "OpenGL_Utils.h" // Supondo funções de carregamento de textura, shaders, etc.
#include "esfera.h"       // Implementação da esfera
#include "Torus.h"        // Implementação do toroide
#include "ImportedModel.h"// Para modelos OBJ

using namespace std;

ObjectManager::ObjectManager() {
    estiloUsado = TRIANGULOS;
    numObjs = 0;
    numSkyBoxes = 0;
}

bool ObjectManager::init() {
    //inicie apenas após o GLEW
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << "\n" << std::endl;
    initializeShaders();
    return true;
}

bool ObjectManager::checkObjID(int ID) {
    if (ID < 0 || ID >= objects.size()) {
        return false;
    }
    return true;
}

void ObjectManager::resetShaders() {
    setConsoleColor(FOREGROUND_BLUE);
    cout << "\n\nResetando todos os Shaders Programs...\n" << endl;
    resetConsoleColor();

    for (auto& pair : shaders) {
        GLuint oldID = pair.second.programID;
        if (oldID != 0) {
            glDeleteProgram(oldID);
            // zerar pair.second.programID se quiser
        }
    }
    // se quiser, também delete "renderingProgramShadow" se ele foi criado antes
    if (renderingProgramShadow) {
        glDeleteProgram(renderingProgramShadow);
    }
    shaders.clear(); // Limpar o map se quiser

    initializeShaders();
    //selectShaderProgram(shaderUsing); usado direto na main.cpp
}

Object3D ObjectManager::createSphere(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
    Sphere sphere(32); // Cria a esfera com precisão fixa (ou parametrizável)
    Object3D obj = { 0 };
    obj.stringType = "Esfera";
    obj.ID = numObjs;
    numObjs++;

    

    std::vector<int> ind = sphere.getIndices();
    std::vector<glm::vec3> vert = sphere.getVertices();
    std::vector<glm::vec2> tex = sphere.getTexCoords();
    std::vector<glm::vec3> norm = sphere.getNormals();

    //vetores para cambiar os vetores da esfera.-------------- Não poderia usar referencia??
    std::vector<float> pvalues; // vertex positions
    std::vector<float> tvalues; // texture coordinates
    std::vector<float> nvalues; // normal vectors

    obj.numIndices = sphere.getNumIndices();
    for (int i = 0; i < sphere.getNumVertices(); i++) {
        pvalues.push_back((vert[i]).x);
        pvalues.push_back((vert[i]).y);
        pvalues.push_back((vert[i]).z);
        tvalues.push_back((tex[i]).s);
        tvalues.push_back((tex[i]).t);
        nvalues.push_back((norm[i]).x);
        nvalues.push_back((norm[i]).y);
        nvalues.push_back((norm[i]).z);
    }


    glGenVertexArrays(1, &obj.VAO); // VAO gerado
    glBindVertexArray(obj.VAO); // VAO linkado
    glGenBuffers(3, obj.VBO); // 3 VBOS gerados
    glGenBuffers(1, &obj.EBO);


    // put the vertices into buffer #0
    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[0]); //OK
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(0); // VBO com vertices

    // put the texture coordinates into buffer #1
    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[1]); //OK
    glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(1); // VBO com textCoords

    // put the normals into buffer #2
    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[2]); //OK
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(2); // VBO com normais

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO); // indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.numIndices * 4, &ind[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

    obj.position = position;
    obj.rotation = rotation;
    obj.scale = scale;
    obj.type = SPHERE;

    return obj;
}

Object3D ObjectManager::createTorus(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
    Torus torus(0.5f, 0.2f, 48); // Parâmetros do toroide
    Object3D obj = { 0 };
    obj.stringType = "Torus";
    obj.ID = numObjs;
    numObjs++;

    std::vector<int> ind = torus.getIndices();
    std::vector<glm::vec3> vert = torus.getVertices();
    std::vector<glm::vec2> tex = torus.getTexCoords();
    std::vector<glm::vec3> norm = torus.getNormals();
    std::vector<float> pvalues;
    std::vector<float> tvalues;
    std::vector<float> nvalues;

    obj.numIndices = torus.getNumIndices();
    for (int i = 0; i < torus.getNumVertices(); i++) {
        pvalues.push_back(vert[i].x);
        pvalues.push_back(vert[i].y);
        pvalues.push_back(vert[i].z);
        tvalues.push_back(tex[i].s);
        tvalues.push_back(tex[i].t);
        nvalues.push_back(norm[i].x);
        nvalues.push_back(norm[i].y);
        nvalues.push_back(norm[i].z);
    }

    glGenVertexArrays(1, &obj.VAO);
    glBindVertexArray(obj.VAO);

    glGenBuffers(3, obj.VBO);
    glGenBuffers(1, &obj.EBO);

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[0]); // vertex positions
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(0); // VBO com vertices

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[1]); // texture coordinates
    glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(1); // VBO com textCoords

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[2]); // normal vectors
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(2); // VBO com normais

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO); // indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.numIndices * 4, &ind[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // VBO com vertices
    glEnableVertexAttribArray(1); // VBO com textCoords
    glEnableVertexAttribArray(2); // VBO com normais
    glBindVertexArray(0);

    obj.position = position;
    obj.rotation = rotation;
    obj.scale = scale;
    obj.type = TORUS;

    return obj;
}

Object3D ObjectManager::createFloor(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
    Object3D obj = { 0 };
    obj.ID = numObjs;
    obj.stringType = "Plano";
    numObjs++;

    glm::vec3 vertices[4] = { {-1.0f, 0.0f, -1.0f},
                              {-1.0f, 0.0f, 1.0f},
                              {1.0f, 0.0f, 1.0f},
                              {1.0f, 0.0f, -1.0f} };

    std::vector<int> ind = { 0,1,2,2,3,0 };

    glm::vec2 texCoord[4] = { {0.0f, 0.0f},
                              {0.0f, 1.0f},
                              {1.0f, 1.0f},
                              {1.0f, 0.0f} };

    glm::vec3 normals[4] = { {0.0f, 1.0f, 0.0f},
                             {0.0f, 1.0f, 0.0f},
                             {0.0f, 1.0f, 0.0f},
                             {0.0f, 1.0f, 0.0f} };

    std::vector<float> pvalues;
    std::vector<float> tvalues;
    std::vector<float> nvalues;

    obj.numIndices = ind.size(); // Atualizar número de índices

    for (int i = 0; i < 4/*num de vertices*/; i++) {
        //posicoes
        pvalues.push_back(vertices[i].x);
        pvalues.push_back(vertices[i].y);
        pvalues.push_back(vertices[i].z);
        // UV
        tvalues.push_back(texCoord[i].s);
        tvalues.push_back(texCoord[i].t);
        //normais
        nvalues.push_back(normals[i].x);
        nvalues.push_back(normals[i].y);
        nvalues.push_back(normals[i].z);
    }

    glGenVertexArrays(1, &obj.VAO);
    glBindVertexArray(obj.VAO);

    glGenBuffers(3, obj.VBO);
    glGenBuffers(1, &obj.EBO);

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[0]); // vertex positions
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(0); // VBO com vertices

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[1]); // texture coordinates
    glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(1); // VBO com textCoords

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[2]); // normal vectors
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(2); // VBO com normais

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO); // indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * 4, &ind[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // VBO com vertices
    glEnableVertexAttribArray(1); // VBO com textCoords
    glEnableVertexAttribArray(2); // VBO com normais
    glBindVertexArray(0);

    obj.position = position;
    obj.rotation = rotation;
    obj.scale = scale;
    obj.type = FLOOR;

    return obj;
}

Object3D ObjectManager::createCube(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
    Object3D obj = { 0 };
    obj.ID = numObjs;
    obj.stringType = "Cubo";
    numObjs++;

    glm::vec3 vertices[8] = { { 1.0f,  1.0f,  1.0f},    //0 D perto cima +
                              { 1.0f,  1.0f, -1.0f},    //1 D longe cima 
                              {-1.0f,  1.0f, -1.0f},    //2 E longe cima 
                              {-1.0f,  1.0f,  1.0f},    //3 E perto cima +
                              { 1.0f, -1.0f,  1.0f},    //4 D perto baixo +
                              { 1.0f, -1.0f, -1.0f},    //5 D longe baixo 
                              {-1.0f, -1.0f, -1.0f},    //6 E longe baixo 
                              {-1.0f, -1.0f,  1.0f} };  //7 E perto baixo +

    std::vector<int> ind = { 0,1,2,2,3,0 ,  //face superior OK
                             0,3,4,3,7,4 ,  //face perto    OK
                             5,2,1,5,6,2 ,  //face longe    OK
                             6,3,2,6,7,3 ,  //face esquerda OK
                             4,1,0,4,5,1 ,  //face direita  OK
                             6,5,4,4,7,6 }; //face inferior OK

    glm::vec2 texCoord[4] = { {0.0f, 0.0f},//Quantos eu preciso??
                              {1.0f, 0.0f},
                              {1.0f, 1.0f},
                              {0.0f, 1.0f} };
   
    glm::vec3 faceNormals[6] = {{ 0.0f,  1.0f,  0.0f}, // Face superior
                                { 0.0f,  0.0f,  1.0f}, // Face perto
                                { 0.0f,  0.0f, -1.0f}, // Face longe
                                {-1.0f,  0.0f,  0.0f}, // Face esquerda
                                { 1.0f,  0.0f,  0.0f}, // Face direita
                                { 0.0f, -1.0f,  0.0f } }; // Face inferior

    std::vector<int> vextex_to_normal_map = { 0,0,0,0,0,0,
                                              1,1,1,1,1,1,
                                              2,2,2,2,2,2,
                                              3,3,3,3,3,3,
                                              4,4,4,4,4,4,
                                              5,5,5,5,5,5 };

    std::vector<float> pvalues;
    std::vector<float> tvalues;
    std::vector<float> nvalues;

    obj.numIndices = ind.size(); // Atualizar número de índices

    for (int i = 0; i < obj.numIndices; i++) {
        int index = ind[i];
        //posicoes
        pvalues.push_back(vertices[index].x);
        pvalues.push_back(vertices[index].y);
        pvalues.push_back(vertices[index].z);
        // UV
        tvalues.push_back(texCoord[i%4].s); //altas chances de ter algo errado!!!!!!!
        tvalues.push_back(texCoord[i%4].t); //ver depoiss todas as texturas

        int normal_index = vextex_to_normal_map[i];
        nvalues.push_back(faceNormals[normal_index].x);
        nvalues.push_back(faceNormals[normal_index].y);
        nvalues.push_back(faceNormals[normal_index].z);
    }

    obj.numVertices = int(pvalues.size()) / 3;

    glGenVertexArrays(1, &obj.VAO);
    glBindVertexArray(obj.VAO);

    glGenBuffers(3, obj.VBO);
    glGenBuffers(1, &obj.EBO);

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[0]); // vertex positions
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(0); // VBO com vertices

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[1]); // texture coordinates
    glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(1); // VBO com textCoords

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[2]); // normal vectors
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(2); // VBO com normais

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO); // indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(int), &ind[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // VBO com vertices
    glEnableVertexAttribArray(1); // VBO com textCoords
    glEnableVertexAttribArray(2); // VBO com normais
    glBindVertexArray(0);

    obj.position = position;
    obj.rotation = rotation;
    obj.scale = scale;
    obj.type = CUBE;

    return obj;
}

Object3D ObjectManager::createCubeSkyBox(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
    Object3D obj = { 0 };
    obj.ID = numSkyBoxes;
    obj.stringType = "SkyBox";
    numSkyBoxes++;

    glm::vec3 vertices[8] = { { 1.0f,  1.0f,  1.0f},    //0 D perto cima +
                              { 1.0f,  1.0f, -1.0f},    //1 D longe cima 
                              {-1.0f,  1.0f, -1.0f},    //2 E longe cima 
                              {-1.0f,  1.0f,  1.0f},    //3 E perto cima +
                              { 1.0f, -1.0f,  1.0f},    //4 D perto baixo +
                              { 1.0f, -1.0f, -1.0f},    //5 D longe baixo 
                              {-1.0f, -1.0f, -1.0f},    //6 E longe baixo 
                              {-1.0f, -1.0f,  1.0f} };  //7 E perto baixo +

    std::vector<int> ind = { 2,1,0,0,3,2 ,  //face superior OK
                             4,7,0,7,3,0 ,  //face perto    OK
                             2,6,5,5,1,2 ,  //face longe    OK
                             7,6,3,6,2,3,  //face esquerda  OK
                             5,4,1,4,0,1 ,  //face direita  OK
                             7,4,5,5,6,7 }; //face inferior OK

    float cubeTextureCoord[72] = {
         0.25f, 1.00f,  0.50f, 1.00f,  0.50f, 0.66f,  // Face Superior - Superior Direita
         0.50f, 0.66f,  0.25f, 0.66f,  0.25f, 1.00f,   // Face Superior - Inferior Esquerda

         0.50f, 0.33f,  0.25f, 0.33f,  0.50f, 0.66f,  // Face Frontal - Inferior Direita
         0.25f, 0.33f,  0.25f, 0.66f,  0.50f, 0.66f,  // Face Frontal - Superior Esquerda

         1.00f, 0.66f,  1.00f, 0.33f,  0.75f, 0.33f,  // Face Traseira - Inferior Direita
         0.75f, 0.33f,  0.75f, 0.66f,  1.00f, 0.66f,  // Face Traseira - Superior Esquerda

         0.25f, 0.33f,  0.00f, 0.33f,  0.25f, 0.66f,  // Face Esquerda - Inferior Direita
         0.00f, 0.33f,  0.00f, 0.66f,  0.25f, 0.66f,  // Face Esquerda - Superior Esquerda

         0.75f, 0.33f,  0.50f, 0.33f,  0.75f, 0.66f,  // Face Direita - Inferior Direita
         0.50f, 0.33f,  0.50f, 0.66f,  0.75f, 0.66f,  // Face Direita - Superior Esquerda    

         0.25f, 0.33f,  0.50f, 0.33f,  0.50f, 0.00f,  // Face Inferior - Superior Direita
         0.50f, 0.00f,  0.25f, 0.00f,  0.25f, 0.33f  // Face Inferior - Inferior Esquerda

         
     };

    glm::vec3 faceNormals[6] = { { 0.0f,  1.0f,  0.0f}, // Face superior
                                { 0.0f,  0.0f,  1.0f}, // Face perto
                                { 0.0f,  0.0f, -1.0f}, // Face longe
                                {-1.0f,  0.0f,  0.0f}, // Face esquerda
                                { 1.0f,  0.0f,  0.0f}, // Face direita
                                { 0.0f, -1.0f,  0.0f } }; // Face inferior

    std::vector<int> vextex_to_normal_map = { 0,0,0,0,0,0,
                                              1,1,1,1,1,1,
                                              2,2,2,2,2,2,
                                              3,3,3,3,3,3,
                                              4,4,4,4,4,4,
                                              5,5,5,5,5,5 };

    std::vector<float> pvalues;
    std::vector<float> nvalues;

    obj.numIndices = ind.size(); // Atualizar número de índices

    for (int i = 0; i < obj.numIndices; i++) {
        int index = ind[i];
        //posicoes
        pvalues.push_back(vertices[index].x);
        pvalues.push_back(vertices[index].y);
        pvalues.push_back(vertices[index].z);

        int normal_index = vextex_to_normal_map[i];
        nvalues.push_back(faceNormals[normal_index].x);
        nvalues.push_back(faceNormals[normal_index].y);
        nvalues.push_back(faceNormals[normal_index].z);
    }

    obj.numVertices = int(pvalues.size()) / 3;

    glGenVertexArrays(1, &obj.VAO);
    glBindVertexArray(obj.VAO);

    glGenBuffers(3, obj.VBO);
    //glGenBuffers(1, &obj.EBO);

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[0]); // vertex positions
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(0); // VBO com vertices

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[1]); // texture coordinates
    glBufferData(GL_ARRAY_BUFFER, 72 * sizeof(float), &cubeTextureCoord[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    //glEnableVertexAttribArray(1); // VBO com textCoords

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[2]); // normal vectors
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    //glEnableVertexAttribArray(2); // VBO com normais

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO); // indices
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(int), &ind[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // VBO com vertices
    glEnableVertexAttribArray(1); // VBO com textCoords
    glEnableVertexAttribArray(2); // VBO com normais
    glBindVertexArray(0);

    obj.position = position;
    obj.rotation = rotation;
    obj.scale = scale;
    obj.type = SKYBOX;

    return obj;
}

Object3D ObjectManager::createPyramid(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
    Object3D obj = { 0 };
    obj.ID = numObjs;
    obj.stringType = "Piramide";
    numObjs++;

    glm::vec3 vertices[5] = { { 0.0f,  1.0f,  0.0f},    //0 Centro
                              { 1.0f,  0.0f,  1.0f},    //1 D perto baixo 
                              { 1.0f,  0.0f, -1.0f},    //2 D longe baixo 
                              {-1.0f,  0.0f, -1.0f},    //3 E longe baixo 
                              {-1.0f,  0.0f,  1.0f} };  //4 E perto baixo 

    std::vector<int> ind = { 3,2,1, 4,3,1, //Base  ok
                             0,4,1, //face perto   ok
                             2,3,0, //face longe   ok
                             1,2,0, //face direita ok
                             3,4,0};//face esquerda


    glm::vec2 texCoord[4] = { {0.0f, 0.0f},//corrigir depois
                              {1.0f, 0.0f},
                              {1.0f, 1.0f},
                              {0.0f, 1.0f} };

    glm::vec3 faceNormals[5] = {{ 0.0f  , -1.0f  ,  0.0f  },   // base
                                { 0.0f  ,  0.707f,  0.707f},   // Face perto
                                { 0.0f  ,  0.707f, -0.707f},   // Face longe
                                {-0.707f,  0.707f,  0.0f  },   // Face esquerda
                                { 0.707f,  0.707f,  0.0f  } }; // Face direita

    std::vector<int> vextex_to_normal_map = { 0,0,0,0,0,0, //base
                                              1,1,1,       //face perto
                                              2,2,2,       //face longe
                                              4,4,4,       //face direita
                                              3,3,3};      //face esquerda

    std::vector<float> pvalues;
    std::vector<float> tvalues;
    std::vector<float> nvalues;

    obj.numIndices = ind.size(); // Atualizar número de índices

    for (int i = 0; i < obj.numIndices; i++) {
        int index = ind[i];
        //posicoes
        pvalues.push_back(vertices[index].x);
        pvalues.push_back(vertices[index].y);
        pvalues.push_back(vertices[index].z);
        // UV
        tvalues.push_back(texCoord[i % 4].s); //altas chances de ter algo errado!!!!!!!
        tvalues.push_back(texCoord[i % 4].t); //ver depoiss todas as texturas

        int normal_index = vextex_to_normal_map[i];
        nvalues.push_back(faceNormals[normal_index].x);
        nvalues.push_back(faceNormals[normal_index].y);
        nvalues.push_back(faceNormals[normal_index].z);
    }

    obj.numVertices = int(pvalues.size()) / 3;

    glGenVertexArrays(1, &obj.VAO);
    glBindVertexArray(obj.VAO);

    glGenBuffers(3, obj.VBO);
    glGenBuffers(1, &obj.EBO);

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[0]); // vertex positions
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(0); // VBO com vertices

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[1]); // texture coordinates
    glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(1); // VBO com textCoords

    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[2]); // normal vectors
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(2); // VBO com normais

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.EBO); // indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(int), &ind[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0); // VBO com vertices
    glEnableVertexAttribArray(1); // VBO com textCoords
    glEnableVertexAttribArray(2); // VBO com normais
    glBindVertexArray(0);

    obj.position = position;
    obj.rotation = rotation;
    obj.scale = scale;
    obj.type = PYRAMID;

    return obj;
}

Object3D ObjectManager::loadModel(const char* modelPath, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale) {
    ImportedModel model(modelPath); // Carrega o modelo
    Object3D obj = { 0 };
    // Encontra o último ponto
    string path = modelPath;
    size_t lastDot = path.find_last_of('.');
    std::string filename = path.substr(0, lastDot);
    obj.stringType = "Modelo 3D: " + filename;
    obj.ID = numObjs;
    numObjs++;

    std::vector<glm::vec3> vert = model.getVertices();
    std::vector<glm::vec2> tex = model.getTextureCoords();
    std::vector<glm::vec3> norm = model.getNormals();
    int numVerticesModel = model.getNumVertices();
    cout << "Numero de vertices do objeto 3D: " << numVerticesModel << endl;
    std::vector<float> pvalues; // vertex positions
    std::vector<float> tvalues; // texture coordinates
    std::vector<float> nvalues; // normal vectors

    obj.numVertices = numVerticesModel;
    for (int i = 0; i < numVerticesModel; i++) {
        pvalues.push_back((vert[i]).x);
        pvalues.push_back((vert[i]).y);
        pvalues.push_back((vert[i]).z);
        tvalues.push_back((tex[i]).s);
        tvalues.push_back((tex[i]).t);
        nvalues.push_back((norm[i]).x);
        nvalues.push_back((norm[i]).y);
        nvalues.push_back((norm[i]).z);
    }

    glGenVertexArrays(1, &obj.VAO);
    glBindVertexArray(obj.VAO);

    glGenBuffers(3, obj.VBO);

    // VBO for vertex locations
    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, pvalues.size() * 4, &pvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(0); // VBO com vertices

    // VBO for texture coordinates
    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, tvalues.size() * 4, &tvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(1); // VBO com textCoords

    // VBO for normal vectors
    glBindBuffer(GL_ARRAY_BUFFER, obj.VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, nvalues.size() * 4, &nvalues[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // Define layout do VBO
    glEnableVertexAttribArray(2); // VBO com normais

    glBindVertexArray(0);

    obj.position = position;
    obj.rotation = rotation;
    obj.scale = scale;
    obj.type = LOAD_MODEL_3D;

    return obj;
}

int ObjectManager::add(TipoObjeto3D type, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const char* modelPath) {
    Object3D obj;
    switch (type) {
    case FLOOR:
        obj = createFloor(position, rotation, scale);
        cout << "Chao adicionado." << endl;
        break;
    case CUBE:
        obj = createCube(position, rotation, scale);
        cout << "Cubo adicionado." << endl;
        break;
    case SKYBOX:
        obj = createCubeSkyBox(position, rotation, scale);
        cout << "SkyBox adicionada." << endl;
        skyBoxes.push_back(obj);
        return skyBoxes.size();
    case PYRAMID:
        obj = createPyramid(position, rotation, scale);
        cout << "Piramide adicionada." << endl;
        break;
    case SPHERE:
        obj = createSphere(position, rotation, scale);
        cout << "Esfera adicionada." << endl;
        break;
    case TORUS:
        obj = createTorus(position, rotation, scale);
        cout << "Torus adicionado." << endl;
        break;
    case LOAD_MODEL_3D:
        obj = loadModel(modelPath, position, rotation, scale);
        cout << "Modelo externo adicionado." << endl;
        break;
    default:
        string textoErro = "modelo " + std::to_string(type) + " nao implementado.";
        printError(textoErro.c_str(),"ObjectManager::add");
    }
    // Adiciona o objeto à lista antes de atualizar
    objects.push_back(obj);

    // Atualiza a Model Matrix para o objeto recém-adicionado
    updateModelMatrix(objects.size() - 1);
    updateObjectMaterials(objects.size() - 1, FULL);
    return objects.size();
}

void ObjectManager::updateObjectTransform(int objID, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
    bool stateLight) {
    // Verifica se o ID do objeto é válido
    if (objID < 0 || objID >= objects.size()) {
        std::cerr << "Erro: ID de objeto inválido (" << objID << ")." << std::endl;
        return;
    }

    // Atualiza posição, rotação e escala
    objects[objID].position = position;
    objects[objID].rotation = rotation;
    objects[objID].scale = scale;
    objects[objID].isLight = stateLight;

    updateModelMatrix(objID);
}

void ObjectManager::updateModelMatrix(int indice_obj) {
    if (objects.size() > 0 && objects.size() > indice_obj) {
        // é importante comentar que a ordem das operações é inversa à escrita aqui
        // devido a assiciatividade, a ultima transformacao da ponta e a primeira usada e vai da direita para a esquerda
        // mMat = translate x rotate x scale x (entrada)
        glm::mat4 mMat = glm::mat4(1.0f);
        mMat = glm::translate(mMat, objects[indice_obj].position);
        mMat = glm::rotate(mMat, glm::radians(objects[indice_obj].rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        mMat = glm::rotate(mMat, glm::radians(objects[indice_obj].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        mMat = glm::rotate(mMat, glm::radians(objects[indice_obj].rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        mMat = glm::scale(mMat, objects[indice_obj].scale);
        objects[indice_obj].mMat = mMat;
    }
    else {
        cout << "Nenhum objeto 3D foi adicionado" << endl;
    }
}

void ObjectManager::updateObjectMaterials(int objID, Material newMaterial) {
    glm::vec4& matAmb = objects[objID].material.ambient;
    glm::vec4& matDif = objects[objID].material.diffuse;
    glm::vec4& matSpe = objects[objID].material.specular;
    float& matShi = objects[objID].material.shininess;

    getMaterial(newMaterial, matAmb, matDif, matSpe, matShi);
}

bool ObjectManager::updateSkyBoxTexture(int objID, const char* path, bool path_is_folder) {
    if (objID != 0) {
        printError("ID informado de skybox invalido", "ObjectManager::uptadeSkyboxTexture");
        return false;
    }

    if (path_is_folder) {
        skyBoxes[objID].texture = loadCubeMap(path);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); //nao sei o que faz
    }
    else
        skyBoxes[objID].texture = loadTexture(path);

    if (skyBoxes[objID].texture != 0) {
        string texID = "Textura carregada. TexID: " + to_string(skyBoxes[objID].texture);
        printColorido(GREEN, texID.c_str(), "ObjectManager::updateSkyBoxTexture");
        return true;
    }
    printError("Textura nao carregada.", "ObjectManager::updateSkyBoxTexture");
    return false;
}

glm::vec3& ObjectManager::getObjectPos(int objID) {
    // Verifica se o ID do objeto é válido
    if (objID < 0 || objID >= objects.size()) {
        throw std::out_of_range("ID de objeto inválido.");
    }
    return objects[objID].position;
}

glm::vec3& ObjectManager::getObjectRotation(int objID) {
    // Verifica se o ID do objeto é válido
    if (objID < 0 || objID >= objects.size()) {
        throw std::out_of_range("ID de objeto inválido.");
    }
    return objects[objID].rotation;
}

glm::vec3& ObjectManager::getObjectScale(int objID) {
    // Verifica se o ID do objeto é válido
    if (objID < 0 || objID >= objects.size()) {
        throw std::out_of_range("ID de objeto inválido.");
    }
    return objects[objID].scale;
}

string ObjectManager::getObjtectType(int objID) {
    // Verifica se o ID do objeto é válido
    if (objID < 0 || objID >= objects.size()) {
        throw std::out_of_range("ID de objeto inválido.");
    }
    return objects[objID].stringType;
}

void ObjectManager::initializeShaders(bool debug) {
    vector<ShaderData> shadersTemp;
    ShaderData ADS_Gouraud_Shader, ADS_Phong_Shader, ADS_Blinn_Phong_Shader;

    ADS_Gouraud_Shader.programID = createShaderProgram("Shaders/ADSvertexShader.glsl", "Shaders/ADSfragmentShader.glsl", debug);
    shadersTemp.push_back(ADS_Gouraud_Shader);

    ADS_Phong_Shader.programID = createShaderProgram("Shaders/PhongVertexShader.glsl", "Shaders/PhongFragmentShader.glsl", debug);
    shadersTemp.push_back(ADS_Phong_Shader);

    ADS_Blinn_Phong_Shader.programID = createShaderProgram("Shaders/Blinn-PhongVertexShader.glsl", "Shaders/Blinn-PhongFragmentShader.glsl", debug);
    shadersTemp.push_back(ADS_Blinn_Phong_Shader);

    for (unsigned i = 0; i < shadersTemp.size(); i++) {
        shadersTemp[i].mvLoc = glGetUniformLocation(shadersTemp[i].programID, "mv_matrix");
        shadersTemp[i].projLoc = glGetUniformLocation(shadersTemp[i].programID, "proj_matrix");
        shadersTemp[i].nLoc = glGetUniformLocation(shadersTemp[i].programID, "norm_matrix");
    }

    shaders[ADS_GOURAUD_SHADER] = shadersTemp[0];
    shaders[ADS_PHONG_SHADER] = shadersTemp[1];
    shaders[ADS_BLINN_PHONG_SHADER] = shadersTemp[2];

    renderingProgramShadow = createShaderProgram("Shaders/pass1ShadowVertex.glsl", "Shaders/pass1ShadowFragment.glsl", debug);
    cubeMapShaderProgram = createShaderProgram("Shaders/CubeMapVertexShader.glsl", "Shaders/CubeMapFragmentShader.glsl", debug);
}

GLuint ObjectManager::selectShaderProgram(ShaderType shaderSelected) { //preciso de ajuda aqui agora....
    auto it = shaders.find(shaderSelected);
    if (it != shaders.end()) {
        ShaderData& shader = it->second; // Pegue a referência ao ShaderData
        renderingProgram.programID = shader.programID;
        shaderUsing = shaderSelected;
        cout << "Shader alterado com sucesso." << " ShaderID: " << renderingProgram.programID << endl;
        return shader.programID;
    }
    std::cerr << "Shader program nao encontrado!" << std::endl;
    renderingProgram.programID = 0;
    return 0; // Retorna um programa inválido para indicar erro
}

GLuint ObjectManager::selectShaderProgram() { //sobrecarga
    auto it = shaders.find(shaderUsing);
    if (it != shaders.end()) {
        ShaderData& shader = it->second; // Pegue a referência ao ShaderData
        renderingProgram.programID = shader.programID;

        lightManager.installLights(renderingProgram.programID); // aquiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii

        cout << "Shader alterado com sucesso." << " ShaderID: " << renderingProgram.programID << endl;
        return shader.programID;
    }
    std::cerr << "Shader program não encontrado para o tipo de desenho selecionado!" << std::endl;
    renderingProgram.programID = 0;
    return 0; // Retorna um programa inválido para indicar erro
}

void ObjectManager::selectStyle(estiloDesenho estilo) {
    estiloUsado = estilo;
}

void ObjectManager::updateViewMatrix(glm::mat4 vMat) {
    viewMatrix = vMat;
    lightManager.updateViewMatrix(vMat);
}

void ObjectManager::updateProjectionMatrix(glm::mat4 pMat) {
    projectionMatrix = pMat;
}

void ObjectManager::updateSkyBoxesModelMatrix(glm::mat4 new_mMat) {
    mMat_skyBox = new_mMat;
}

void ObjectManager::addlight(typeLight tipo, int largura, int altura) {
    lightManager.addLight(tipo, largura, altura);
    lightManager.installLights(renderingProgram.programID);
}

void ObjectManager::passOne()
{
    glUseProgram(renderingProgramShadow);

    glDrawBuffer(GL_NONE);//desativa cor
    glEnable(GL_DEPTH_TEST);//ativa teste de profundidade para criacao do shadowBuffer
    glDepthFunc(GL_LEQUAL);// less or equal

    //glClear(GL_DEPTH_BUFFER_BIT); // reseta o buffer
    glEnable(GL_CULL_FACE); // permite desenhar apenas em um dos lados
    glFrontFace(GL_CCW); // define a orientacao de desenho para anti-horario
    

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(0.5f, 1.0f);// meu
    //glPolygonOffset(4.0f, 8.0f); // chat

    
    for (int i = 0; i < lightManager.numLights; i++) {
        lightManager.bindShadowBuffers(i);

        GLuint sLoc = glGetUniformLocation(renderingProgramShadow, "shadowMVP");

        glClear(GL_DEPTH_BUFFER_BIT); // reseta o buffer para cada luz
        for (const auto& obj : objects) {
            glBindVertexArray(obj.VAO);

            shadowMVP1 = lightManager.lightSources[i].projectionMatrix * lightManager.lightSources[i].viewMatrix * obj.mMat; //ok
            glUniformMatrix4fv(sLoc, 1, GL_FALSE, glm::value_ptr(shadowMVP1));

            if (obj.numVertices > 0) { glDrawArrays(GL_TRIANGLES, 0, obj.numVertices); }
            else { glDrawElements(GL_TRIANGLES, obj.numIndices, GL_UNSIGNED_INT, 0); }
        }
    }

    glDisable(GL_POLYGON_OFFSET_FILL);//remove o offset

    // restore the default display buffer, and re-enable drawing
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // ou seja, desassocia o framebuffer

    glBindVertexArray(0); // desassocia qualquer VAO restante
}

void ObjectManager::passTwo() {

    if (renderingProgram.programID == 0) {
        cout << "RenderingProgram nao selecionado" << endl;
        return; // Sai se o shader não foi encontrado
    }

    glUseProgram(cubeMapShaderProgram);
    glClear(GL_DEPTH_BUFFER_BIT);  // Importante: limpa a profundidade antes da Skybox

    GLuint CShader_vMat = glGetUniformLocation(cubeMapShaderProgram, "v_matrix");
    GLuint projLoc = glGetUniformLocation(cubeMapShaderProgram, "proj_matrix");
    // set up vertices buffer for cube (buffer for texture coordinates not necessary
    // UAUUUUUU

    glDrawBuffer(GL_FRONT); // re-enables drawing colors

    for (const auto& skybox : skyBoxes) { //pensei no loop para uma possivel transição um dia
        glBindVertexArray(skybox.VAO);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniformMatrix4fv(CShader_vMat, 1, GL_FALSE, glm::value_ptr(viewMatrix));

        // make the cube map the active texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.texture);
        //glUniform1i(isSkyBoxLoc, true);

        // activate the skybox texture
        ////////////////////////////////glActiveTexture(GL_TEXTURE1); descomentar para usar skybox de path
        ////////////////////////////////glBindTexture(GL_TEXTURE_2D, skybox.texture);

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW); // cube has CW winding order, but we are viewing its interior
        glDisable(GL_DEPTH_TEST);
        //glDepthFunc(GL_ALWAYS);
        glDrawArrays(GL_TRIANGLES, 0, 36); // draw the skybox without depth testing
        glEnable(GL_DEPTH_TEST);
        //glDepthFunc(GL_LEQUAL); // Restaura para os objetos normais

    }

    glUseProgram(renderingProgram.programID);
    // essa atualizacao de Locs é NECESSÀRIA
    renderingProgram.mvLoc = glGetUniformLocation(renderingProgram.programID, "mv_matrix");
    renderingProgram.projLoc = glGetUniformLocation(renderingProgram.programID, "proj_matrix");
    renderingProgram.nLoc = glGetUniformLocation(renderingProgram.programID, "norm_matrix");
    //renderingProgram.sLoc = glGetUniformLocation(renderingProgram.programID, "shadowMVP2");
    renderingProgram.isLightLoc = glGetUniformLocation(renderingProgram.programID, "IsLight");

    GLuint mAmbLoc = glGetUniformLocation(renderingProgram.programID, "material.ambient");
    GLuint mDiffLoc = glGetUniformLocation(renderingProgram.programID, "material.diffuse");
    GLuint mSpecLoc = glGetUniformLocation(renderingProgram.programID, "material.specular");
    GLuint mShiLoc = glGetUniformLocation(renderingProgram.programID, "material.shininess");
    
    lightManager.applyLights(); // ja estou enviando pos, dir, type e cores aqui

    glUniformMatrix4fv(renderingProgram.projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    glDrawBuffer(GL_FRONT); // re-enables drawing colors

    // ativa o canal 4
    glEnable(GL_BLEND); // permite mistura, ou seja, ver através?
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // ativa canal alpha

    //glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    //glDisable(GL_CULL_FACE); //permite a visualização da face em qualquer lado
    glFrontFace(GL_CCW); //CCW=Counter-Clockwise, CW=Clockwise=> altera o que é considerado face frontal
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);//nao sei se precisa

    //GLuint isSkyBoxLoc = glGetUniformLocation(renderingProgram.programID, "IsSkyBox");

    //antigo loop da skybox ficava aqui!

    //glUniform1i(isSkyBoxLoc, false);

    for (const auto& obj : objects) {
        glBindVertexArray(obj.VAO);

        // Atualiza a matriz de modelo

        mvMat = viewMatrix * obj.mMat;

        // build the inverse-transpose of the MV matrix, for transforming normal vectors
        invTrMat = glm::transpose(glm::inverse(mvMat));

        glm::mat4 b = {
        0.5f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f
        };

        for (int i = 0; i < lightManager.numLights; i++) {
            shadowMVP2 = b * lightManager.lightSources[i].projectionMatrix * lightManager.lightSources[i].viewMatrix * obj.mMat;

            // Envie a matriz da luz para o shader (como array de matrizes, se necessário)
            glUniformMatrix4fv(lightManager.lightSources[i].shadowMVP2Loc, 1, GL_FALSE, glm::value_ptr(shadowMVP2));

            //associa as texturas geradas pelo shadowBuffers
            //uniformName = "shadowTex[" + std::to_string(i) + "]";
            //GLuint loc = glGetUniformLocation(renderingProgram.programID, uniformName.c_str());
            //glUniform1i(loc, i); // "shadowTex[i]" = texture unit i

            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, lightManager.lightSources[i].shadowTexture);

        }

        GLuint numLightLoc = glGetUniformLocation(renderingProgram.programID, "numLights");
        glUniform1i(numLightLoc, lightManager.numLights);

        glUniformMatrix4fv(renderingProgram.mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
        glUniformMatrix4fv(renderingProgram.nLoc, 1, GL_FALSE, glm::value_ptr(invTrMat));
        
        if (obj.isLight) glUniform1i(renderingProgram.isLightLoc, 1);
        else glUniform1i(renderingProgram.isLightLoc, 0);

        //glActiveTexture(0);//evitando desenho na esferinha
        // Configura a textura, se existir
        //if (obj.texture) {
            //glActiveTexture(GL_TEXTURE0);
            //glBindTexture(GL_TEXTURE_2D, obj.texture);

            //glUniform1i(glGetUniformLocation(renderingProgram, "texture0"), 0); nao precisa
        //}
        
        glProgramUniform4fv(renderingProgram.programID, mAmbLoc, 1, glm::value_ptr(obj.material.ambient));
        glProgramUniform4fv(renderingProgram.programID, mDiffLoc, 1, glm::value_ptr(obj.material.diffuse));
        glProgramUniform4fv(renderingProgram.programID, mSpecLoc, 1, glm::value_ptr(obj.material.specular));
        glProgramUniform1f(renderingProgram.programID, mShiLoc, obj.material.shininess);

        if (obj.numVertices > 0) glDrawArrays(GL_TRIANGLES, 0, obj.numVertices);

        else glDrawElements(GL_TRIANGLES, obj.numIndices, GL_UNSIGNED_INT, 0);

    }
    glBindVertexArray(0); // desassocia qualquer VAO restante
}