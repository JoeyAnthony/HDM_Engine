#include "stdafx.h"
#include "gep/globalManager.h"

#include "gepimpl/subsystems/renderer/renderer.h"
#include "gepimpl/subsystems/renderer/extractor.h"
#include "gepimpl/settings.h"
#include "gepimpl/subsystems/updateFramework.h"
#include "gepimpl/subsystems/logging.h"
#include "gepimpl/subsystems/memoryManager.h"
#include "gepimpl/subsystems/resourceManager.h"



#include "gep/timer.h"

//singleton static members
gep::GlobalManager* volatile gep::DoubleLockingSingleton<gep::GlobalManager>::s_instance = nullptr;
gep::Mutex gep::DoubleLockingSingleton<gep::GlobalManager>::s_creationMutex;

gep::GlobalManager::GlobalManager() :
    m_pRenderer(nullptr),
    m_pRendererExtractor(nullptr),
    m_pUpdateFramework(nullptr),
    m_pLogging(nullptr),
    m_ConsoleLogSink(nullptr),
    m_FileLogSink(nullptr),
    m_pMemoryManager(nullptr),
    m_pResourceManager(nullptr),
    m_pTimer(nullptr)
{
}

gep::GlobalManager::~GlobalManager()
{
}

void gep::GlobalManager::initialize()
{
    m_pLogging = new gep::Logging();
#ifdef _DEBUG
    m_ConsoleLogSink = new ConsoleLogSink();
    m_pLogging->registerSink(m_ConsoleLogSink);
#endif
    m_FileLogSink = new FileLogSink("logfile.txt");
    m_pLogging->registerSink(m_FileLogSink);
    m_pLogging->logMessage("log system initialized");

    m_pLogging->logMessage("\n==================================================");

    m_pLogging->logMessage("initializing settings");
    m_pSettings = new Settings();
    m_pLogging->logMessage("settings initialized");
    m_pLogging->logMessage("\n==================================================");

    m_pLogging->logMessage("initializing memory manager");
    m_pMemoryManager = new gep::MemoryManager();
    m_pMemoryManager->initialize();
    m_pLogging->logMessage("memory manager initialized");


    m_pLogging->logMessage("\n==================================================");

    m_pLogging->logMessage("initializing timer");
    m_pTimer = new Timer();
    m_pLogging->logMessage("timer initialized");

    m_pLogging->logMessage("\n==================================================");

    m_pLogging->logMessage("initializing update framework");
    m_pUpdateFramework = new gep::UpdateFramework();
    m_pLogging->logMessage("update framework initialized");

    m_pLogging->logMessage("\n==================================================");
    


    m_pLogging->logMessage("initializing resource manager");
    m_pResourceManager = new gep::ResourceManager();
    m_pResourceManager->initialize();
    m_pLogging->logMessage("resource manager initialized");


    m_pLogging->logMessage("\n==================================================");

    m_pLogging->logMessage("initializing renderer extractor");
    m_pRendererExtractor = new gep::RendererExtractor();
    m_pLogging->logMessage("renderer extractor initialized");

    m_pLogging->logMessage("\n==================================================");

    m_pLogging->logMessage("initializing renderer");
    m_pRenderer = new gep::Renderer(m_pSettings->getVideoSettings());
    m_pRenderer->initialize();
    m_pLogging->logMessage("renderer initialized");

    m_pLogging->logMessage("\n==================================================");



    m_pLogging->logMessage("\n"
        "==================================================\n"
        "=== Global Manager Initialized ===================\n"
        "==================================================\n");
}

void gep::GlobalManager::destroy()
{
    m_pLogging->logMessage("\n==================================================");


    m_pLogging->logMessage("destroying update framework");
    DELETE_AND_NULL(m_pUpdateFramework);
    m_pLogging->logMessage("update framework destroyed");


    m_pLogging->logMessage("\n==================================================");

    m_pLogging->logMessage("destroying timer");
    DELETE_AND_NULL(m_pTimer);
    m_pLogging->logMessage("timer destroyed");

    m_pLogging->logMessage("\n==================================================");
    
    m_pLogging->logMessage("destroying resource manager");
    if(m_pResourceManager) m_pResourceManager->destroy();
    DELETE_AND_NULL(m_pResourceManager);
    m_pLogging->logMessage("resource manager destroyed");
    m_pLogging->logMessage("\n==================================================");

    m_pLogging->logMessage("destroying resource manager");
    if(m_pResourceManager) m_pResourceManager->destroy();
    DELETE_AND_NULL(m_pResourceManager);
    m_pLogging->logMessage("resource manager destroyed");
    m_pLogging->logMessage("\n==================================================");


    m_pLogging->logMessage("destroying renderer");
    if(m_pRenderer) m_pRenderer->destroy();
    DELETE_AND_NULL(m_pRenderer);
    m_pLogging->logMessage("renderer destroyed");

    m_pLogging->logMessage("\n==================================================");

    m_pLogging->logMessage("destroying renderer extractor");
    DELETE_AND_NULL(m_pRendererExtractor);
    m_pLogging->logMessage("renderer extractor destroyed");

    m_pLogging->logMessage("\n==================================================");

    m_pLogging->logMessage("destroying memory manager");
    if(m_pMemoryManager) m_pMemoryManager->destroy();
    DELETE_AND_NULL(m_pMemoryManager);
    m_pLogging->logMessage("memory manager destroyed");

    m_pLogging->logMessage("\n==================================================");



    m_pLogging->logMessage("destroying settings");
    DELETE_AND_NULL(m_pSettings);
    m_pLogging->logMessage("settings destroyed");
    m_pLogging->logMessage("\n==================================================");

    m_pLogging->logMessage("destroying log system");
    m_pLogging->deregisterSink(m_FileLogSink);
    DELETE_AND_NULL(m_FileLogSink);
#ifdef _DEBUG
    m_pLogging->deregisterSink(m_ConsoleLogSink);
    DELETE_AND_NULL(m_ConsoleLogSink);
#endif
    DELETE_AND_NULL(m_pLogging);
}

