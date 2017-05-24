#pragma once

#include <vector>

#include <glm/gtc/type_ptr.hpp>

using namespace std;

class RigidbodyListener
{
	public:
		virtual void notify(glm::vec3 position) = 0;
		virtual void notify(glm::quat rotation) = 0;
};

class Rigidbody
{
	private:
		glm::vec3 _position;
		glm::quat _rotation;
		glm::vec3 _velocity;
		glm::vec3 _predictedVelocity;
		glm::vec3 _angularVelocity;
		glm::vec3 _acceleration;
		glm::vec3 _force;
		float _mass;
		float _inverseMass;
		glm::mat3 _I;
		glm::mat3 _invI;

		vector<RigidbodyListener *> *_listeners;

	public:
		Rigidbody(glm::vec3 position, float mass, glm::mat3 I);
		void addListener(RigidbodyListener *rigidbodyListener);
		void setPosition(glm::vec3 position);
		void setRotation(glm::quat rotation);
		void setVelocity(glm::vec3 velocity);
		void setPredictedVelocity(glm::vec3 predictedVelocity);
		void setAngularVelocity(glm::vec3 angularVelocity);
		void setAcceleration(glm::vec3 acceleration);
		void addForce(glm::vec3 deltaForce);
		void resetForce();

		glm::vec3 getPosition();
		glm::quat getRotation();
		glm::vec3 getVelocity();
		glm::vec3 getPredictedVelocity();
		glm::vec3 getAngularVelocity();
		glm::vec3 getAcceleration();
		glm::vec3 getForce();
		float getMass();
		float getInverseMass();
		glm::mat3 getI();
		glm::mat3 getInvI();
};

