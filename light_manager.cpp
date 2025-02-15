#include "light_manager.h"
#include <iostream>
#include <string>

using namespace std;

void LightManager::addLight(typeLight tipo, int largura, int altura) { //faltou adicionar e configurar os ShadowBuffers!!!!!!!!!!!!!!!!!!!!!!
	if (numLights >= MAX_LIGHTS) return;

	Light new_light;
	new_light.type = tipo;

	// create the custom frame buffer
	glGenFramebuffers(1, &new_light.shadowBuffer);
	// create the shadow texture and configure it to hold depth information.
	// these steps are similar to those in Program 5.2
	glGenTextures(1, &new_light.shadowTexture);
	glBindTexture(GL_TEXTURE_2D, new_light.shadowTexture);
	//int shadowMapResolution = 2048; // Teste 1024, 2048, 4096 e veja o impacto
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, shadowMapResolution, shadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, largura, altura, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	// corrige defeitos de sombra no "mapa" ou chao
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//configure a cor de borda da textura
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 }; //acho que nao está sendo aplicado
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// fim da correção


	cout << "Nova luz adicionada. ID:" << numLights << endl;

	lightSources[numLights] = new_light;
	numLights++;
}

void LightManager::bindShadowBuffers(int ID) {
	if (!verificarID(ID,"bindShadowBuffers")) return;

	// make the custom frame buffer current, and associate it with the shadow texture
	glBindFramebuffer(GL_FRAMEBUFFER, lightSources[ID].shadowBuffer);//linka o shadowBuffer 3D com....
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lightSources[ID].shadowTexture, 0); // a textura2D resultante

	glClear(GL_DEPTH_BUFFER_BIT);
	// disable drawing colors, but enable the depth computation
	//glDrawBuffer(GL_NONE);
	//glEnable(GL_DEPTH_TEST);
}

bool LightManager::verificarID(int ID, string FuncName) {
	if (ID >= numLights) {
		string textoErro = "ID de luz informado nao existe. (num: " + std::to_string(ID) + ")";
		string origem = "LightManager::verificarID[" + FuncName + "]";
		printError(textoErro.c_str(), origem.c_str());
		return false;
	}
	return true;
}

void LightManager::uptadeLightPhisicalProperties(int ID, int sumLightProperties, glm::vec3 pos, glm::vec3 dir ) {
	if (!verificarID(ID, "uptadeLightPhisicalProperties")) return;

	if (sumLightProperties > 3) {
		string textoErro = "Argumento incorreto. Use POSITION ou DIRECTION.";
		printError(textoErro.c_str(), "LightManager::uptadeLightPhisicalProperties");
	}

	if (sumLightProperties & POSITION) {
		//lightSources[ID].position = glm::vec3(vMat * glm::vec4(pos, 1.0));
		lightSources[ID].position = pos; // sem vMat
	}

	if (sumLightProperties & DIRECTION) {
		//lightSources[ID].direction = glm::normalize(dir);
		lightSources[ID].direction = dir;
	}

	updateLightViewMatrix(ID);
	if (lightSources[ID].type == PONTUAL) 
		lightSources[ID].position = glm::vec3(vMat * glm::vec4(lightSources[ID].position, 1.0));
	else
		lightSources[ID].direction = normalize(glm::vec3(vMat * glm::vec4(lightSources[ID].direction, 1.0)));

}

void LightManager::uptadeLightPhisicalProperties(int ID, int sumLightProperties, glm::vec3 pos_or_dir) {
	if (!verificarID(ID, "uptadeLightPhisicalProperties")) return;

	if (sumLightProperties > 3) {
		string textoErro = "Argumento incorreto. Use POSITION ou DIRECTION.";
		printError(textoErro.c_str(), "LightManager::uptadeLightPhisicalProperties");
	}

	if (sumLightProperties & POSITION) {
		//lightSources[ID].position = glm::vec3(vMat * glm::vec4(pos, 1.0));
		lightSources[ID].position = pos_or_dir; // sem vMat
	}

	if (sumLightProperties & DIRECTION) {
		//lightSources[ID].direction = glm::normalize(dir);
		lightSources[ID].direction = pos_or_dir;
	}

	updateLightViewMatrix(ID);
}

void LightManager::updateLightEyeSpace(int ID) {
	if (lightSources[ID].type == PONTUAL)
		lightSources[ID].position_eyeSpace = glm::vec3(vMat * glm::vec4(lightSources[ID].position, 1.0));
	else
		lightSources[ID].direction_eyeSpace = normalize(glm::mat3(vMat) * glm::vec3(lightSources[ID].direction));
}

void LightManager::updateViewMatrix(glm::mat4 new_vMat) {
	vMat = new_vMat; 
	for (int i = 0; i < numLights; i++) {
		updateLightEyeSpace(i);
	}
}

void LightManager::uptadeLightColorProperties(int ID, int sumLightProperties, glm::vec4 color1) {
	if (!verificarID(ID, "uptadeLightColorProperties")) return;
	int sum = sumLightProperties;
	
	if (sum <= 3 || sum > 28) {
		string textoErro = "Argumento incorreto. Use AMBIENT_COLOR, DIFFUSE_COLOR ou SPECULAR_COLOR.";
		printError(textoErro.c_str(), "LightManager::uptadeLightColorProperties");
	}
	if (sum & AMBIENT_COLOR) {
		lightSources[ID].Ambient = color1;
		sum -= AMBIENT_COLOR;
		if (!sum) return;
	}
	if (sum & DIFFUSE_COLOR) {
		lightSources[ID].Diffuse = color1;
		sum -= DIFFUSE_COLOR;
		if (!sum) return;
	}
	if (sum & SPECULAR_COLOR) {
		lightSources[ID].Specular = color1;
		sum -= SPECULAR_COLOR;
		if (!sum) return;
	}
}

void LightManager::uptadeLightColorProperties(int ID, int sumLightProperties, glm::vec4 color1, glm::vec4 color2) {
	if (!verificarID(ID, "uptadeLightColorProperties")) return;
	int sum = sumLightProperties;

	if (sum <= 7 || sum > 28) {
		string textoErro = "Argumento incorreto. Use AMBIENT_COLOR, DIFFUSE_COLOR ou SPECULAR_COLOR.";
		printError(textoErro.c_str(), "LightManager::uptadeLightColorProperties");
	}
	if (sum & AMBIENT_COLOR) {
		lightSources[ID].Ambient = color1;
		sum -= AMBIENT_COLOR;
		if (!sum) return;
	}
	if (sum & DIFFUSE_COLOR) {
		lightSources[ID].Diffuse = color2;
		sum -= DIFFUSE_COLOR;
		if (!sum) return;
	}
	if (sum & SPECULAR_COLOR) {
		lightSources[ID].Specular = color2;
		sum -= SPECULAR_COLOR;
		if (!sum) return;
	}
}

void LightManager::uptadeLightColorProperties(int ID, int sumLightProperties, glm::vec4 color1, glm::vec4 color2, glm::vec4 color3) {
	if (!verificarID(ID, "uptadeLightColorProperties")) return;

	if (sumLightProperties != 28) {
		string textoErro = "Argumento incorreto. Use (AMBIENT_COLOR | DIFFUSE_COLOR | SPECULAR_COLOR).";
		printError(textoErro.c_str(), "LightManager::uptadeLightColorProperties");
	}
	if (sumLightProperties & AMBIENT_COLOR) {
		lightSources[ID].Ambient = color1;
	}
	if (sumLightProperties & DIFFUSE_COLOR) {
		lightSources[ID].Diffuse = color2;
	}
	if (sumLightProperties & SPECULAR_COLOR) {
		lightSources[ID].Specular = color3;
	}
}

void LightManager::uptadeLightProjectionMatrixProperties(int ID, int sumLightProperties, float value1, float value2) {
	if (!verificarID(ID, "uptadeLightMatrixProperties")) return;

	if (sumLightProperties <= 31) {
		string textoErro = "Argumento incorreto. Use FOV_ASPECT, NEAR_FAR_PLANE ou ORTHOSIZE).";
		printError(textoErro.c_str(), "LightManager::uptadeLightMatrixProperties");
	}

	if (sumLightProperties & FOV_ASPECT) {
		lightSources[ID].fov = value1;
		lightSources[ID].aspect = value2;
	}
	if (sumLightProperties & NEAR_FAR_PLANE) {
		lightSources[ID].nearPlane = value1;
		lightSources[ID].farPlane = value2;
	}
	if (sumLightProperties & ORTHOSIZE) {
		lightSources[ID].orthonSize = value1;
	}

	updateLightProjectionMatrix(ID);//perigoso
}

void LightManager::updateLightViewMatrix(int ID) {
	//if (!verificarID) return; não precisa pois a chamada é feita dentro do metodo que ja avaliou o ID
	if (lightSources[ID].type == PONTUAL) {
		lightSources[ID].viewMatrix = glm::lookAt(
			lightSources[ID].position,
			glm::vec3(0.0f),//lightSources[ID].direction,			// Olha na direção da luz
			glm::vec3(0.0f, 1.0f, 0.0f));							// Up vector
			
	}

	else { // DIRECIONAL
		lightSources[ID].viewMatrix = glm::lookAt(					//sem interferencia da vMat!
			lightSources[ID].position,								// Posição da câmera "da luz"
			lightSources[ID].position + lightSources[ID].direction, // Onde a luz está "olhando"
			glm::vec3(0.0f, 1.0f, 0.0f) );							// Up vector
	}
}

void LightManager::updateLightProjectionMatrix(int ID) {
	//if (!verificarID) return; não precisa pois a chamada é feita dentro do metodo que ja avaliou o ID
	if (lightSources[ID].type == PONTUAL) {
		lightSources[ID].projectionMatrix = glm::perspective(glm::radians(lightSources[ID].fov),
			lightSources[ID].aspect, lightSources[ID].nearPlane, lightSources[ID].farPlane);
	}

	else { // DIRECIONAL
		lightSources[ID].projectionMatrix = glm::ortho(
			-lightSources[ID].orthonSize, lightSources[ID].orthonSize,  // Esquerda, Direita
			-lightSources[ID].orthonSize, lightSources[ID].orthonSize,  // Inferior, Superior
			lightSources[ID].nearPlane, lightSources[ID].farPlane           // Near, Far
		);
	}
}

void LightManager::installLights(GLuint renderingProgram) { 
	// eu sei que buscar e escrever nos Locs durante o desenho funcionam
	// quero testar pegar uma vez e escrever todas as vezes quando desenhar
	globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
	posNumLightsLoc = glGetUniformLocation(renderingProgram, "numLights");

	for (int i = 0; i < numLights; i++) {
		string lightInd = "lights[" + to_string(i) + "]";

		// colors properties
		lightSources[i].ambLoc = glGetUniformLocation(renderingProgram, (lightInd + ".ambient").c_str());
		lightSources[i].diffLoc = glGetUniformLocation(renderingProgram, (lightInd + ".diffuse").c_str());
		lightSources[i].specLoc = glGetUniformLocation(renderingProgram, (lightInd + ".specular").c_str());

		// physical properties
		lightSources[i].posLoc = glGetUniformLocation(renderingProgram, (lightInd + ".position").c_str());
		lightSources[i].dirLoc = glGetUniformLocation(renderingProgram, (lightInd + ".direction").c_str());
		lightSources[i].typeLoc = glGetUniformLocation(renderingProgram, (lightInd + ".type").c_str());

		lightSources[i].shadowMVP2Loc = glGetUniformLocation(renderingProgram, (lightInd + ".shadowMVP2").c_str());
	}

	renderingProgramUsed = renderingProgram;
}

void LightManager::applyLights() {
	glProgramUniform4fv(renderingProgramUsed, globalAmbLoc, 1, glm::value_ptr(globalAmbientColor));
	glProgramUniform1i(renderingProgramUsed, posNumLightsLoc, numLights);

	for (int i = 0; i < numLights; i++) {
		glProgramUniform4fv(renderingProgramUsed, lightSources[i].ambLoc, 1, glm::value_ptr(lightSources[i].Ambient));
		glProgramUniform4fv(renderingProgramUsed, lightSources[i].diffLoc, 1, glm::value_ptr(lightSources[i].Diffuse));
		glProgramUniform4fv(renderingProgramUsed, lightSources[i].specLoc, 1, glm::value_ptr(lightSources[i].Specular));

		glProgramUniform3fv(renderingProgramUsed, lightSources[i].posLoc, 1, glm::value_ptr(lightSources[i].position_eyeSpace));
		glProgramUniform3fv(renderingProgramUsed, lightSources[i].dirLoc, 1, glm::value_ptr(lightSources[i].direction_eyeSpace));
		glProgramUniform1i(renderingProgramUsed, lightSources[i].typeLoc, lightSources[i].type);
		
		//falta o MVP2
	}
}