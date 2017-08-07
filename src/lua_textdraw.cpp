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
#include "lua_textdraw.h"
#include "text_drawer.h"
#include "game_sa/CFont.h"
#include "optional.h"


namespace lua_textdraw
{
	using namespace optional;

	Textdraw::Textdraw(const std::string& text, float x, float y) :
		DisplayText(text, x, y),
		_visible(true)
	{
		TextDrawer::get()->addTextdraw(this);
	}

	Textdraw::~Textdraw()
	{
		TextDrawer::get()->removeTextdraw(this);
	}

	static void print(const std::string& text, float x, float y, opt<byte> style, opt<float> width, opt<float> height, opt<eFontAlignment> align,
						opt<float> wrap, opt<bool> proportional, opt<bool> justify, opt<CRGBA> color, opt<byte> outline, opt<byte> shadow,
						opt<CRGBA> shadowColor, opt<bool> background, opt<CRGBA> backgroundColor)
	{
		Textdraw td{text, x, y};
		optionalSet(style, td._style);
		optionalSet(width, td._width);
		optionalSet(height, td._height);
		if (align)
		{
			td._alignment = align.value();
			optionalSet(wrap, td._wordwrap);
			optionalSet(proportional, td._proportional);
			optionalSet(justify, td._justify);
			optionalSet(color, td._textColor);
			optionalSet(outline, td._outline);
			optionalSet(shadow, td._shadow);
			optionalSet(shadowColor, td._shadowColor);
			optionalSet(background, td._background);
			optionalSet(backgroundColor, td._backgroundColor);
		}
		TextDrawer::get()->push(std::move(td));
	}

	void initialize(sol::table& module)
	{
		module.set_function("draw_text", &print);
		module.new_usertype<Textdraw>("textdraw", sol::constructors<Textdraw(const std::string&, float, float)>(),
			"text", &Textdraw::_text,
			"x", &Textdraw::_x, "y", &Textdraw::_y,
			"width", &Textdraw::_width, "height", &Textdraw::_height,
			"wrap", &Textdraw::_wordwrap,
			"style", &Textdraw::_style,
			"proportional", &Textdraw::_proportional,
			"justify", &Textdraw::_justify,
			"background", &Textdraw::_background,
			"outline", &Textdraw::_outline,
			"shadow", &Textdraw::_shadow,
			"alignment", &Textdraw::_alignment,
			"visible", &Textdraw::_visible,
			"set_text_color", &Textdraw::setTextColor,
			"get_text_color", &Textdraw::getTextColor,
			"set_background_color", &Textdraw::setBackgroundColor,
			"get_background_color", &Textdraw::getBackgroundColor,
			"set_shadow_color", &Textdraw::setShadowColor,
			"get_shadow_color", &Textdraw::getShadowColor);
		module.new_enum<false>("font_style",
						"GOTHIC", FONT_GOTHIC,
						"SUBTITLES", FONT_SUBTITLES,
						"MENU", FONT_MENU,
						"PRICEDOWN", FONT_PRICEDOWN);
		module.new_enum<false>("font_align",
						"CENTER", ALIGN_CENTER,
						"LEFT", ALIGN_LEFT,
						"RIGHT", ALIGN_RIGHT);
	}
}
