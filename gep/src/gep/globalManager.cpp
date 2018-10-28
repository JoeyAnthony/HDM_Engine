#pragma once
#include "stdafx.h"
#include "gep/globalManager.h"

//implemented susbsystems
#include "gepimpl/subsystems/renderer/renderer.h"
#include "gepimpl/subsystems/renderer/extractor.h"
//#include "gepimpl/settings.h"
#include "gepimpl/subsystems/updateFramework.h"
#include "gepimpl/subsystems/logging.h"
#include "gepimpl/subsystems/memoryManager.h"
#include "gepimpl/subsystems/resourceManager.h"
#include "gep/timer.h"


//singleton static members
gep::GlobalManager* volatile gep::DoubleLockingSingleton<gep::GlobalManager>::s_instance = nullptr;
gep::Mutex gep::DoubleLockingSingleton<gep::GlobalManager>::s_creationMutex;

namespace gep
{
	void GlobalManager::initialize()
	{
		// Logging
		m_pLogging = new Logging;
		m_pLogging->registerSink(new FileLogSink);
		m_pLogging->registerSink(new ConsoleLogSink);
		m_pLogging->registerSink(new ConsoleLogSink);

		//order of initialization:
		m_pMemoryManager = new MemoryManager;
		m_pResourceManager = new ResourceManager;
		m_pRenderer = new Renderer;
		m_pRendererExtractor = new RendererExtractor;
		m_pUpdateFramework = new UpdateFramework;
		m_pTimer = new Timer;
	}

	void GlobalManager::destroy()
	{
		//order of initialization:
		delete m_pMemoryManager;
		delete m_pResourceManager;
		delete m_pRenderer;
		delete m_pRendererExtractor;
		delete m_pUpdateFramework;
		delete m_pTimer;

		m_pMemoryManager = nullptr;
		m_pResourceManager = nullptr;
		m_pRenderer = nullptr;
		m_pRendererExtractor = nullptr;
		m_pUpdateFramework = nullptr;
		m_pTimer = nullptr;
		
		//logging
		delete m_pLogging;
		m_pLogging = nullptr;
	}

	GlobalManager::GlobalManager():
		m_pMemoryManager(nullptr),
		m_pResourceManager(nullptr),
		m_pRenderer(nullptr),
		m_pRendererExtractor(nullptr),
		m_pUpdateFramework(nullptr),
		m_pTimer(nullptr)
	{

	}

	GlobalManager::~GlobalManager()
	{

	}
}