#include "engine/renderer/RenderInformation.h"

#include "engine/renderer/RenderJob.h"
#include "engine/scene/entity/component/Camera.h"

#include <vector>

RenderInformation::RenderInformation(vector<Camera *> *cameras, vector<RenderJob *> *renderJobs, vector<PointLight *> *pointLights)
{
	_cameras = cameras;
	_renderJobs = renderJobs;
	_pointLights = pointLights;
}

vector<Camera *> *RenderInformation::getCameras()
{
	return _cameras;
}

vector<RenderJob *> *RenderInformation::getRenderJobs()
{
	return _renderJobs;
}

vector<PointLight *> *RenderInformation::getPointLights()
{
	return _pointLights;
}