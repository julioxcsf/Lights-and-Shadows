#include <iostream>
#include <fstream>
#include <sstream>

#include "ImportedModel.h"

using namespace std;

// ------------ Imported Model class
ImportedModel::ImportedModel() : numVertices(0) {
    // Construtor padrão vazio
    cout << "Construtor padrao" << endl;
}

ImportedModel::ImportedModel(const char* filePath) {
	ModelImporter modelImporter = ModelImporter();
	modelImporter.parseOBJ(filePath); // uses modelImporter to get vertex information
	numVertices = modelImporter.getNumVertices();
	std::vector<float> verts = modelImporter.getVertices();
	std::vector<float> tcs = modelImporter.getTextureCoordinates();
	std::vector<float> normals = modelImporter.getNormals();
	for (int i = 0; i < numVertices; i++) {
		vertices.push_back(glm::vec3(verts[i * 3], verts[i * 3 + 1], verts[i * 3 + 2]));
		texCoords.push_back(glm::vec2(tcs[i * 2], tcs[i * 2 + 1]));
		normalVecs.push_back(glm::vec3(normals[i * 3], normals[i * 3 + 1], normals[i * 3 + 2]));
	}
}

int ImportedModel::getNumVertices() { return numVertices; } // accessors
std::vector<glm::vec3> ImportedModel::getVertices() { return vertices; }
std::vector<glm::vec2> ImportedModel::getTextureCoords() { return texCoords; }
std::vector<glm::vec3> ImportedModel::getNormals() { return normalVecs; }


// -------------- Model Importer class
ModelImporter::ModelImporter() {}

void ModelImporter::parseOBJ(const char* filePath) {
    float x, y, z;
    string content;
    ifstream fileStream(filePath, ios::in);

    // Verificar se o arquivo foi aberto com sucesso
    if (!fileStream.is_open()) {
        cout << "Erro ao abrir o arquivo: " << filePath << endl;
        return;
    }

    string line = "";
    while (getline(fileStream, line)) {
        // Ignorar linhas em branco ou comentários
        if (line.empty() || line[0] == '#') {
            continue;
        }

        if (line.substr(0, 2) == "v ") { // vertex position ("v" case)
            stringstream ss(line.substr(2));
            ss >> x; ss >> y; ss >> z; // extract the vertex position values
            vertVals.push_back(x);
            vertVals.push_back(y);
            vertVals.push_back(z);
        }
        else if (line.substr(0, 3) == "vt ") { // texture coordinates ("vt" case)
            stringstream ss(line.substr(3));
            ss >> x; ss >> y; // extract texture coordinate values
            stVals.push_back(x);
            stVals.push_back(y);
        }
        else if (line.substr(0, 3) == "vn ") { // vertex normals ("vn" case)
            stringstream ss(line.substr(3));
            ss >> x; ss >> y; ss >> z; // extract the normal vector values
            normVals.push_back(x);
            normVals.push_back(y);
            normVals.push_back(z);
        }
        else if (line.substr(0, 2) == "f ") { // triangle faces ("f" case)
            string oneCorner;
            stringstream ss(line.substr(2));
            while (ss >> oneCorner) { // extract all face references
                if (oneCorner.empty()) continue;

                vector<string> tokens;
                size_t prev = 0, pos = 0;
                while ((pos = oneCorner.find('/', prev)) != string::npos) {
                    tokens.push_back(oneCorner.substr(prev, pos - prev));
                    prev = pos + 1;
                }
                tokens.push_back(oneCorner.substr(prev));

                int vIndex = 0, vtIndex = 0, vnIndex = 0;
                try {
                    if (!tokens[0].empty()) {
                        vIndex = stoi(tokens[0]);
                    }

                    if (tokens.size() > 1 && !tokens[1].empty()) {
                        vtIndex = stoi(tokens[1]);
                    }

                    if (tokens.size() > 2 && !tokens[2].empty()) {
                        vnIndex = stoi(tokens[2]);
                    }
                }
                catch (const std::exception& e) {
                    cerr << "Erro ao converter índices: " << e.what() << endl;
                    continue;
                }

                int vertRef = (vIndex - 1) * 3;

                if (vertRef + 2 >= vertVals.size()) {
                    cerr << "Índice de vértice fora dos limites ao processar face." << endl;
                    continue;
                }

                triangleVerts.push_back(vertVals[vertRef]);     // X
                triangleVerts.push_back(vertVals[vertRef + 1]); // Y
                triangleVerts.push_back(vertVals[vertRef + 2]); // Z

                // Coordenadas de textura
                if (vtIndex > 0) {
                    int tcRef = (vtIndex - 1) * 2;
                    if (tcRef + 1 >= stVals.size()) {
                        cerr << "Índice de textura fora dos limites ao processar face." << endl;
                        // Definir coordenadas de textura padrão
                        textureCoords.push_back(0.0f);
                        textureCoords.push_back(0.0f);
                    }
                    else {
                        textureCoords.push_back(stVals[tcRef]);     // U
                        textureCoords.push_back(stVals[tcRef + 1]); // V
                    }
                }
                else {
                    // Sem coordenada de textura, definir padrão
                    textureCoords.push_back(0.0f);
                    textureCoords.push_back(0.0f);
                }

                // Normais
                if (vnIndex > 0) {
                    int normRef = (vnIndex - 1) * 3;
                    if (normRef + 2 >= normVals.size()) {
                        cerr << "Índice de normal fora dos limites ao processar face." << endl;
                        // Definir normal padrão
                        normals.push_back(0.0f);
                        normals.push_back(0.0f);
                        normals.push_back(0.0f);
                    }
                    else {
                        normals.push_back(normVals[normRef]);     // NX
                        normals.push_back(normVals[normRef + 1]); // NY
                        normals.push_back(normVals[normRef + 2]); // NZ
                    }
                }
                else {
                    // Sem normal, definir padrão (por exemplo, vetor normal padrão)
                    normals.push_back(0.0f);
                    normals.push_back(0.0f);
                    normals.push_back(1.0f); // Normal padrão apontando para +Z
                }
            }
        }
    }

    fileStream.close();
}


int ModelImporter::getNumVertices() { return (triangleVerts.size() / 3); } // accessors
std::vector<float> ModelImporter::getVertices() { return triangleVerts; }
std::vector<float> ModelImporter::getTextureCoordinates() { return textureCoords; }
std::vector<float> ModelImporter::getNormals() { return normals; }