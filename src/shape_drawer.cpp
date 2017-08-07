// MIT License

// Copyright (c) 2012 DK22Pac
// Copyright (c) 2017 FYP

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "pch.h"
#include "shape_drawer.h"
#include "lua_texture.h"
#include "lua_shape.h"


void Shape::draw() const
{
	auto shape = _shape.lock();
	if (shape)
	{
		auto tex = _texture.lock();
		RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATETEXTURERASTER, tex ? tex->getRwTexture()->raster : nullptr);
		RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)_enableAlpha);
		RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATESRCBLEND, (void*)_srcBlend);
		RwEngineInstance->dOpenDevice.fpRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)_dstBlend);
		auto vertices = const_cast<RwIm2DVertex*>(static_cast<const RwIm2DVertex*>(shape->getVertices().data()));
		RwEngineInstance->dOpenDevice.fpIm2DRenderPrimitive(_primType, vertices, shape->getVertices().size());
	}
}

void ShapeDrawer::drawShapes()
{
	if (!_drawingQueue.empty())
	{
		RenderStateData data;
		storeRenderStates(data);
		Drawer<Shape>::draw();
		restoreRenderStates(data);
	}
}

void ShapeDrawer::storeRenderStates(RenderStateData& rsData) const
{
	auto rsGet = RwEngineInstance->dOpenDevice.fpRenderStateGet;
	rsGet(rwRENDERSTATETEXTURERASTER, &rsData._TEXTURERASTER);
	rsGet(rwRENDERSTATEVERTEXALPHAENABLE, &rsData._VERTEXALPHAENABLE);
	rsGet(rwRENDERSTATESRCBLEND, &rsData._SRCBLEND);
	rsGet(rwRENDERSTATEDESTBLEND, &rsData._DESTBLEND);
}

void ShapeDrawer::restoreRenderStates(const RenderStateData& rsData)
{
	auto rsSet = RwEngineInstance->dOpenDevice.fpRenderStateSet;
	rsSet(rwRENDERSTATETEXTURERASTER, rsData._TEXTURERASTER);
	rsSet(rwRENDERSTATEVERTEXALPHAENABLE, rsData._VERTEXALPHAENABLE);
	rsSet(rwRENDERSTATESRCBLEND, rsData._SRCBLEND);
	rsSet(rwRENDERSTATEDESTBLEND, rsData._DESTBLEND);
}
