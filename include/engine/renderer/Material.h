#pragma once

#include <iostream>

#include <glm/gtc/type_ptr.hpp>

class Texture;

using namespace std;

class Material
{
	public: 
		/*
		* Constructor
		*/
		Material(string name, glm::vec3 albedo, float roughness, float metallic);

		~Material();

		/*
		* Gets the name of the material
		*/
		string getName();

		/*
		* Gets the albedo of the material.
		*/
		glm::vec3 getAlbedo();

		Texture *getAlbedoMap();

		void setAlbedoMap(Texture *texture);

		/*
		* Gets the roughness of the material.
		*/
		float getRoughness();

		Texture *getRoughnessMap();

		void setRoughnessMap(Texture *texture);

		/*
		* Gets how metllic the matrial is. 
		*/
		float getMetallic();

		Texture *getMetallicMap();

		void setMetallicMap(Texture *texture);

	private:
		string _name;
		glm::vec3 _albedo;
		Texture *_albedoMap;
		float _roughness;
		Texture *_roughnessMap;
		float _metallic;
		Texture *_metallicMap;
};