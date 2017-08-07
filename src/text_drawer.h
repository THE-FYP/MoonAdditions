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

#pragma once
#include "drawer.h"
#include "singleton.h"
#include "forward_declarations.h"
#include "game_sa/CFont.h"


class DisplayText
{
public:
	DisplayText(const std::string& text, float x, float y) :
		_text(text),
		_x(x), _y(y),
		_width(0.48f),
		_height(1.12f),
		_wordwrap(600.0f),
		_style(FONT_SUBTITLES),
		_proportional(true),
		_justify(false),
		_background(false),
		_outline(1),
		_shadow(0),
		_textColor(255, 255, 255, 255),
		_shadowColor(0, 0, 0, 255),
		_alignment(ALIGN_LEFT)
	{
	}
	virtual ~DisplayText() {}

	void draw() const;

public:
	std::string _text;
	float _x, _y;
	float _width, _height;
	float _wordwrap;
	byte _style;
	bool _proportional;
	bool _justify;
	bool _background;
	byte _outline;
	byte _shadow;
	CRGBA _textColor;
	CRGBA _backgroundColor;
	CRGBA _shadowColor;
	eFontAlignment _alignment;
};

class TextDrawer : public Drawer<DisplayText>, public Singleton<TextDrawer>
{
public:
	void drawTextdraws() const;
	void addTextdraw(const lua_textdraw::Textdraw* textdraw);
	void removeTextdraw(const lua_textdraw::Textdraw* textdraw);

private:
	std::vector<const lua_textdraw::Textdraw*> _textdraws;
};
