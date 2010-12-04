
#include "object.h"


ObjectShape::ObjectShape(){}
ObjectShape::ObjectShape(IDirect3DDevice9* device, LLVolumeFace* volumeFace)
{
	int numVertices = volumeFace->mVertices.size();
	int numIndices = volumeFace->mIndices.size();
	D3DXCreateMeshFVF(
		numIndices/3, //NumFaces，三角形个数
		numVertices, //NumVertices
		D3DXMESH_MANAGED, //Options
		FVF_VERTEX, //FVF
		device, 
		&objMesh); //产出的mesh结果

	//向空mesh填充点坐标
	Vertex* v = 0;
	objMesh->LockVertexBuffer(0, (void**)&v);

	for(int j = 0; j<numVertices; j++)
	{
		float p0 = volumeFace->mVertices[j].mPosition.mV[0];
		float p1 = volumeFace->mVertices[j].mPosition.mV[1];
		float p2 = volumeFace->mVertices[j].mPosition.mV[2];
		float n0 = volumeFace->mVertices[j].mNormal.mV[0];
		float n1 = volumeFace->mVertices[j].mNormal.mV[1];
		float n2 = volumeFace->mVertices[j].mNormal.mV[2];
		float u1 = volumeFace->mVertices[j].mTexCoord.mV[0];
		float u2 = volumeFace->mVertices[j].mTexCoord.mV[1];

		v[j] = Vertex(p0, -p1, p2, n0, n1, n2, u1, u2);
	}
	objMesh->UnlockVertexBuffer();

	//填充index
	WORD* i = 0;
	objMesh->LockIndexBuffer(0, (void**)&i);
	for(U32 j=0; j<numIndices; j++)
	{
		i[j] = volumeFace->mIndices[j];
	}
	objMesh->UnlockIndexBuffer();

	//指明哪些三角形属于哪个Subset，为贴纹理作准备，这里将所有三角形都归一个类了
	DWORD* attributeBuffer = 0;
	objMesh->LockAttributeBuffer(0, &attributeBuffer);
	for(int j=0; j<numIndices/3; j++) 
	{
		attributeBuffer[j] = 0;
	}
	objMesh->UnlockAttributeBuffer();
}


ObjectShape::~ObjectShape()
{

}

bool ObjectShape::draw(IDirect3DDevice9* device, const D3DMATERIAL9* mtrl, IDirect3DTexture9* tex)
{
	//简化处理了，应该就每个Subset设置material, texture, DrawSubset(i)
	if( mtrl )
		device->SetMaterial(mtrl);
	if( tex )
		device->SetTexture(0, tex);

	objMesh->DrawSubset(0);

	return true;
}

Object::Object(IDirect3DDevice9* device, LLVolumeFace* volumeFace, LLVector3 pos, LLQuaternion rot, LLVector3 scale)
		:ObjectShape(device, volumeFace)
{
	mPosition = pos;
	mRotation = rot;
	mScale = scale;
	LLMatrix4 llMat;
	getLocalMat4(llMat);
	LLMat2D3DMat(llMat, d3dMat);
}

bool Object::draw(IDirect3DDevice9* device, const D3DMATERIAL9* mtrl, IDirect3DTexture9* tex)
{
	device->SetTransform(D3DTS_WORLD, &d3dMat);

	ObjectShape::draw(device, mtrl, tex);
	return true;
}

void Object::LLMat2D3DMat(LLMatrix4 &llmat, D3DXMATRIX &d3dmat)
{
	for(int i=0; i<4; i++)
		for(int j=0; j<4; j++)
			d3dmat(i, j) = llmat.mMatrix[i][j];
}
