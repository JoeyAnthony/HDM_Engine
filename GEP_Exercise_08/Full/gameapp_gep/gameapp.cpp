// GameApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "gep/utils.h"

#include "gep/globalManager.h"
#include "gep/exit.h"
#include "gepimpl/subsystems/logging.h"
#include "gepimpl/subsystems/updateFramework.h"

// Implement new/delete so they match the engine dll
#include "gep/memory/newdelete.inl"
#include "gep/interfaces/renderer.h"


using namespace gep;


class GameApp
{
private:
    IModel* m_pBall;
    IModel* m_pSponza;
    mat4 m_ballTransform;

public:

    void initialize()
    {

        m_pBall = g_globalManager.getRenderer()->loadModel("data/models/ball.thModel");
        m_pSponza = g_globalManager.getRenderer()->loadModel("data/sponza/sponza.thModel");
        // TODO register the render function as callback with the renderer extractor
    }

    void gameloop(float elapsedTime)
    {
        // TODO animate the ball here
        m_ballTransform = mat4::identity();
    }

    void render(IRendererExtractor& extractor)
    {
        m_pSponza->extract(extractor, mat4::identity());
        m_pBall->extract(extractor, m_ballTransform);

        auto& context2D = extractor.getContext2D();
        context2D.printText(vec2(0.05f, 0.05f), "TODO print frames per second here");
    }

};

int main(int argc, const char* argv[])
{
    {
        g_globalManager.initialize();
        const unsigned int numFrames = 500;
        g_globalManager.getLogging()->logMessage("updating %d frames", numFrames);
        for (unsigned int frame=0; frame<numFrames; ++frame)
        {
            g_globalManager.getUpdateFramework()->run();
            if (frame % 50 == 0)
                g_globalManager.getLogging()->logMessage(".");
            Sleep(17);
        }
        g_globalManager.getLogging()->logMessage("\n%d frames updated", numFrames);

        g_globalManager.destroy();
    }

    gep::destroy(); //Shutdown gep

    #ifdef _DEBUG
    std::cout << std::endl << "press any key to quit...";
    std::cin.get();
    #endif

    return 0;
}

