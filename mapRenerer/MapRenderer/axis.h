#pragma once

#include <d3dx9.h>
#include "vertex.h"
#include "color.h"

class Axis
{
public:
	Axis(IDirect3DDevice9* device);
	bool draw(IDirect3DDevice9* device);
private:
	IDirect3DVertexBuffer9* AxisVertex;
	IDirect3DIndexBuffer9* AxisIndex;
};