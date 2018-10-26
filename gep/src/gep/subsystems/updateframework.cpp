#include "stdafx.h"
#include "..\..\..\include\gepimpl\subsystems\updateFramework.h"

namespace gep
{

	void gep::UpdateFramework::stop()
	{
	}

	void gep::UpdateFramework::run()
	{
	}

	float gep::UpdateFramework::getElapsedTime() const
	{
		return 0.0f;
	}

	float gep::UpdateFramework::calcElapsedTimeAverage(size_t numFrames) const
	{
		return 0.0f;
	}

	CallbackId gep::UpdateFramework::registerUpdateCallback(std::function<void(float elapsedTime)> callback)
	{
		return CallbackId(0);
	}

	void gep::UpdateFramework::deregisterUpdateCallback(CallbackId id)
	{
	}
}
