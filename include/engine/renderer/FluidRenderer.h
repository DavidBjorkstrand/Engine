#pragma once

#include <vector>

class Camera;
class Mesh;
class Texture;
class TextureGenerator;
class Shader;
struct RenderCommand;

using namespace std;

class FluidRenderer
{
	public: 
		FluidRenderer();
		void draw(Camera *camera, vector<RenderCommand> renderCommands);

		Texture *getDepthMap();
		Texture *getThicknessMap();
		Texture *getNoiseMap();

	private:
		void drawDepth(Camera *camera, vector<RenderCommand> renderCommands);
		void smoothDepth(Camera *camera);
		void drawThickness(Camera *camera, vector<RenderCommand> renderCommands);
		void smoothThickness(Camera *camera);
		void drawNoise(Camera *camera, vector<RenderCommand> renderCommands);
		void smoothNoise();

		Mesh *_screenAlignedQuad;

		// Particle depth map
		Shader *_depthShader;
		TextureGenerator *_fluidDepthMapGenerator;

		Shader *_thicknessShader;
		TextureGenerator *_fluidThicknessMapGenerator;

		// Smoothing
		Shader *_curvatureFlow;
		Shader *_smoother;
		TextureGenerator *_fluidDepthMapSmoother[2];
		TextureGenerator *_fluidThicknessMapSmoother[2];
		TextureGenerator *_fluidNoiseMapSmoother[2];

		Shader *_noiseShader;
		TextureGenerator *_fluidNoiseMapGenerator;
};