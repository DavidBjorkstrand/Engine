#pragma once

#include "engine/scene/entity/component/Behaviour.h"

#include <vector>

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

class Scene;
class Mesh;
class ParticleSystem;

struct Particle;
struct RenderCommand;
struct Vertex;

using namespace std;

class ParticleEmitter : public Behaviour
{
	private:
		vector<Vertex> *_vertices;
		vector<GLuint> *_indices;
		Mesh *_particleMesh;
		ParticleSystem *_particleSystem;

		glm::vec2 _spawnArea;
		float _spawnRadius;
		float _spawnRate;
		float _spawnRest;
		float _velocity;
		float _velocityDeviation;
		float _maxLifeTime;
		float *_lifeTime;
		GLboolean _sphereSpawn;
		
	public:
		ParticleEmitter(float particleRadius, glm::vec2 spawnArea, float spawnRate, float mass, 
			float velocity, float velocityDeviation, GLuint maxParticles, float maxLifeTime);
		ParticleEmitter(float particleRadius, float spawnRadius, float spawnRate, float mass,
			float velocity, float velocityDeviation, GLuint maxParticles, float maxLifeTime);
		~ParticleEmitter();
		bool hasActiveParticles();
		ParticleSystem *getParticleSystem();
		RenderCommand getRenderCommand();
		void accept(Scene *scene);
		void update(float dt);

	private:
		void init(float particleRadius, float spawnRate, float mass, float velocity,
			float velocityDeviation, GLuint maxParticles, float maxLifeTime);
		void destroyParticles(float dt);
		void spawnParticles(float dt);
		void createParticle(glm::vec3 position, glm::vec3 velocity);
		float random(float min, float max);
		
};