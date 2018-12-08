#include "stdafx.h"
#include "gepimpl/subsystems/updateFramework.h"
#include "gep/globalmanager.h"
#include "gepimpl/subsystems/renderer/renderer.h"
#include "gepimpl/subsystems/renderer/extractor.h"
#include "gepimpl/subsystems/resourcemanager.h"

gep::UpdateFramework::UpdateFramework() :
    m_FrameTimesPtr(m_pFrameTimesArray)
    , m_frameIdx(m_FrameTimesPtr.length()-1)
{
    // initialize the frame times array to some default value
    const float defaultTime = 1.0f / 60.0f;
    for (auto& frametime : m_FrameTimesPtr)
    {
        frametime = defaultTime;
    }
}

void gep::UpdateFramework::stop()
{
	m_running = false;
}

void gep::UpdateFramework::run()
{
	lastFrameTime = PointInTime(gameTimer);
	while (m_running) {

		for(std::function<void(float elapsedTime)> f : m_updateDelegate)
		{
			f(m_pFrameTimesArray[m_frameIdx]);
		}

		g_globalManager.instance().getResourceManager()->update(getElapsedTime());
		g_globalManager.instance().getRenderer()->update(getElapsedTime());

		//calculate delta time
		PointInTime newFrameTime = PointInTime(gameTimer);
		float frameTime = newFrameTime - lastFrameTime;
		m_frameIdx = (m_frameIdx + 1) % m_FrameTimesPtr.length();
		m_FrameTimesPtr[m_frameIdx] = frameTime;
	}
}



float gep::UpdateFramework::getElapsedTime() const
{
	return m_FrameTimesPtr[m_frameIdx];
}

float gep::UpdateFramework::calcElapsedTimeAverage(size_t numFrames) const
{
    // TODO implement
    return 1.0f / 60.0f;
}

gep::CallbackId gep::UpdateFramework::registerUpdateCallback(std::function<void(float elapsedTime)> callback)
{
	size_t id = m_callbacknr;
	m_callbacknr++;
	m_ids.append(id);
	m_updateDelegate.append(callback);
    return gep::CallbackId(id);
}


void gep::UpdateFramework::deregisterUpdateCallback(CallbackId id)
{
	for (int i = 0; i < m_updateDelegate.length(); ++i) {
	    if(id.id == m_ids[i]){
			m_updateDelegate.removeAtIndex(i);
			m_ids.removeAtIndex(i);
			break;
	    }
    }
}



