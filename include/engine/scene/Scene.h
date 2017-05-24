#pragma once

#include <vector>
#include <string>
#include <map>

class SkyBox;
class Entity;
class MatrixStack;
class Behaviour;
class Camera;
class Mesh;
class PointLight;
class ParticleEmitter;
class ParticleSystem;
class SphereCollider;
class Collider;
class Cloth;
class SoftBody;
class Rope;
class RigidbodyComponent;
class Rigidbody;

struct Particle;
struct RenderCommand;

using namespace std;

/*
* This class represents a scene.
*/
class Scene
{
	private:
		string _name;
		vector<Entity*> *_entities;
		SkyBox *_skyBox;

		MatrixStack *_matrixStack;
		vector<Behaviour *> *_behaviours;
		vector<Camera *> *_cameras;
		vector<RenderCommand> *_renderCommands;
		vector<PointLight *> *_pointLights;
		vector<ParticleSystem *> *_particleSystems;
		vector<Collider *> *_colliders;
		vector<SoftBody *> *_softBodies;
		vector<Rigidbody *> *_rigidbodies;

    public:
		/*
		* Constructor. Creates a empty scene. 
		*/
        Scene(string sceneName);

		/*
		* Deconstructor.
		*/
        ~Scene();

		/*
		* Gets the name of the scene.
		*/
		string getName();

		void setSkyBox(SkyBox *skyBox);

		SkyBox *getSkyBox();

		/*
		* Adds a Entity to the scene. 
		*/
        void addEntity(Entity *entity);

		vector<Behaviour *> *getBehaviours();

		vector<Camera *> *getCameras();

		vector<PointLight *> *getPointLights();

		vector<RenderCommand> *getRenderCommands();

		vector<ParticleSystem *> *getParticleSystems();

		vector<Collider *> *getColliders();

		vector<SoftBody *> *getSoftBodies();

		vector<Rigidbody *> *getRigidbodies();

		/*
		* Parse functions
		*/
		void traverse();

		void visit(Behaviour *behaviour);

		void visit(Camera *camera);

		void visit(Mesh *mesh);

		void visit(PointLight *pointLight);

		void visit(ParticleEmitter *particleEmitter);

		void visit(Collider *collider);

		void visit(Cloth *cloth);

		void visit(Rope *rope);

		void visit(RigidbodyComponent *rigidbodyComponent);

    private:
		void depthFirst(vector<Entity*> *entities);
};