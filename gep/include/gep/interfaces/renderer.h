#pragma once

#include "gep/interfaces/subsystem.h"
#include "gep/math3d/vec2.h"
#include "gep/math3d/vec3.h"
#include "gep/math3d/mat4.h"
#include "gep/math3d/color.h"
#include "gep/math3d/quaternion.h"
#include <functional>


namespace gep
{

    /// \brief Interface for debug drawing
    class IDebugRenderer
    {
    public:
        virtual ~IDebugRenderer(){}
        /// \brief draws a 3d line
        virtual void drawLine(const vec3& start, const vec3& end, Color color = Color::white()) = 0;
        /// \brief draws a 2d line
        virtual void drawLine(const vec2& start, const vec2& end, Color color = Color::white()) = 0;
        /// \brief draws a 3d arrow
        virtual void drawArrow(const vec3& start, const vec3& end, Color color = Color::white()) = 0;
        /// \brief draws a 3d axis aligned box
        virtual void drawBox(const vec3& min, const vec3& max, Color color = Color::white()) = 0;

        /// \brief prints text on the screen (2D)
        virtual void printText(const vec2& screenPositionNormalized, const char* text, Color color = Color::white()) = 0;
        /// \brief prints text in the world (3D)
        virtual void printText(const vec3& worldPosition, const char* text, Color color = Color::white()) = 0;

		/// \brief draws local axes at given world space coordinate
        virtual void drawLocalAxes(
            const vec3& objectPosition,
            const Quaternion& objectRotation,
            float axesScale = 10.0f,
            Color colorX = Color::red(),
            Color colorY = Color::green(),
            Color colorZ = Color::blue()) = 0;

        virtual void drawLocalAxes(
            const vec3& objectPosition,
            float axesScale = 10.0f,
            Color colorX = Color::red(),
            Color colorY = Color::green(),
            Color colorZ = Color::blue()) = 0;

    };


    /// \brief Renderer interface
    class IRenderer : public ISubsystem
    {
        friend class InputHandler;
    public:
        virtual ~IRenderer(){}
        /// \brief returns the debug renderer interface
        virtual IDebugRenderer& getDebugRenderer() = 0;
    };


    /// \brief Camera interface
    class GEP_API ICamera
    {
    public:
        virtual ~ICamera() = 0 {};
        virtual const mat4 getViewMatrix() const = 0;
        virtual const mat4 getProjectionMatrix() const = 0;
    };

    /// \brief Renderer extractor interface
    class IRendererExtractor
    {
    public:
        virtual ~IRendererExtractor(){}
    };



};

