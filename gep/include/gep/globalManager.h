#pragma once

#include "gepmodule.h"
#include "gep/singleton.h"
#include "interfaces/subsystem.h"


namespace gep
{
    //forward declarations
    class IRenderer;
    class IRendererExtractor;
    class ISettings;
    class IUpdateFramework;
    class ILogging;
    class ILogSink;
    class IMemoryManager;
    class IResourceManager;
    class Timer;

	

    /// \brief the global manager responsible for destroying most subsystems
    class GEP_API GlobalManager : public DoubleLockingSingleton<GlobalManager>, public ISubsystem
    {
        friend class DoubleLockingSingleton<GlobalManager>;
    private:
		IRenderer* m_pRenderer;
		IRendererExtractor* m_pRendererExtractor;
		//ISettings* m_pSettings;
		IUpdateFramework* m_pUpdateFramework;
		//ILogging* m_pLogging;
		//ILogSink* m_pLogSink;
		IMemoryManager* m_pMemoryManager;
		IResourceManager* m_pResourceManager;
		Timer* m_pTimer;

    public:
        inline IRenderer* getRenderer()
        {
            return nullptr;
        }
        inline IRendererExtractor* getRendererExtractor()
        {
            return nullptr;
        }
        inline IUpdateFramework* getUpdateFramework()
        {
            return nullptr;
        }
        inline ILogging* getLogging()
        {
            return nullptr;
        }
        inline IMemoryManager* getMemoryManager()
        {
            return m_pMemoryManager;
        }
        inline IResourceManager* getResourceManager()
        {
            return nullptr;
        }
        inline Timer* getTimer()
        {
            return nullptr;
        }

		virtual void initialize() override;
		virtual void destroy() override;

		GlobalManager();
		~GlobalManager();
    };
}


#define g_globalManager gep::GlobalManager::instance()
