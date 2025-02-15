#include "Materials.h"

namespace Materials {

	// GOLD material - ambient, diffuse, specular, and shininess
    glm::vec4 goldAmbient() { return glm::vec4(0.2473f, 0.1995f, 0.0745f, 1.0f); }
    glm::vec4 goldDiffuse() { return glm::vec4(0.7516f, 0.6065f, 0.2265f, 1.0f); }
    glm::vec4 goldSpecular() { return glm::vec4(0.6283f, 0.5558f, 0.3661f, 1.0f);}
	float goldShininess() { return 51.2f; }

	// SILVER material - ambient, diffuse, specular, and shininess
    glm::vec4 silverAmbient() { return glm::vec4(0.1923f, 0.1923f, 0.1923f, 1.0f); }
    glm::vec4 silverDiffuse() { return glm::vec4(0.5075f, 0.5075f, 0.5075f, 1.0f); }
    glm::vec4 silverSpecular() { return glm::vec4(0.5083f, 0.5083f, 0.5083f, 1.0f); }
	float silverShininess() { return 51.2f; }

	// BRONZE material - ambient, diffuse, specular, and shininess
    glm::vec4 bronzeAmbient() { return glm::vec4(0.2125f, 0.1275f, 0.0540f, 1.0f); }
    glm::vec4 bronzeDiffuse() { return glm::vec4(0.7140f, 0.4284f, 0.1814f, 1.0f); }
    glm::vec4 bronzeSpecular() { return glm::vec4(0.3935f, 0.2719f, 0.1667f, 1.0f); }
	float bronzeShininess() { return 25.6f; }

	// Pewter material - ambient, diffuse, specular, and shininess
    glm::vec4 pewterAmbient() { return glm::vec4(.11f, .06f, .11f, 1.0f); }
    glm::vec4 pewterDiffuse() { return glm::vec4(.43f, .47f, .54f, 1.0f); }
    glm::vec4 pewterSpecular() { return glm::vec4(.33f,.33f, .52f, 1.0f); }
	float pewterShininess() { return 9.85f; }

	// Jade material - ambient, diffuse, specular, and shininess
    glm::vec4 jadeAmbient() { return glm::vec4(0.1350f, 0.2225f, 0.1575f, .95f); }
    glm::vec4 jadeDiffuse() { return glm::vec4(0.5400f, 0.8900f, 0.6300f, .95f); }
    glm::vec4 jadeSpecular() { return glm::vec4(0.3162f, 0.3162f, 0.3162f, .95f); }
	float jadeShininess() { return 12.8f; }

	// Pearl material - ambient, diffuse, specular, and shininess
    glm::vec4 pearlAmbient() { return glm::vec4(0.2500f, 0.2073f, 0.2073f, .922f); }
    glm::vec4 pearlDiffuse() { return glm::vec4(1.0000f, 0.8290f, 0.8290f, .922f); }
    glm::vec4 pearlSpecular() { return glm::vec4(0.2966f, 0.2966f, 0.2966f, .922f); }
	float pearlShininess() { return 11.264f; }

} // namespace Materials

void getMaterial(Material material, glm::vec4& ambientVec, glm::vec4& diffuseVec, glm::vec4& specularVec, float& shininess) {
    if (material == GOLD) {
        ambientVec = glm::make_vec4(Materials::goldAmbient());
        diffuseVec = glm::make_vec4(Materials::goldDiffuse());
        specularVec = glm::make_vec4(Materials::goldSpecular());
        shininess = Materials::goldShininess();
    }
    else if (material == SILVER) {
        ambientVec = glm::make_vec4(Materials::silverAmbient());
        diffuseVec = glm::make_vec4(Materials::silverDiffuse());
        specularVec = glm::make_vec4(Materials::silverSpecular());
        shininess = Materials::silverShininess();
    }
    else if (material == BRONZE) {
        ambientVec = glm::make_vec4(Materials::bronzeAmbient());
        diffuseVec = glm::make_vec4(Materials::bronzeDiffuse());
        specularVec = glm::make_vec4(Materials::bronzeSpecular());
        shininess = Materials::bronzeShininess();
    }
    else if (material == PEWTER) {
        ambientVec = glm::make_vec4(Materials::pewterAmbient());
        diffuseVec = glm::make_vec4(Materials::pewterDiffuse());
        specularVec = glm::make_vec4(Materials::pewterSpecular());
        shininess = Materials::pewterShininess();
    }
    else if (material == JADE) {
        ambientVec = glm::make_vec4(Materials::jadeAmbient());
        diffuseVec = glm::make_vec4(Materials::jadeDiffuse());
        specularVec = glm::make_vec4(Materials::jadeSpecular());
        shininess = Materials::jadeShininess();
    }
    else if (material == PEARL) {
        ambientVec = glm::make_vec4(Materials::pearlAmbient());
        diffuseVec = glm::make_vec4(Materials::pearlDiffuse());
        specularVec = glm::make_vec4(Materials::pearlSpecular());
        shininess = Materials::pearlShininess();
    }
    else {
        // Valores padrão ou tratamento de erro
        ambientVec = glm::vec4(1.0f);
        diffuseVec = glm::vec4(1.0f);
        specularVec = glm::vec4(1.0f);
        shininess = 0.0f;
    }
}
