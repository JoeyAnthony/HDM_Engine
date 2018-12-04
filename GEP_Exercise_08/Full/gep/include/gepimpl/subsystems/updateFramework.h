#pragma once

#include "gep/interfaces/updateFramework.h"
#include "gep/ArrayPtr.h"
#include "gep/container/dynamicarray.h"
#include "gep/timer.h"



namespace gep
{

    class UpdateFramework
        : public IUpdateFramework
    {
    private:
        float m_pFrameTimesArray[60];
        ArrayPtr<float> m_FrameTimesPtr;
        size_t m_frameIdx;
        bool m_running;

		PointInTime lastFrameTime;
		Timer gameTimer;
		size_t m_callbacknr = 0;
		DynamicArray<size_t> m_ids;
		DynamicArray<std::function<void(float time)>> m_updateDelegate;



    public:
        UpdateFramework();

        // IUpdateFramework interface
        virtual void stop() override;
        virtual void run() override;
        virtual float getElapsedTime() const override;
        virtual float calcElapsedTimeAverage(size_t numFrames) const override;
        virtual CallbackId registerUpdateCallback(std::function<void(float elapsedTime)> callback) override;
        virtual void deregisterUpdateCallback(CallbackId id) override;



    };
}
