#pragma once

#include <d3dx9.h>

struct Vertex
{
	//Vertex(){}
	Vertex(
		float x, float y, float z)
	{
		_x  = x;  _y  = y;  _z  = z;
	}
	Vertex(
		float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v)
	{
		_x  = x;  _y  = y;  _z  = z;
		_nx = nx; _ny = ny; _nz = nz;
		_u  = u;  _v  = v;
	}
	float _x, _y, _z;
    float _nx, _ny, _nz;
    float _u, _v; // texture coordinates

};
#define FVF_VERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)


struct ColorVertex
{
	ColorVertex(float _x, float _y, float _z, D3DCOLOR _color)
	{
		x = _x; y = _y; z=_z; 
		color = _color;
	}
	float x, y, z;
	D3DCOLOR color;
};
#define FVF_COLORVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)