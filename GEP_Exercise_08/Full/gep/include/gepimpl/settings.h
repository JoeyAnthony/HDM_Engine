#pragma once
#include "gep/settings.h"

#include "gep/math3d/vec2.h"

namespace gep
{
    class Settings : public ISettings
    {
        settings::General m_general;
        settings::Video m_video;

    public:

        virtual void setVideoSettings(const settings::Video& settings) override { m_video = settings; }
        virtual       settings::Video& getVideoSettings()       override { return m_video; }
        virtual const settings::Video& getVideoSettings() const override { return m_video; }

        virtual void setGeneralSettings(const settings::General& settings) override { m_general = m_general; }
        virtual       settings::General& getGeneralSettings()       override { return m_general; }
        virtual const settings::General& getGeneralSettings() const override { return m_general; }
        
    };
}
