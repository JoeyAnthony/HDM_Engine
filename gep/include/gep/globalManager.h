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
		ILogging* m_pLogging;
		//ILogSink* m_pLogSink;
		IMemoryManager* m_pMemoryManager;
		IResourceManager* m_pResourceManager;
		Timer* m_pTimer;

    public:
        inline IRenderer* getRenderer()
        {
            return m_pRenderer;
        }
        inline IRendererExtractor* getRendererExtractor()
        {
            return m_pRendererExtractor;
        }
        inline IUpdateFramework* getUpdateFramework()
        {
            return m_pUpdateFramework;
        }
        inline ILogging* getLogging()
        {
            return m_pLogging;
        }
        inline IMemoryManager* getMemoryManager()
        {
            return m_pMemoryManager;
        }
        inline IResourceManager* getResourceManager()
        {
            return m_pResourceManager;
        }
        inline Timer* getTimer()
        {
            return m_pTimer;
        }

		virtual void initialize() override;
		virtual void destroy() override;

		GlobalManager();
		~GlobalManager();
    };
}

#define g_globalManager gep::GlobalManager::instance()
#define g_logMessage(msg) gep::GlobalManager::instance().getLogging()->logMessage(msg)
#define g_logWarning(msg) gep::GlobalManager::instance().getLogging()->logWarning(msg)
#define g_logError(msg) gep::GlobalManager::instance().getLogging()->logError(msg)
