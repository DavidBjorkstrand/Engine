#pragma once

#include "engine/scene/entity/component/Behaviour.h"

#include <vector>

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

class Scene;
class Mesh;

struct Particle;
struct RenderCommand;

using namespace std;

class ParticleEmitter : public Behaviour
{
	private:
		Mesh *_particleMesh;
		float _particleRadius;
		glm::vec2 _spawnArea;
		float _spawnRadius;
		float _spawnRate;
		float _spawnRest;
		float _mass;
		float _inverseMass;
		float _velocity;
		float _velocityDeviation;
		GLuint _maxParticles;
		GLuint _nrParticles;
		float _maxLifeTime;
		float *_lifeTime;
		vector<Particle> *_particles;
		vector<GLuint> *_freeIndexes;
		vector<RenderCommand> *_renderCommands;
		GLboolean _sphereSpawn;
		
	public:
		ParticleEmitter(float particleRadius, glm::vec2 spawnArea, float spawnRate, float mass, 
			float velocity, float velocityDeviation, GLuint maxParticles, float maxLifeTime);
		ParticleEmitter(float particleRadius, float spawnRadius, float spawnRate, float mass,
			float velocity, float velocityDeviation, GLuint maxParticles, float maxLifeTime);
		~ParticleEmitter();
		vector<Particle> *getParticles();
		vector<RenderCommand> *getRenderCommands();
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