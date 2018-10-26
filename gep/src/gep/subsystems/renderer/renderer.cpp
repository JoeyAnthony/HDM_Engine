#include "stdafx.h"
#include "..\..\..\..\include\gepimpl\subsystems\renderer\renderer.h"

namespace gep {
	void gep::Renderer::initialize()
	{
	}

	void gep::Renderer::destroy()
	{
	}

	IDebugRenderer & gep::Renderer::getDebugRenderer()
	{
		// TODO: insert return statement here
		return DebugRenderer();
	}
	void DebugRenderer::drawLine(const vec3 & start, const vec3 & end, Color color )
	{
	}
	void DebugRenderer::drawLine(const vec2 & start, const vec2 & end, Color color )
	{
	}
	void DebugRenderer::drawArrow(const vec3 & start, const vec3 & end, Color color )
	{
	}
	void DebugRenderer::drawBox(const vec3 & min, const vec3 & max, Color color )
	{
	}
	void DebugRenderer::printText(const vec2 & screenPositionNormalized, const char * text, Color color)
	{
	}
	void DebugRenderer::printText(const vec3 & worldPosition, const char * text, Color color )
	{
	}
	void DebugRenderer::drawLocalAxes(const vec3 & objectPosition, const Quaternion & objectRotation, float axesScale, Color colorX, Color colorY, Color colorZ )
	{
	}
	void DebugRenderer::drawLocalAxes(const vec3 & objectPosition, float axesScale, Color colorX , Color colorY, Color colorZ)
	{
	}
}

