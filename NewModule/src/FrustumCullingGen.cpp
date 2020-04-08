#include "FrustumCullingGen.h"
#include "ManagerGLView.h"
#include "GLView.h"
#include "Camera.h"
#include "Model.h"
#include "MGLFrustum.h"

using namespace Aftr;
using namespace std;

FrustumCullingGen::FrustumCullingGen() {

}


bool FrustumCullingGen::isCameraVisible(WO* wo) {
	Camera* cam = ManagerGLView::getGLView()->getCamera();
	float HFOV = cam->getCameraHorizontalFOVDeg();
	float VFOV = 2.0f * atan(tan(HFOV * DEGtoRAD / 2.0f) / cam->getCameraAspectRatioWidthToHeight()) * DEGtoRAD;
	AftrGeometryFrustum* frustumC = new AftrGeometryFrustum(cam->getCameraAspectRatioWidthToHeight(), VFOV, cam->getCameraNearClippingPlaneDistance(),
		cam->getCameraFarClippingPlaneDistance(), cam->getLookDirection(), cam->getNormalDirection(), cam->getPosition());

	return this->isInFrustum(wo, frustumC);
}


bool FrustumCullingGen::isFrustumVisible(WO* wo, WO* frust) {
	//MGLFrustum* frustum = frust->getModelT<MGLFrustum>();
	MGLFrustum* frustum = static_cast<MGLFrustum*>(frust->getModel());
	float HFOV = frustum->getHorzFOVDeg();
	float VFOV = 2.0f * atan(tan(HFOV * DEGtoRAD / 2.0f) / frustum->getAspectRatioWidthToHeight()) * DEGtoRAD;
	AftrGeometryFrustum* frustumNC = new AftrGeometryFrustum(frustum->getAspectRatioWidthToHeight(), VFOV, frustum->getNearPlane(), 
		frustum->getFarPlane(), frustum->getLookDirection(), frustum->getNormalDirection(), frustum->getPosition());

	return this->isInFrustum(wo, frustumNC);
}


bool FrustumCullingGen::isInFrustum(WO* wo, AftrGeometryFrustum* frustum) {
	Vector boxXYZ = wo->getModel()->getBoundingBox().getlxlylz() / 2.0f;
	Mat4 disMatrix = wo->getDisplayMatrix();
	Vector pos = wo->getPosition();
	// plane [0-5]
	for (int plane = 0; plane <= 5; plane++) {
		bool inside = false;
		Vector normal = frustum->getPlaneNormal(plane);
		float coef = frustum->getPlaneCoef(plane);

		for (int x = -1; x <= 1; x += 2) {
			for (int y = -1; y <= 1; y += 2) {
				for (int z = -1; z <= 1; z += 2) {
					if (normal.dotProduct(disMatrix * (Vector(x, y, z) * boxXYZ) + pos) < coef)
						inside = true;
				}
			}
		}

		if (!inside) {
			return false;
		}
	}

	return true;
}
