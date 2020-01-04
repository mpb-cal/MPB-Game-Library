#ifndef __MPBPLANESPRING_H__
#define __MPBPLANESPRING_H__

class MPBPhysicsParticle;

class MPBPlaneSpring
{
	public:

		MPBPlaneSpring(	MPBPhysicsParticle* particle, 
								MPBPhysicsParticle* normalStart, 
								MPBPhysicsParticle* normalEnd,
								MPBPhysicsParticle* pointInPlane	);

		MPBPlaneSpring(	MPBPhysicsParticle* particle, 
								MPBPhysicsParticle* planeLL, 
								MPBPhysicsParticle* planeLR,
								MPBPhysicsParticle* planeUR,
								bool stayBehind	);

		void updateFrame();
		void draw() const;

	private:

		MPBPhysicsParticle*		m_particle;
		MPBPhysicsParticle*		m_normalStart;
		MPBPhysicsParticle*		m_normalEnd;
		MPBPhysicsParticle*		m_pointInPlane;
		MPBPhysicsParticle*		m_planeLL;
		MPBPhysicsParticle*		m_planeLR;
		MPBPhysicsParticle*		m_planeUR;
		bool							m_stayBehind;
};


#endif

