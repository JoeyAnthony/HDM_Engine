#pragma once
#include "gep/interfaces/renderer.h"

namespace gep
{
    //forward declarations
    class Texture2D;
    class ITexture2DLoader;
    class Font;
    enum class FontHorizontalOrientation;
    class Shader;
    class Vertexbuffer;
    class Model;
    enum class ModelType;
    class RendererExtractor;
    struct CommandBase;
    struct LineInfo;
    struct LineInfo2D;

    struct RenderTextInfo
    {
        uint32 startIndex, numIndices;
    };


    enum class TextureMode
    {
        Static,
        Dynamic
    };

    class Renderer : public IRenderer
    {
		// Inherited via IRenderer
		virtual void initialize() override;
		virtual void destroy() override;
		virtual IDebugRenderer & getDebugRenderer() override;
	};

    class DebugRenderer : public IDebugRenderer
    {
		// Inherited via IDebugRenderer
		virtual void drawLine(const vec3 & start, const vec3 & end, Color color = Color::white()) override;
		virtual void drawLine(const vec2 & start, const vec2 & end, Color color = Color::white()) override;
		virtual void drawArrow(const vec3 & start, const vec3 & end, Color color = Color::white()) override;
		virtual void drawBox(const vec3 & min, const vec3 & max, Color color = Color::white()) override;
		virtual void printText(const vec2 & screenPositionNormalized, const char * text, Color color = Color::white()) override;
		virtual void printText(const vec3 & worldPosition, const char * text, Color color = Color::white()) override;
		virtual void drawLocalAxes(const vec3 & objectPosition, const Quaternion & objectRotation, float axesScale = 10.0f, Color colorX = Color::red(), Color colorY = Color::green(), Color colorZ = Color::blue()) override;
		virtual void drawLocalAxes(const vec3 & objectPosition, float axesScale = 10.0f, Color colorX = Color::red(), Color colorY = Color::green(), Color colorZ = Color::blue()) override;
	};
}
