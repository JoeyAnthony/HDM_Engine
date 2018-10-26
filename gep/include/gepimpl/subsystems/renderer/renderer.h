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

    class DebugRenderer
    {
    };
}
