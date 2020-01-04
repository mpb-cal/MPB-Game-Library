#ifndef __MPBSPRING_H__
#define __MPBSPRING_H__

class MPBPhysicsObject;

class MPBSpring
{
	public:

		MPBSpring(	MPBPhysicsObject* object1, 
						MPBPhysicsObject* object2, 
						bool pullBoth, 
						MPBFLOAT restLength, 
						MPBFLOAT springConstant,
						MPBFLOAT dampingConstant );

		void updateFrame();
		void draw() const;
		void draw2D( const void* hdc ) const;

	private:

		MPBFLOAT 				m_restLength;			// in meters
		MPBFLOAT 				m_springConstant;		// in kg/sec squared
		MPBFLOAT 				m_dampingConstant;	// in kg/sec
		MPBPhysicsObject*		m_object1;
		MPBPhysicsObject*		m_object2;
		bool						m_pullBoth;
};


#endif

