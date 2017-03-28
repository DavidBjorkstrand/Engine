#pragma once

#include <vector>

class Entity;
class Mesh;
class Camera;
class Behaviour;
class RenderJob;
class MatrixStack;
class RenderInformation;

using namespace std;

/*
* This class parses the contents of a scene. 
*/
class SceneParser 
{
	public:
		/*
		* Constructor.
		*/
		SceneParser();

		/*
		* Deconstructor. 
		*/
		~SceneParser();

		/*
		* Gets render information from scene.
		*/
		RenderInformation *getRenderInformation();

		/*
		* Gets behviours from scene.
		*/
		vector<Behaviour *> *getBehaviours();

		/*
		* Parses the scene graph with the roots in root. 
		*/
		void parse(vector<Entity *> *root);

		/*
		* All visit function are used for scene parsing.
		* There is one for every subclass of Component. 
		*/
		void visit(Mesh *mesh);

		void visit(Camera *camera);

		void visit(Behaviour *behaviour);

	private:
		/*
		* Private helper function that traverses scene. 
		* Depth first. 
		*/
		void traverse(vector<Entity *> *root);

		vector<RenderJob *> *_renderJobs;
		vector<Camera *> *_cameras;
		vector<Behaviour *> *_behaviours;
		MatrixStack *_matrixStack;
};