#ifndef __MPBHARDSPRING_H__
#define __MPBHARDSPRING_H__

class MPBPhysicsParticle;

class MPBHardSpring
{
	public:

		MPBHardSpring(	MPBPhysicsParticle* object1, 
							MPBPhysicsParticle* object2, 
							MPBFLOAT restLength	);

		MPBHardSpring(	MPBPhysicsParticle* object1, 
							MPBPhysicsParticle* object2, 
							MPBFLOAT restLengthMin,
							MPBFLOAT restLengthMax	);

		MPBHardSpring(	MPBPhysicsParticle* object1, 
							MPBPhysicsParticle* object2	);

		void updateFrame();
		void draw() const;

	private:

		MPBFLOAT 					m_restLengthMin;			// in meters
		MPBFLOAT 					m_restLengthMax;			// in meters
		MPBPhysicsParticle*		m_object1;
		MPBPhysicsParticle*		m_object2;
};


#endif

