
#include "MPBPerformanceMonitor.h"
#include "mmgr.h"

int				MPBPerformanceMonitor::m_spherePlaneCollisionTests = 0;
int				MPBPerformanceMonitor::m_sphereSphereCollisionTests = 0;
int				MPBPerformanceMonitor::m_aabbAABBCollisionTests = 0;
int				MPBPerformanceMonitor::m_planesDrawn = 0;
int				MPBPerformanceMonitor::m_shapesDrawn = 0;
int				MPBPerformanceMonitor::m_messagesReceived = 0;



void MPBPerformanceMonitor::updateFrame()
{
	m_spherePlaneCollisionTests = 0;
	m_sphereSphereCollisionTests = 0;
	m_aabbAABBCollisionTests = 0;
	m_planesDrawn = 0;
	m_shapesDrawn = 0;
}




