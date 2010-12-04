#include "axis.h"
#include "d3dUtility.h"

Axis::Axis(IDirect3DDevice9* device)
{
	device->CreateVertexBuffer(
			4 * sizeof(ColorVertex), // size in bytes
			D3DUSAGE_WRITEONLY, // flags
			FVF_COLORVERTEX,        // vertex format
			D3DPOOL_MANAGED,    // managed memory pool
			&AxisVertex,          // return create vertex buffer
			0);                 // not used - set to 0
	ColorVertex* vertices;
	AxisVertex->Lock(0, 0, (void**)&vertices, 0);

	vertices[0] = ColorVertex(0.f, 0.0f, 0.0f, BLACK);
	vertices[1] = ColorVertex( 10.0f, 0.0f, 0.0f, RED);
	vertices[2] = ColorVertex( 0.0f, 10.0f, 0.0f, GREEN);
	vertices[3] = ColorVertex( 0.0f, 0.0f, 10.0f, BLUE);

	AxisVertex->Unlock();
	
	device->CreateIndexBuffer(
	6 * sizeof(WORD), // size in bytes
	D3DUSAGE_WRITEONLY, // flags
	D3DFMT_INDEX16,        // index format
	D3DPOOL_MANAGED,    // managed memory pool
	&AxisIndex,          // return create vertex buffer
	0);                 // not used - set to 0
	WORD* indices = 0;
	AxisIndex->Lock(0, 0, (void**)&indices, 0);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 0;
	indices[3] = 2;
	indices[4] = 0;
	indices[5] = 3;

	AxisIndex->Unlock();
}

bool Axis::draw(IDirect3DDevice9* device)
{
	device->SetStreamSource(0, AxisVertex, 0, sizeof(ColorVertex));
	device->SetFVF(FVF_COLORVERTEX);
	device->SetIndices(AxisIndex);
	device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);
	// Draw axises
	device->DrawIndexedPrimitive(D3DPT_LINELIST, 0, 0, 4, 0, 3);
	return true;
}
