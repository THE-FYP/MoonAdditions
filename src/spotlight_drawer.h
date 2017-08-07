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


class Spotlight
{
public:
	Spotlight(unsigned int id, const CVector& origin, const CVector& target, float originRadius, float targetRadius, bool shadow, float shadowIntensity);

	void draw() const;

private:
	unsigned int _id;
	CVector _origin;
	CVector _target;
	float _originRadius;
	float _targetRadius;
	float _shadowIntensity;
	bool _shadow;
};

class SpotlightDrawer : private Drawer<Spotlight>, public Singleton<SpotlightDrawer>
{
public:
	void addSpotlight(const CVector& origin, const CVector& target, float originRadius, float targetRadius, bool shadow, float shadowIntensity)
	{
		add(_drawingQueue.size(), origin, target, originRadius, targetRadius, shadow, shadowIntensity);
	}

	using Drawer<Spotlight>::draw;
};
