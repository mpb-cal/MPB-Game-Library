#ifndef __PERFORMANCEMONITOR_H__
#define __PERFORMANCEMONITOR_H__


class MPBPerformanceMonitor
{
	public:

	static int				m_spherePlaneCollisionTests;
	static int				m_sphereSphereCollisionTests;
	static int				m_aabbAABBCollisionTests;
	static int				m_planesDrawn;
	static int				m_shapesDrawn;
	static int				m_messagesReceived;

	static void updateFrame();
};




#endif


