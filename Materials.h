#ifndef MATERIALS_H
#define MATERIALS_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Materials {
	// Ouro
	glm::vec4 goldAmbient();
	glm::vec4 goldDiffuse();
	glm::vec4 goldSpecular();
	float goldShininess();

	// Prata
	glm::vec4 silverAmbient();
	glm::vec4 silverDiffuse();
	glm::vec4 silverSpecular();
	float silverShininess();

	// Bronze
	glm::vec4 bronzeAmbient();
	glm::vec4 bronzeDiffuse();
	glm::vec4 bronzeSpecular();
	float bronzeShininess();

	// Pewter
	glm::vec4 pewterAmbient();
	glm::vec4 pewterDiffuse();
	glm::vec4 pewterSpecular();
	float pewterShininess();

	// Jade
	glm::vec4 jadeAmbient();
	glm::vec4 jadeDiffuse();
	glm::vec4 jadeSpecular();
	float jadeShininess();

	// Pearl
	glm::vec4 pearlAmbient();
	glm::vec4 pearlDiffuse();
	glm::vec4 pearlSpecular();
	float pearlShininess();
}

enum Material {
	FULL,
	GOLD,
	BRONZE,
	SILVER,
	PEWTER,
	JADE,
	PEARL
};

void getMaterial(Material, glm::vec4&, glm::vec4&, glm::vec4&, float&);

#endif // MATERIALS_H
