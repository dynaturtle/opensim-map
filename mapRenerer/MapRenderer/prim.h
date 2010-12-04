#pragma once

#include "logError.h"
#include <d3dx9.h>
#include <string>
#include "../llvolume/llvolume.h"
#include "vertex.h"
#include "../llcommon/m4math.h"
#include "../llprimitive/lltextureentry.h"
#include "../llcommon/stdtypes.h"


class PrimMesh
{
public:
	PrimMesh(IDirect3DDevice9* device, LLVolumeFace* volumeFace);

	bool draw(IDirect3DDevice9* device);

protected:
	ID3DXMesh*	primMesh;
};

class Prim: PrimMesh
{
public:
	Prim(IDirect3DDevice9* device, 
		LLVolumeFace* volumeFace, 
		LLVector3 pos, 
		LLQuaternion rot, 
		LLVector3 scale,
		char *textureName);
	bool draw(IDirect3DDevice9* device);
	void getLocalMat4(LLMatrix4 &mat) const { mat.initAll(mScale, mRotation, mPosition); }
	void LLMat2D3DMat(LLMatrix4 &llmat, D3DXMATRIX &d3dmat);

	inline void setPosition(const LLVector3& pos);
	inline void setPosition(const F32 x, const F32 y, const F32 z);
	inline void setPositionX(const F32 x);
	inline void setPositionY(const F32 y);
	inline void setPositionZ(const F32 z);
	inline void addPosition(const LLVector3& pos);


	inline void setScale(const LLVector3& scale);
	inline void setScale(const F32 x, const F32 y, const F32 z);
	inline void setRotation(const LLQuaternion& rot);
	inline void setRotation(const F32 x, const F32 y, const F32 z);
	inline void setRotation(const F32 x, const F32 y, const F32 z, const F32 s);

private:
	LLVector3	  mPosition;
	LLQuaternion  mRotation; 
	LLVector3	  mScale;
	D3DXMATRIX d3dMat;
	U8 material;
	char *textureName;
};
