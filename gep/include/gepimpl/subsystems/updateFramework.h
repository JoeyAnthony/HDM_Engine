#pragma once
#include "gep/interfaces/updateframework.h"



namespace gep
{

    class UpdateFramework : public IUpdateFramework
    {
		// Inherited via IUpdateFramework
		virtual void stop() override;
		virtual void run() override;
		virtual float getElapsedTime() const override;
		virtual float calcElapsedTimeAverage(size_t numFrames) const override;
		virtual CallbackId registerUpdateCallback(std::function<void(float elapsedTime)> callback) override;
		virtual void deregisterUpdateCallback(CallbackId id) override;
	};
}
