#include "engine/physics/collision/CollisionSystem.h"

#include "engine/physics/ParticleSystem.h"
#include "engine/physics/Rigidbody.h"
#include "engine/physics/SoftBody.h"
#include "engine/physics/collision/SpatialHashing.h"
#include "engine/physics/collision/Collider.h"
#include "engine/physics/collision/SphereCollider.h"
#include "engine/physics/collision/PlaneCollider.h"
#include "engine/physics/constraint/ConstraintSolver.h"
#include "engine/physics/constraint/RPCollisionConstraint.h"
#include "engine/physics/constraint/PCollisionConstraint.h"
#include "engine/physics/constraint/RRCollisionConstraint.h"
#include "engine/physics/constraint/RCollisionConstraint.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_cross_product.hpp>

CollisionSystem::CollisionSystem(ConstraintSolver *constraintSolver)
{
	_constraintSolver = constraintSolver;
	_spatialHashing = new SpatialHashing();
}

void CollisionSystem::collisionResolution(vector<ParticleSystem *> *particleSystems, vector<SoftBody *> *softbodies, vector<Collider *> *colliders)
{
	extractColliders(colliders);
	setupSpatialHashing();

	for (ParticleSystem *particleSystem : *particleSystems)
	{
		for (ParticleSystem::iterator it = particleSystem->begin(); it != particleSystem->end(); it++)
		{
			particleSphereResolution(&it);
			particlePlaneResolution(&it);
		}
	}

	for (SoftBody *softbody : *softbodies)
	{
		for (ParticleSystem::iterator it = softbody->begin(); it != softbody->end(); it++)
		{
			particleSphereResolution(&it);
			particlePlaneResolution(&it);
		}
	}

	for (SphereCollider *sphereCollider : _sphereColliders)
	{
		sphereSphereResolution(sphereCollider);
		spherePlaneResolution(sphereCollider);
	}

	//Applying impulses
	for (ParticleSystem *particleSystem : *particleSystems)
	{
		for (ParticleSystem::iterator it = particleSystem->begin(); it != particleSystem->end(); it++)
		{
			Particle *particle = &it;

			particle->velocity += particle->inverseMass*particle->J;
			particle->J = glm::vec3(0.0f);
		}
	}

	for (SoftBody *softbody : *softbodies)
	{
		for (ParticleSystem::iterator it = softbody->begin(); it != softbody->end(); it++)
		{
			Particle *particle = &it;

			particle->velocity += particle->inverseMass*particle->J;
			particle->J = glm::vec3(0.0f);
		}
	}

	for (SphereCollider *collider : _sphereColliders)
	{
		collider->getRigidbody()->addImpuleseToVelocity();
		collider->getRigidbody()->resetImpulse();
	}
}

void CollisionSystem::extractColliders(vector<Collider *> *colliders)
{
	_planeColliders.clear();
	_sphereColliders.clear();

	for (Collider *collider : *colliders)
	{
		SphereCollider *sphereCollider = nullptr;
		PlaneCollider *planeCollider = nullptr;

		sphereCollider = dynamic_cast<SphereCollider *> (collider);
		planeCollider = dynamic_cast<PlaneCollider *> (collider);

		if (sphereCollider != nullptr)
		{
			_sphereColliders.push_back(sphereCollider);
		}
		else if (planeCollider != nullptr)
		{
			_planeColliders.push_back(planeCollider);
		}
	}
}

void CollisionSystem::setupSpatialHashing()
{
	_spatialHashing->clear();

	for (SphereCollider *sphereCollider : _sphereColliders)
	{
		_spatialHashing->insert(sphereCollider);
	}
}

void CollisionSystem::particleSphereResolution(Particle *particle)
{
	vector<SphereCollider *> *possibleIntersections = _spatialHashing->checkSphere(particle);

	for (SphereCollider *collider : *possibleIntersections)
	{
		Intersection intersection = collider->checkCollision(particle);

		if (intersection.intersecting)
		{
			Rigidbody *r = collider->getRigidbody();
			glm::vec3 u = r->getVelocity() - particle->velocity;
			glm::vec3 n = intersection.normal;
			glm::vec3 un = glm::dot(u, n)*n;
			glm::vec3 t = glm::normalize(u - un);

			float NdotV = glm::dot(n, u);
			glm::vec3 ra = -n*collider->getRadius();
			glm::vec3 rb = n*particle->radius;

			if (NdotV < 0.0f && glm::length(un) > 0.5f)
			{
				glm::mat3 K = r->getInverseMass()*glm::mat3() + particle->inverseMass*glm::mat3();
				K -= glm::matrixCross3(ra)*r->getInvI()*glm::matrixCross3(ra);
				K -= glm::matrixCross3(rb)*glm::mat3()*glm::matrixCross3(rb);
				glm::vec3 J = glm::inverse(K)*(-0.1f*un - u);
				glm::vec3 Jn = glm::dot(J, n)*n;
				glm::vec3 Jt = J - Jn;
				float coef = 0.45f;

				if (!(glm::length(Jt) <= coef*glm::length(Jn)))
				{
					glm::vec3 ut = u - un;
					glm::vec3 t = glm::normalize(ut);
					float j = -((1.0f + 0.1f) * glm::length(un)) / (glm::dot(n, K * (n - coef*t)));

					J = -j*n + coef*j*t;
				}

				particle->J -= J;
				r->addLinearImpulse(J);
				r->addAngularImpulse(glm::cross(ra, J));
			}
			else if (NdotV < 0.00001f)
			{
				RPCollisionConstraint *constraint = new RPCollisionConstraint(collider->getRigidbody(), particle, intersection.normal, intersection.distance);

				_constraintSolver->addConstraint(constraint);
			}
		}
	}
}

void CollisionSystem::particlePlaneResolution(Particle *particle)
{
	for (PlaneCollider *planeCollider : _planeColliders)
	{
		Intersection intersection = planeCollider->checkCollision(particle);

		if (intersection.intersecting)
		{
			glm::vec3 u = particle->velocity;
			glm::vec3 n = intersection.normal;
			glm::vec3 un = glm::dot(u, n)*n;
			glm::vec3 ut = u - un;
			glm::vec3 t = glm::normalize(u - un);

			float NdotV = glm::dot(-n, u);

			if (NdotV < 0.0f && glm::length(un) > 0.5f)
			{
				glm::vec3 J = -particle->mass*(1.0f + 0.1f)*un;
				J += particle->mass * (std::max(0.0f, 1.0f - 0.1f*(glm::length(un) / glm::length(ut))) - 1.0f)* ut;
				particle->J += J;

			}
			else if (NdotV < 0.00001f)
			{
				PCollisionConstraint *constraint = new PCollisionConstraint(particle, -intersection.normal, intersection.distance);

				_constraintSolver->addConstraint(constraint);
			}
		}
	}
}

void CollisionSystem::sphereSphereResolution(SphereCollider *sphereCollider)
{
	vector<SphereCollider *> *possibleIntersections = _spatialHashing->checkSphere(sphereCollider);

	for (SphereCollider *sphereCollider2 : *possibleIntersections)
	{
		Intersection intersection = sphereCollider2->checkCollision(sphereCollider);

		if (intersection.intersecting)
		{
			Rigidbody *r1 = sphereCollider->getRigidbody();
			Rigidbody *r2 = sphereCollider2->getRigidbody();
			glm::vec3 u = r2->getVelocity() - r1->getVelocity();
			glm::vec3 n = intersection.normal;
			glm::vec3 un = glm::dot(u, n)*n;
			glm::vec3 t = glm::normalize(u - un);

			float NdotV = glm::dot(n, u);
			glm::vec3 ra = n*sphereCollider->getRadius();
			glm::vec3 rb = -n*sphereCollider2->getRadius();

			if (NdotV < 0.0f && glm::length(un) > 0.5f)
			{
				glm::mat3 K = r1->getInverseMass()*glm::mat3() + r2->getInverseMass()*glm::mat3();
				K -= glm::matrixCross3(ra)*r1->getInvI()*glm::matrixCross3(ra);
				K -= glm::matrixCross3(ra)*r2->getInvI()*glm::matrixCross3(rb);
				glm::vec3 J = glm::inverse(K)*(-0.1f*un - u);
				glm::vec3 Jn = glm::dot(J, n)*n;
				glm::vec3 Jt = J - Jn;
				float coef = 0.1f;

				if (!(glm::length(Jt) <= coef*glm::length(Jn)))
				{
					glm::vec3 ut = u - un;
					glm::vec3 t = glm::normalize(ut);
					float j = -((1.0f + 0.1f) * glm::length(un)) / (glm::dot(n, K * (n - coef*t)));

					J = -j*n + coef*j*t;
				}

				r1->addLinearImpulse(-J);
				r2->addLinearImpulse(J);
				r1->addAngularImpulse(glm::cross(ra, J));
				r2->addAngularImpulse(-glm::cross(rb, J));

			}
			else if (NdotV < 0.00001f)
			{
				RRCollisionConstraint *constraint = new RRCollisionConstraint(sphereCollider->getRigidbody(), sphereCollider2->getRigidbody(), -intersection.normal, intersection.distance);

				_constraintSolver->addConstraint(constraint);
			}
		}
	}
}

void CollisionSystem::spherePlaneResolution(SphereCollider *sphereCollider)
{
	for (PlaneCollider *planeCollider : _planeColliders)
	{
		Intersection intersection = planeCollider->checkCollision(sphereCollider);

		if (intersection.intersecting)
		{
			Rigidbody *r1 = sphereCollider->getRigidbody();
			glm::vec3 u = r1->getVelocity();
			glm::vec3 n = intersection.normal;
			glm::vec3 un = glm::dot(u, n)*n;
			glm::vec3 t = glm::normalize(u - un);

			float NdotV = glm::dot(-n, u);
			glm::vec3 ra = -n*sphereCollider->getRadius();

			if (NdotV < 0.0f && glm::length(un) > 0.5f)
			{
				glm::mat3 K = r1->getInverseMass()*glm::mat3();
				K -= glm::matrixCross3(ra)*r1->getInvI()*glm::matrixCross3(ra);
				glm::vec3 J = glm::inverse(K)*(-0.9f*un - u);
				glm::vec3 Jn = glm::dot(J, n)*n;
				glm::vec3 Jt = J - Jn;
				float coef = 0.1f;

				if (!(glm::length(Jt) <= coef*glm::length(Jn)))
				{
					glm::vec3 ut = u - un;
					glm::vec3 t = glm::normalize(ut);
					float j = -((1.0f + 0.9f) * glm::length(un)) / (glm::dot(n, K * (n - coef*t)));

					J = j*n + coef*j*t;
				}

				r1->addLinearImpulse(J);
				r1->addAngularImpulse(-glm::cross(ra, J));
			}
			else if (NdotV < 0.00001f)
			{
				RCollisionConstraint *constraint = new RCollisionConstraint(sphereCollider->getRigidbody(), intersection.normal, intersection.distance);

				_constraintSolver->addConstraint(constraint);
			}
		}
	}
}