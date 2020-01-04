#ifndef __SHADOW_H__
#define __SHADOW_H__


class MPBVector;
class Plane;
//class Stillbot;

class MPBShadow
{
public:

	static void projectionShadowBegin();
	static void projectionShadowEnd();
	static void projectionShadowPrepReceiver( Plane* receiver, int index );
	static void projectionShadowMatrix(	MPBFLOAT* shadowMatrix, 
													const MPBVector& lightPosition, 
													const Plane* receivingPlane	);
};




#endif

