#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "3D_objs_manager.h"
#include "OpenGL_Utils.h"

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#define numVAOs         4
#define numVBOs_Esfera  3
#define numVBOs_Torus   4
#define numVBOs_Objs    3

constexpr auto CONSOLE_DEBUG = true;

#define ALTURA_TELA     1063
#define LARGURA_TELA    1921

using namespace std;

ObjectManager objManager; // Gerenciador de objetos

GLuint renderingProgram, pass1_renderingProgram;
glm::mat4 pMat, vMat, mMat_skybox;

GLuint imageTexture;
int contador_imagens_salvas = 0;
vector<string> caminhos_validos_texturas;

glm::vec3 cameraPosition(0.0f, 3.0f, -2.0f);  // Posição inicial da câmera
glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);   // Direção inicial da câmera
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);       // Vetor "up" da câmera
float cameraSpeed = 0.05f;                  // Velocidade de movimento da câmera
bool cameraEnabled = true; // Inicialmente com controle de câmera ativado

int style_idx = 1;
int texture_idx = 0;
int obj_idx = 0;

// calcular FPS
double previousTime = 0.0;
int frameCount = 0;
float fps = 0.0f;

glm::vec3 lightPosV, lightPosV2; // light position as Vector3f, in both model and view space
float newlightPos[3]; // light position as float array
float newlightPos2[3]; // light position as float array
float direction[3];

// initial light location
glm::vec3 lightPos = glm::vec3(10.0f, 10.0f, 10.0f);
glm::vec3 lightPos2 = glm::vec3(10.0f, 10.0f, 0.0f);
glm::vec3 lightDirection = glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f));


// white light properties
float globalAmbient[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
float lightAmbient[4] = { 0.01f, 0.01f, 0.01f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

float globalAmbient2[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
float lightAmbient2[4] = { 0.05f, 0.0f, 0.0f, 1.0f };
float lightDiffuse2[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
float lightSpecular2[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

int width, height;
float aspect;
float orthosize = 1.0f;

bool JanelaLuz, JanelaDebug, JanelaMaterial = false;
bool rotationEnabled = false; // Controle para ligar/desligar a rotação
float rotationSpeed = 1.0f;   // Velocidade de rotação (ajuste conforme necessário)
float rotationAngle = 0.0f;   // Ângulo de rotação acumulado

char imagePath[256] = "earth.jpg"; // caminho incial da textura

// Variáveis para a notificação
std::string notificationText = "";
ImVec4 notificationColor = ImVec4(0, 1, 0, 1); // Verde por padrão
float notificationTime = 0.0f; // Tempo restante para exibição (em segundos)

// Variáveis para controlar a rotação do objeto
float objectYaw = 0.0f, objectPitch = 0.0f;
double lastX = LARGURA_TELA / 2.0f, lastY = ALTURA_TELA / 2.0f;
bool rightButtonPressed = false;

//prototipos
//void installLights(glm::mat4 vMatrix);
void showNotification(const std::string& text, const ImVec4& color, float duration);

void createConsole() {
    AllocConsole(); // Cria uma nova janela de console

    // Redireciona a saída padrão (stdout) para a nova janela de console
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
}

void processInput(GLFWwindow* window) {
    static bool escPressed = false;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        if (!escPressed) {
            cameraEnabled = !cameraEnabled; // Alterna o estado da câmera
            glfwSetInputMode(window, GLFW_CURSOR, cameraEnabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);

            // Atualiza o centro da janela ao ativar a câmera
            if (cameraEnabled) {
                int winWidth, winHeight;
                glfwGetWindowSize(window, &winWidth, &winHeight);
                lastX = winWidth / 2.0f;
                lastY = winHeight / 2.0f;
                glfwSetCursorPos(window, lastX, lastY); // Centraliza o cursor
            }
            escPressed = true;
        }
    }
    else {
        escPressed = false;
    }

    if (!cameraEnabled) return; // Não processa entrada de movimento se a câmera estiver desativada

    float adjustedSpeed = cameraSpeed * 0.8f;  // Multiplicador para ajustar sensibilidade

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosition += adjustedSpeed * cameraFront;  // Avançar

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosition -= adjustedSpeed * cameraFront;  // Recuar

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * adjustedSpeed;  // Esquerda

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * adjustedSpeed;  // Direita
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    float zoomSensitivity = 0.03f; // Sensibilidade do zoom
    cameraPosition.z -= (float)yoffset * zoomSensitivity;

    // Limita o valor de zoom para evitar que a câmera chegue muito perto ou muito longe
    if (cameraPosition.z < 0.0001f) cameraPosition.z = 0.0001f;  // Limite próximo
    if (cameraPosition.z > 100.0f) cameraPosition.z = 100.0f; // Limite distante
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            rightButtonPressed = true;
            glfwGetCursorPos(window, &lastX, &lastY);
        }
        else if (action == GLFW_RELEASE) {
            rightButtonPressed = false;
        }
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static bool firstMouse = true;
    static float yaw = -90.0f;  // Yaw inicial
    static float pitch = 0.0f;  // Pitch inicial

    static double lastX = LARGURA_TELA / 2.0f;
    static double lastY = ALTURA_TELA / 2.0f;

    if (!cameraEnabled) return; // Não processa movimentos se a câmera estiver desativada

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // Calcular deslocamentos (valores relativos)
    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;  // Inverter para tornar mais intuitivo
    lastX = xpos;
    lastY = ypos;

    // Ajustar sensibilidade do mouse
    float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    // Atualizar yaw e pitch
    yaw += xOffset;
    pitch += yOffset;

    // Limitar pitch para evitar "girar de cabeça para baixo"
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Atualizar direção da câmera
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void init(GLFWwindow* window) {
    glfwGetFramebufferSize(window, &width, &height);
    aspect = (float)width / (float)height;
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472 radians = 60 degrees
    objManager.updateProjectionMatrix(pMat);

    // Configuração inicial
    objManager.init();
    renderingProgram = objManager.selectShaderProgram(ADS_PHONG_SHADER);

    // Definir os valores iniciais da luz
    glm::vec3 lightDirection(0.0f, 0.0f, 0.0f);
    float lightFOV = 60.0f;      // Campo de visão
    float nearPlane = 0.1f;      // Plano próximo
    float farPlane = 50.0f;     // Plano distante

    // Adiciona objetos para teste
    objManager.add(SKYBOX, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
    objManager.updateSkyBoxTexture(0,"Texture/NasaCubemap8k", true);// path_is_folder = true;
    
    objManager.add(FLOOR, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f), glm::vec3(10.0f,1.0f, 10.0f));
    objManager.add(SPHERE, lightPos, glm::vec3(0.0f), glm::vec3(0.2f));

    objManager.add(SPHERE, lightPos2, glm::vec3(0.0f), glm::vec3(0.2f));
    
    objManager.addlight(PONTUAL, width, height);
    objManager.lightManager.uptadeLightProjectionMatrixProperties(0, FOV_ASPECT, lightFOV, aspect); // (0 - 2)
    objManager.lightManager.uptadeLightProjectionMatrixProperties(0, NEAR_FAR_PLANE, nearPlane, farPlane); // (0 - 3)
    
    objManager.addlight(DIRECIONAL, width, height);
    orthosize = 10.0f;
    objManager.lightManager.uptadeLightProjectionMatrixProperties(1, ORTHOSIZE, orthosize); // (1 - 2)
    objManager.lightManager.uptadeLightProjectionMatrixProperties(1, NEAR_FAR_PLANE, nearPlane, farPlane); // (1 - 3)
    glm::vec3 lightDir = glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f));
    objManager.lightManager.uptadeLightPhisicalProperties(1, DIRECTION, lightDir);
    objManager.add(SPHERE, -lightDir * glm::vec3(50.0f), glm::vec3(0.0f), glm::vec3(0.2f));
    objManager.updateObjectTransform(3, -lightDir * glm::vec3(50.0f), glm::vec3(0.0f), glm::vec3(0.2f), true); // esfera da luz 0

    objManager.addlight(PONTUAL, width, height);
    objManager.lightManager.uptadeLightProjectionMatrixProperties(2, FOV_ASPECT, lightFOV, aspect); // (0 - 2)
    objManager.lightManager.uptadeLightProjectionMatrixProperties(2, NEAR_FAR_PLANE, nearPlane, farPlane); // (0 - 3)

    

    //adicao de objetos extras
    
    objManager.add(SPHERE, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
    //objManager.add(TORUS, glm::vec3(2.0f, 1.0f, 0.0f), glm::vec3(45.0f, 0.0f, 0.0f), glm::vec3(0.5f));
    
}

void window_reshape_callback(GLFWwindow* window, int newWidth, int newHeight) {
    if (newHeight == 0) newHeight = 1; // Prevenir divisão por zero
    aspect = (float)newWidth / (float)newHeight; // new width&height provided by the callback
    glViewport(0, 0, newWidth, newHeight); // sets screen region associated with framebuffer
    pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

void display(GLFWwindow* window, double currentTime) {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Atualiza a visão da câmera
    vMat = glm::lookAt(
        cameraPosition,          // Posição da câmera
        cameraPosition + cameraFront,  // Alvo da câmera (posição + direção)
        cameraUp                 // Vetor "up"
    );

    objManager.updateViewMatrix(vMat);

    //mMat_skybox = glm::translate(glm::mat4(1.0f), glm::vec3(cameraPosition.x, cameraPosition.y, cameraPosition.z));
    //objManager.updateSkyBoxesModelMatrix(mMat_skybox);

    
    objManager.lightManager.uptadeLightPhisicalProperties(0, POSITION, lightPos);//(0 - 1)
    objManager.lightManager.uptadeLightPhisicalProperties(2, POSITION, lightPos2);//(2 - 1)

    objManager.updateObjectTransform(1, lightPos, glm::vec3(0.0f), glm::vec3(0.2f), true); // esfera da luz 0
    objManager.updateObjectTransform(2, lightPos2, glm::vec3(0.0f), glm::vec3(0.2f), true); // esfera da luz 0
    
    //objManager.updateObjectTransform(1, glm::vec3(0.0f, 30.0f, 0.0f), glm::vec3(0.0f), glm::vec3(0.2f), true);

    objManager.passOne();
    objManager.passTwo();
}

void InterfacePrincipal() {
    // Criar uma janela ImGui
    

    ImGui::Begin("Controle de Cena");
    if (ImGui::Button("Adicionar Esfera")) {
        objManager.add(SPHERE, glm::vec3(rand() % 5, 0.0f, 0.0f));
    }
    if (ImGui::Button("Adicionar Torus")) {
        objManager.add(TORUS, glm::vec3(rand() % 5, 0.0f, 0.0f));
    }
    if (ImGui::Button("Adicionar Plano")) {
        objManager.add(FLOOR, glm::vec3(rand() % 5, 0.0f, 0.0f));
    }
    if (ImGui::Button("Adicionar Cubo")) {
        objManager.add(CUBE, glm::vec3(rand() % 5, 0.0f, 0.0f));
    }
    if (ImGui::Button("Adicionar SKYBOX - teste")) {
        objManager.add(SKYBOX, glm::vec3(rand() % 5, 0.0f, 0.0f));
    }
    if (ImGui::Button("Adicionar Piramide")) {
        objManager.add(PYRAMID, glm::vec3(rand() % 5, 0.0f, 0.0f));
    }
    if (ImGui::Button("Adicionar Dragao")) {
        objManager.add(LOAD_MODEL_3D, glm::vec3(rand() % 5, 0.0f, 0.0f));
    }
    if (ImGui::Button("Adicionar golfinho")) {
        objManager.add(LOAD_MODEL_3D, glm::vec3(rand() % 5, 0.0f, 0.0f), glm::vec3(0), glm::vec3(1),
            "10014_dolphin_v1/10014_dolphin_v2_max2011_it2.obj");
    }
    if (ImGui::Button("Adicionar Cubo carregado")) {
        objManager.add(LOAD_MODEL_3D, glm::vec3(rand() % 5, 0.0f, 0.0f), glm::vec3(0), glm::vec3(1), "cube.obj");
    }

    ImGui::Separator();

    const char shaderEscolhido[] =
        "ADS Gouraud\0ADS Phong\0ADS Blinn-Phong\0";
    if (ImGui::Combo("Escolha o tipo de iluminacao", &style_idx, shaderEscolhido))
    {
        switch (style_idx)
        {
        case 0: renderingProgram = objManager.selectShaderProgram(ADS_GOURAUD_SHADER); break;
        case 1: renderingProgram = objManager.selectShaderProgram(ADS_PHONG_SHADER); break;
        case 2: renderingProgram = objManager.selectShaderProgram(ADS_BLINN_PHONG_SHADER); break;
        }
    }
    ImGui::Separator();

    // Definir uma largura específica para o combo box
    ImGui::SetNextItemWidth(60.0f); // Ajuste este valor conforme necessário

    // Botão que alterna a visibilidade das outras interfaces
    if (ImGui::Button("Mostrar/Ocultar janela de config. de luz")) {
        JanelaLuz = !JanelaLuz;
    }

    if (ImGui::Button("Mostrar/Ocultar janela de config. de material")) {
        JanelaMaterial = !JanelaMaterial;
    }

    if (ImGui::Button("Mostrar/Ocultar janela de config. de debug")) {
        JanelaDebug = !JanelaDebug;
    }

    ImGui::Separator();

    // Campo de entrada de texto para o caminho da imagem
    ImGui::InputText("Caminho da Imagem", imagePath, IM_ARRAYSIZE(imagePath));
    if (ImGui::IsItemDeactivatedAfterEdit()) {
        cout << "Antigo ID_texture: " << imageTexture << ", Adress: " << &imageTexture << endl;
        GLuint novaTextura = loadTexture(imagePath);
        std::string parte = imagePath;
        if (novaTextura != 0) {
            glDeleteTextures(1, &imageTexture);
            imageTexture = novaTextura;
            cout << "Novo ID_texture: " << imageTexture << ", Adress: " << &imageTexture << endl;
            std::string texto = "Imagem carregada com sucesso: " + parte;
            showNotification(texto, ImVec4(0, 1, 0, 1), 5.0f);
            contador_imagens_salvas++;
            caminhos_validos_texturas.push_back(imagePath); //eu nao verifico se a textura ja foi salva...
        }
        else {
            std::cerr << "Falha ao carregar a imagem: " << imagePath << std::endl;
            std::string texto = "Falha ao carregar a imagem: " + parte;
            showNotification(texto, ImVec4(1, 0, 0, 1), 5.0f);
        }
    }

    if (contador_imagens_salvas > 1) {
        string textoOpcoes_Combo = "";
        for (unsigned i = 0; i < caminhos_validos_texturas.size(); i++) {
            textoOpcoes_Combo += caminhos_validos_texturas[i] + '\0';
        }

        if (ImGui::Combo("Texturas usadas", &texture_idx, textoOpcoes_Combo.c_str()))
        {
            //textura escolhida = caminhos_validos_texturas[style_idx];
            GLuint novaTextura = loadTexture(caminhos_validos_texturas[texture_idx].c_str());
            glDeleteTextures(1, &imageTexture);
            imageTexture = novaTextura;
            cout << "Novo ID_texture: " << imageTexture << ", Adress: " << &imageTexture << endl;
            std::string texto = "Imagem carregada com sucesso: " + caminhos_validos_texturas[texture_idx];
            showNotification(texto, ImVec4(0, 1, 0, 1), 5.0f);
        }
    }

    ImGui::Separator();

    if (ImGui::Button("Resetar Shaders")) {
        objManager.resetShaders();
        renderingProgram = objManager.selectShaderProgram();
    }
    

    ImGui::End();
}

void EditorDeLuz() {
    // Criar uma janela ImGui
    ImGui::Begin("Configuracao de Luz");

    // Luz 0
    ImGui::Text("Luz 0 Pontual");
    ImGui::DragFloat3("Posicao##0", glm::value_ptr(lightPos), 0.1f, -30.0f, 30.0f, "%.3f", ImGuiSliderFlags_None);
    ImGui::ColorEdit4("Global ambiente##0", globalAmbient);
    ImGui::ColorEdit4("Luz ambiente##0", lightAmbient);
    ImGui::ColorEdit4("Luz difusa##0", lightDiffuse);
    ImGui::ColorEdit4("Luz especular##0", lightSpecular);

    glm::vec4 cor = glm::vec4(lightAmbient[0], lightAmbient[1], lightAmbient[2], lightAmbient[3]);
    objManager.lightManager.uptadeLightColorProperties(0, AMBIENT_COLOR, cor);

    cor = glm::vec4(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], lightDiffuse[3]);
    objManager.lightManager.uptadeLightColorProperties(0, DIFFUSE_COLOR, cor);

    cor = glm::vec4(lightSpecular[0], lightSpecular[1], lightSpecular[2], lightSpecular[3]);
    objManager.lightManager.uptadeLightColorProperties(0, SPECULAR_COLOR, cor);

    ImGui::Separator();

    // Luz 1
    ImGui::Text("Luz 1 Direcional");
    ImGui::DragFloat3("Posicao##1", glm::value_ptr(lightPos2), 0.1f, -30.0f, 30.0f, "%.3f", ImGuiSliderFlags_None);
    ImGui::ColorEdit4("Global ambiente##1", globalAmbient2);
    ImGui::ColorEdit4("Luz ambiente##1", lightAmbient2);
    ImGui::ColorEdit4("Luz difusa##1", lightDiffuse2);
    ImGui::ColorEdit4("Luz especular##1", lightSpecular2);

    ImGui::Separator();
    ImGui::DragFloat("OrthonSize##2", &orthosize, 0.1f, -30.0f, 100.0f, "%.3f", ImGuiSliderFlags_None);
    objManager.lightManager.uptadeLightProjectionMatrixProperties(1,ORTHOSIZE, orthosize);

    cor = glm::vec4(lightAmbient2[0], lightAmbient2[1], lightAmbient2[2], lightAmbient2[3]);
    objManager.lightManager.uptadeLightColorProperties(1, AMBIENT_COLOR, cor);

    cor = glm::vec4(lightDiffuse2[0], lightDiffuse2[1], lightDiffuse2[2], lightDiffuse2[3]);
    objManager.lightManager.uptadeLightColorProperties(1, DIFFUSE_COLOR, cor);

    cor = glm::vec4(lightSpecular2[0], lightSpecular2[1], lightSpecular2[2], lightSpecular2[3]);
    objManager.lightManager.uptadeLightColorProperties(1, SPECULAR_COLOR, cor);

    ImGui::End();
}

void InterfaceDebug() {
    ImGui::Begin("Janela de Debug"); // Janela principal de depuração

    // Exibir FPS
    ImGui::Text("FPS: %.2f", fps);

    // Instruções básicas
    ImGui::Text("Use WASD para movimentar a camera.");
    ImGui::Text("Aperte o botao direito do mouse e mova para rotacionar.\n");

    // Exibir status de rotação automática
    ImGui::Text("Rotacao automatica: %s", rotationEnabled ? "Ligada" : "Desligada");
    ImGui::Text("Velocidade de rotacao: %.2f", rotationSpeed);

    // Mostrar ângulo de rotação acumulado
    ImGui::Text("Angulo de rotacao: %.2f graus", rotationAngle);

    // Informações da câmera
    ImGui::Separator();
    ImGui::Text("Dados da Camera:");
    ImGui::Text("Posicao: [%.2f, %.2f, %.2f]", cameraPosition.x, cameraPosition.y, cameraPosition.z);
    ImGui::Text("Direcao (Front): [%.2f, %.2f, %.2f]", cameraFront.x, cameraFront.y, cameraFront.z);
    ImGui::Text("Up Vector: [%.2f, %.2f, %.2f]", cameraUp.x, cameraUp.y, cameraUp.z);

    // Outros dados dinâmicos podem ser adicionados conforme necessário

    ImGui::End(); // Finaliza a janela de depuração
}

void InterfaceObjeto() {
    static int selectedObjID = -1; // ID do objeto selecionado
    static int lastNumObjects = 0; // Número de objetos na última verificação

    ImGui::Begin("Gerenciamento de Objetos");

    // Obtem o número de objetos atual
    int numObjects = objManager.getNumObjects();

    // Verifica se há um novo objeto adicionado
    if (numObjects > lastNumObjects) {
        selectedObjID = numObjects - 1; // Seleciona automaticamente o último objeto
    }
    lastNumObjects = numObjects; // Atualiza o número de objetos conhecido

    // Exibe lista de objetos criados
    ImGui::Text("Objetos Criados:");
    ImGui::Separator();

    for (int i = 0; i < objManager.getNumObjects(); i++) {
        // Nome dos objetos para exibir na lista
        std::string objName = "Obj. " + std::to_string(i+1) + " - " + objManager.getObjtectType(i);
        if (ImGui::Selectable(objName.c_str(), selectedObjID == i)) {
            selectedObjID = i; // Define o objeto selecionado
        }
    }

    ImGui::Separator();

    // Exibe e permite editar a posição do objeto selecionado
    if (selectedObjID >= 0 && selectedObjID < objManager.getNumObjects()) {
        ImGui::Text("Objeto selecionado: %d", selectedObjID+1);

        // Obtenção da referência à posição do objeto
        glm::vec3& pos = objManager.getObjectPos(selectedObjID);
        glm::vec3& rot = objManager.getObjectRotation(selectedObjID);
        glm::vec3& scale = objManager.getObjectScale(selectedObjID);

        // Editor para modificar a posição
        if (ImGui::DragFloat3("Posicao", glm::value_ptr(pos), 0.1f)) {
            // Atualiza a matriz de modelo (mMat) do objeto
            objManager.updateModelMatrix(selectedObjID);
        }
        if (ImGui::DragFloat3("Rotacao", glm::value_ptr(rot), 0.1f)) {
            // Atualiza a matriz de modelo (mMat) do objeto
            objManager.updateModelMatrix(selectedObjID);
        }
        if (ImGui::DragFloat3("Escala", glm::value_ptr(scale), 0.1f)) {
            // Atualiza a matriz de modelo (mMat) do objeto
            objManager.updateModelMatrix(selectedObjID);
        }

        ImGui::Separator();

        const char materialEscolhido[] =
            "GOLD\0BRONZE\0SILVER\0PEWTER\0JADE\0PEARL\0";
        int materialIndex = -1;
        if (ImGui::Combo("Escolha o tipo de iluminacao", &materialIndex, materialEscolhido))
        {
            switch (materialIndex)
            {
            case 0: objManager.updateObjectMaterials(selectedObjID, GOLD); break;
            case 1: objManager.updateObjectMaterials(selectedObjID, BRONZE); break;
            case 2: objManager.updateObjectMaterials(selectedObjID, SILVER); break;
            case 3: objManager.updateObjectMaterials(selectedObjID, PEWTER); break;
            case 4: objManager.updateObjectMaterials(selectedObjID, JADE); break;
            case 5: objManager.updateObjectMaterials(selectedObjID, PEARL); break;
            default: break;
            }
        }

        glm::vec4& matAmb2 = objManager.objects[selectedObjID].material.ambient;
        glm::vec4& matDif2 = objManager.objects[selectedObjID].material.diffuse;
        glm::vec4& matSpe2 = objManager.objects[selectedObjID].material.specular;
        float& matShi2 = objManager.objects[selectedObjID].material.shininess;

        // Converter glm::vec4 para array de floats
        float ambientArr[4] = { matAmb2.x, matAmb2.y, matAmb2.z, matAmb2.w };
        float diffuseArr[4] = { matDif2.x, matDif2.y, matDif2.z, matDif2.w };
        float specularArr[4] = { matSpe2.x, matSpe2.y, matSpe2.z, matSpe2.w };

        // ImGui manipulando os valores
        ImGui::DragFloat4("matAmb", ambientArr, 0.01f, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_None);
        ImGui::DragFloat4("matDif", diffuseArr, 0.01f, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_None);
        ImGui::DragFloat4("matSpe", specularArr, 0.01f, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_None);
        ImGui::DragFloat("matShi", &matShi2, 0.01f, 0.0f, 10000.0f, "%.3f", ImGuiSliderFlags_None);

        // Atualizar os valores no glm::vec4 após manipulação
        matAmb2 = glm::vec4(ambientArr[0], ambientArr[1], ambientArr[2], ambientArr[3]);
        matDif2 = glm::vec4(diffuseArr[0], diffuseArr[1], diffuseArr[2], diffuseArr[3]);
        matSpe2 = glm::vec4(specularArr[0], specularArr[1], specularArr[2], specularArr[3]);

    }
    else {
        ImGui::Text("Nenhum objeto selecionado.");
    }

    ImGui::End();
}

void TESTE_ESCRITA() {
    cout << "Console window created." << endl;

    // Exemplo de mensagens com cores no console
    setConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
    cout << "Mensagem vermelha no console." << endl;

    setConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    cout << "Mensagem verde no console." << endl;

    setConsoleColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    cout << "Mensagem azul no console." << endl;

    setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    cout << "Mensagem branca (intensa) no console." << endl;

    setConsoleColor(BACKGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    cout << "Texto com fundo vermelho e texto ciano no console." << endl;

    setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
}

void showNotification(const std::string& text, const ImVec4& color, float duration) {
    notificationText = text;
    notificationColor = color;
    notificationTime = duration;
}

int main() {
    if (CONSOLE_DEBUG) {
        createConsole();
        //TESTE_ESCRITA();
    }

    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW." << endl;
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(1000, 1000, "Gerador de Objetos com iluminacao e sombra - 21/01/25 - Julio CSF", NULL, NULL);
    
    if (!window) {
        cerr << "Failed to create GLFW window." << endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to initialize GLEW." << endl;
        exit(EXIT_FAILURE);
    }
    glfwSwapInterval(1);//Desativa V-Sync // 1 -> 60 FPS, 2 -> 30 FPS, 0.5 -> Nao pode

    glfwSetWindowSizeCallback(window, window_reshape_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Configuração do ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Configuração do estilo do ImGui
    ImGui::StyleColorsDark();

    // Inicialização dos backends do ImGui
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");

    init(window);

    // Definir os valores iniciais da luz
    glm::vec3 lightPosition(10.0f, 10.0f, 10.0f);
    glm::vec3 lightDirection(0.0f, -1.0f, 0.0f);
    float lightFOV = 60.0f;      // Campo de visão
    float nearPlane = 0.1f;      // Plano próximo
    float farPlane = 1000.0f;     // Plano distante
    

    //glfwSetCursorPosCallback(window, cursor_position_callback);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        processInput(window);  // Processa a entrada do teclado para movimentação

        float deltaTime = ImGui::GetIO().DeltaTime; // Tempo desde o último frame

        // calcular FPS
        double currentTime = glfwGetTime();
        frameCount++;

        // Se passou 1 segundo, calcula o FPS
        if (currentTime - previousTime >= 1.0) {
            fps = frameCount / (currentTime - previousTime);

            // Reseta o contador
            previousTime = currentTime;
            frameCount = 0;
        } // fim calcular FPS
        

        // Inicia o frame do ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        
        InterfacePrincipal();
        InterfaceObjeto();

        if (JanelaLuz) EditorDeLuz();
        //if (JanelaMaterial) EditorDeMaterial();
        if (JanelaDebug) InterfaceDebug();

        // Renderiza a notificação, se houver
        if (notificationTime > 0.0f && !notificationText.empty()) {
            notificationTime -= deltaTime;
            float alpha = notificationTime / 5.0f;
            alpha = (alpha < 0.0f) ? 0.0f : (alpha > 1.0f) ? 1.0f : alpha;
            ImVec4 colorWithAlpha = notificationColor;
            colorWithAlpha.w = alpha;

            ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always);
            ImGui::SetNextWindowBgAlpha(alpha);
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
                ImGuiWindowFlags_NoInputs |
                ImGuiWindowFlags_AlwaysAutoResize;

            if (ImGui::Begin("NotificationWindow", NULL, window_flags)) {
                ImGui::TextColored(colorWithAlpha, "%s", notificationText.c_str());
            }
            ImGui::End();
        }

        display(window, glfwGetTime());

        // Renderização do ImGui
        ImGui::Render();

        // Renderiza a interface do ImGui
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        
    }

    // Limpeza
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    glfwDestroyWindow(window);
    //glDeleteVertexArrays(numVAOs, vao);
    //glDeleteBuffers(numVBOs_Esfera + numVBOs_Torus, vbo);
    //glDeleteTextures(1, &imageTexture);//só estou apagando uma textura
    glfwTerminate();

    return 0;
}