#include "AftrGeometryFrustum.h"
#include "WO.h"

namespace Aftr {
	class FrustumCullingGen {
	public:
		FrustumCullingGen();
		bool isCameraVisible(WO* frust);
		bool isFrustumVisible(WO* wo, WO* frust);
		bool isInFrustum(WO* wo, AftrGeometryFrustum* frustum);
	};
}