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
#include "text_drawer.h"
#include "lua_textdraw.h"


void DisplayText::draw() const
{
	CFont::SetScale(_width, _height);
	CFont::SetFontStyle(_style);
	CFont::SetProp(_proportional);
	CFont::SetJustify(_justify);
	CFont::SetAlignment(_alignment);
	CFont::SetOutlinePosition(_outline);
	if (!_outline)
		CFont::SetDropShadowPosition(_shadow);
	if (_outline || _shadow)
		CFont::SetDropColor(_shadowColor);
	CFont::SetBackground(_background, false);
	if (_background)
		CFont::SetBackgroundColor(_backgroundColor);
	CFont::SetColor(_textColor);
	switch (_alignment)
	{
		case ALIGN_LEFT: CFont::SetWrapx(_wordwrap); break;
		case ALIGN_CENTER: CFont::SetCentreSize(_wordwrap); break;
		case ALIGN_RIGHT: CFont::SetRightJustifyWrap(_wordwrap); break;
	}
	CFont::PrintString(_x, _y, const_cast<char*>(_text.c_str()));
}

void TextDrawer::drawTextdraws() const
{
	for (auto& td : _textdraws)
	{
		if (td->_visible)
			td->draw();
	}
}

void TextDrawer::addTextdraw(const lua_textdraw::Textdraw* textdraw)
{
	_textdraws.push_back(textdraw);
}

void TextDrawer::removeTextdraw(const lua_textdraw::Textdraw* textdraw)
{
	_textdraws.erase(std::remove(_textdraws.begin(), _textdraws.end(), textdraw), _textdraws.end());
}
