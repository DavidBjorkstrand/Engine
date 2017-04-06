#pragma once

#include "engine/scene/entity/component/Behaviour.h"
#include "engine/physics/PhysicsSystem.h"

#include <vector>

#include <GL/glew.h>

class Scene;
class Mesh;

struct RenderCommand;

using namespace std;

struct Particle
{
	Rigidbody *rigidbody;
	long startTime;
	long lifeTime;
};

class ParticleEmitter : public Behaviour
{
	public:
		ParticleEmitter();
		void accept(Scene *scene);
		void update();
		vector<RenderCommand> getRenderCommands();
		vector<Rigidbody *> getRigidBodies();

	private:
		vector<Particle> *_inActiveParticles;
		vector<Particle> *_activeParticles;
		vector<Mesh *> *_particleMeshes;
};