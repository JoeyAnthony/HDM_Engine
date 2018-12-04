#pragma once

#include "gep/math3d/vec2.h"
#include "gep/math3d/color.h"


namespace gep
{
    namespace settings
    {
        struct General
        {
            std::wstring applicationTitle;

            General() :
				applicationTitle(L"Game Engine Programming")
            {
            }
        };


        struct Video
        {
            uvec2 initialRenderWindowPosition;
            uvec2 screenResolution;
            bool vsyncEnabled;
            bool adaptiveVSyncEnabled;
            float adaptiveVSyncThreshold;
            float adaptiveVSyncTolerance;
            Color clearColor;

            Video() :
                initialRenderWindowPosition(CW_USEDEFAULT, CW_USEDEFAULT),
                screenResolution(1280, 720),
                vsyncEnabled(false),
                adaptiveVSyncEnabled(true),
                adaptiveVSyncThreshold(1.0f / 59.0f), // 59 FPS
                adaptiveVSyncTolerance(1),            // +- 1 FPS
                clearColor(0.0f, 0.125f, 0.3f, 1.0f)  // Blueish color
            {
            }
        };
        
    }
    
    class ISettings
    {
    public:
        ISettings() {}
        virtual ~ISettings() {}

        virtual void setGeneralSettings(const settings::General& settings) = 0;
        virtual       settings::General& getGeneralSettings() = 0;
        virtual const settings::General& getGeneralSettings() const = 0;

        virtual void setVideoSettings(const settings::Video& settings) = 0;
        virtual       settings::Video& getVideoSettings() = 0;
        virtual const settings::Video& getVideoSettings() const = 0;
        
    };
}
