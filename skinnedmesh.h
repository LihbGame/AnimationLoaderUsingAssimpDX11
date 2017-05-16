#pragma once

#ifndef SKINNED_MESH_H
#define	SKINNED_MESH_H

#include <map>
#include <vector>

#include <assimp/cimport.h>
#include <assimp/Importer.hpp>
#include <assimp/ai_assert.h>
#include <assimp/cfileio.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <assimp/IOSystem.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/LogStream.hpp>
#include <assimp/DefaultLogger.hpp>
#include "util.h"
#include "ogldev_math_3d.h"
#include <d3dx11.h>
#include "d3dx11Effect.h"
#include <xnamath.h>
#include <dxerr.h>
#include <cassert>
#include <ctime>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>

//using namespace std;

struct VertexBoneData
{
	float Weights[NUM_BONES_PER_VEREX];
	float IDs[NUM_BONES_PER_VEREX];
	VertexBoneData(const VertexBoneData& vbd)
	{
		IDs[0] = vbd.IDs[0];
		IDs[1] = vbd.IDs[1];
		IDs[2] = vbd.IDs[2];
		IDs[3] = vbd.IDs[3];

		Weights[0] = vbd.Weights[0];
		Weights[1] = vbd.Weights[1];
		Weights[2] = vbd.Weights[2];
		Weights[3] = vbd.Weights[3];
	}
	VertexBoneData()
	{
		Reset();
	};

	void Reset()
	{
		ZERO_MEM(IDs);
		ZERO_MEM(Weights);
	}

	void AddBoneData(float BoneID, float Weight);
};

struct SkinnedVertex
{
	Vector3f m_pos;
	Vector2f m_tex;
//	nv::vec3f m_pos;
//	nv::vec2f m_tex;
	//nv::vec3f m_normal;

	VertexBoneData bonedata;

	SkinnedVertex() { }
	SkinnedVertex(const Vector3f& pos, const Vector2f& tex /*const nv::vec3f& normal*/,const VertexBoneData& boneinfo)
	{
		m_pos = pos;
		m_tex = tex;
		//m_normal = normal;
		bonedata = boneinfo;
	}
};

class SkinnedMesh
{
public:

	ID3D11Device* device;

	SkinnedMesh();

	~SkinnedMesh();

	bool LoadMesh(const std::string& Filename);
	//int FindValidPath(aiString* p_szString);
//	bool TryLongerPath(char* szTemp, aiString* p_szString);
	void Render(ID3DX11Effect* mFX, ID3D11DeviceContext*& md3dImmediateContext);

	void InitBoneLocation(/*NvGLSLProgram* shader*/);
	unsigned int NumBones() const
	{
		return m_NumBones;
	}
	std::vector<Matrix4f> Transforms;
	void BoneTransform(float TimeInSeconds, std::vector<Matrix4f>& Transforms);
	bool WriteAnimInfo(const char * filename, const aiNodeAnim* animinfo);
	struct BoneInfo
	{
		Matrix4f BoneOffset;
		Matrix4f FinalTransformation;

		BoneInfo()
		{
			BoneOffset.SetZero();
			FinalTransformation.SetZero();
		}
	};

	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const Matrix4f& ParentTransform);
	bool InitSkinnedMeshFromScene(const aiScene* pScene, const std::string& Filename);
	void InitSkinnedMesh(unsigned int MeshIndex,
		const aiMesh* paiMesh);
	void LoadBones(unsigned int MeshIndex, const aiMesh* paiMesh, std::vector<VertexBoneData>& Bones);
	bool InitMaterials(const aiScene* pScene, const std::string& Filename);
	void Clear();

	//	void InitLocation();
#define INVALID_MATERIAL 0xFFFFFFFF

	enum VB_TYPES
	{
		INDEX_BUFFER,
		POS_VB,
		NORMAL_VB,
		TEXCOORD_VB,
		BONE_VB,
		NUM_VBs
	};

	struct MeshEntry
	{
		MeshEntry()
		{
			NumIndices = 0;
			MaterialIndex = INVALID_MATERIAL;
		}
		void Init(ID3D11Device* device);

		ID3D11Buffer* mVB;
		ID3D11Buffer* mIB;

		DXGI_FORMAT mIndexBufferFormat; // Always 16-bit
		UINT mVertexStride;
		std::vector<SkinnedVertex> m_Vertex;
		std::vector<unsigned int> m_Indices;
		unsigned int NumIndices;
		unsigned int MaterialIndex;
	};
	struct Texture
	{
		Texture() {};
		Texture( const std::string& FileName);
		bool Load(ID3D11Device* device, aiScene* pScene, aiMaterial* material);

		std::string m_fileName;
		//ID3D11Texture2D *pTexture = NULL;
		ID3D11ShaderResourceView* mDiffuseMapSRV;

	};
	std::vector<MeshEntry> m_Entries;
	std::vector<Texture*> m_Textures;

	std::map<std::string, unsigned int> m_BoneMapping; // maps a bone name to its index
	unsigned int m_NumBones;
	std::vector<BoneInfo> m_BoneInfo;
	Matrix4f m_GlobalInverseTransform;
	
	const aiScene* m_pScene;
	Assimp::Importer m_Importer;
};


#endif	/* OGLDEV_SKINNED_MESH_H */
