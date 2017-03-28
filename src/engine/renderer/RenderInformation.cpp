#include "RenderInformation.h"
#include "RenderJob.h"
#include "..\scene\entity\component\Camera.h"

#include <vector>

RenderInformation::RenderInformation(vector<Camera *> *cameras, vector<RenderJob *> *renderJobs)
{
	_cameras = cameras;
	_renderJobs = renderJobs;
}

vector<Camera *> *RenderInformation::getCameras()
{
	return _cameras;
}

vector<RenderJob *> *RenderInformation::getRenderJobs()
{
	return _renderJobs;
}