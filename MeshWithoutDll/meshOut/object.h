
#pragma once
#include <d3dx9.h>
#include <string>
#include "llvolume.h"
#include "vertex.h"
#include "m4math.h"


class ObjectShape
{
public:
	ObjectShape(IDirect3DDevice9* device, LLVolumeFace* volumeFace);
	ObjectShape();
	~ObjectShape();

	bool draw(IDirect3DDevice9* device, const D3DMATERIAL9* mtrl, IDirect3DTexture9* tex);
protected:
	//LLVolume*								 _volume;
	//std::vector<LLVolume::Point>	_mMesh;
	ID3DXMesh*							objMesh;
	//S32										numFaces;
	//int										numVertices;
	//int										numIndices;
};

class Object: ObjectShape
{
public:
	Object(IDirect3DDevice9* device, LLVolumeFace* volumeFace, LLVector3 pos, LLQuaternion rot, LLVector3 scale);
	bool draw(IDirect3DDevice9* device, const D3DMATERIAL9* mtrl, IDirect3DTexture9* tex);
	void getLocalMat4(LLMatrix4 &mat) const { mat.initAll(mScale, mRotation, mPosition); }
	void LLMat2D3DMat(LLMatrix4 &llmat, D3DXMATRIX &d3dmat);

private:
	LLVector3	  mPosition;
	LLQuaternion  mRotation; 
	LLVector3	  mScale;
	D3DXMATRIX d3dMat;
};
