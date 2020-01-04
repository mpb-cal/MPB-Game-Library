#ifndef __MPBHARDSPRINGANGULAR_H__
#define __MPBHARDSPRINGANGULAR_H__

class MPBPhysicsParticle;

class MPBHardSpringAngular
{
	public:

		MPBHardSpringAngular(	MPBPhysicsParticle* object1, 
										MPBPhysicsParticle* object2, 
										MPBPhysicsParticle* objectHinge, 
										MPBFLOAT restAngle	);

		MPBHardSpringAngular(	MPBPhysicsParticle* object1, 
										MPBPhysicsParticle* object2, 
										MPBPhysicsParticle* objectHinge, 
										MPBFLOAT restAngleMin,
										MPBFLOAT restAngleMax	);

//		MPBHardSpringAngular(	MPBPhysicsParticle* object1, 
//										MPBPhysicsParticle* object2,
//										MPBPhysicsParticle* objectHinge	);

		void updateFrame();
		void draw() const;

	private:

		MPBFLOAT 					m_restAngleMin;			// in radians
		MPBFLOAT 					m_restAngleMax;			// in radians
		MPBPhysicsParticle*		m_object1;
		MPBPhysicsParticle*		m_object2;
		MPBPhysicsParticle*		m_objectHinge;
};


#endif

