#pragma once
#include "gep/interfaces/renderer.h"

namespace gep
{
	class RendererExtractor : public IRendererExtractor
	{
		virtual const mat4 getViewMatrix() const;
		virtual const mat4 getProjectionMatrix() const;
	};
}

