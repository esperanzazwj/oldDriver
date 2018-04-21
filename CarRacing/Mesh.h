#ifndef _MESH_H_
#define _MESH_H_
#include "StdAfx.h"
#include "type.h"
#include "Texture.h"
using namespace std;
class Mesh
{
public:
	vector<tMesh> m_Meshes;
	int NumMaterials;
	tMaterial **m_Materials;
	bool IsLoad;
public:
	Mesh():NumMaterials(0),m_Materials(NULL),IsLoad(false){}
	Mesh(string Filename)
	{
		LoadFromFile(Filename);
	}
	~Mesh()
	{
		if (NumMaterials!=0)
		{
			for (int i=0;i<NumMaterials;i++)
				delete m_Materials[i];
			delete []m_Materials;
		}
	}
	bool LoadFromFile(string Filename);
	void InitNode(const aiNode*, const aiMatrix4x4& );
	void InitMesh(int, const aiMesh*);
	bool InitMaterial(const aiScene*);
};


bool Mesh::LoadFromFile(string Filename)
{
	if (IsLoad) return false;
	Assimp::Importer importer;
	const aiScene *pScene=importer.ReadFile(Filename,aiProcess_Triangulate |aiProcess_SplitLargeMeshes
							|aiProcess_GenSmoothNormals |aiProcess_CalcTangentSpace);//| aiProcess_FlipUVs//);
							//| aiProcess_RemoveRedundantMaterials | aiProcess_PreTransformVertices | aiProcess_GenUVCoords |aiProcess_TransformUVCoords );
	if (!pScene) 
	{
		cerr<<"Cannot open file!"<<endl;
		return false;
	}
	m_Meshes.resize(pScene->mNumMeshes);
	for (unsigned int i = 0 ; i < m_Meshes.size() ; i++) 
	{
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh);
    }
	InitNode(pScene->mRootNode,aiMatrix4x4(1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f));
	return InitMaterial(pScene);
	 
};
void Mesh::InitNode(const aiNode *pNode, const aiMatrix4x4& Mat)
{
	const aiMatrix4x4 MatCur=Mat*pNode->mTransformation;
	for (int i=0;i<pNode->mNumMeshes;i++)
	{
		m_Meshes[pNode->mMeshes[i]].TransformMat=MatCur;
		m_Meshes[pNode->mMeshes[i]].Name=pNode->mName.C_Str();
	}
	for (int i=0;i<pNode->mNumChildren;i++)
		InitNode(pNode->mChildren[i],MatCur);
}
void Mesh::InitMesh(int Index, const aiMesh *paiMesh)
{
	m_Meshes[Index].MaterialIndex = paiMesh->mMaterialIndex;
	m_Meshes[Index].TransformMat=aiMatrix4x4(1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,0.0f,1.0f);
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
    for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) 
	{
        const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = paiMesh->HasNormals()? &(paiMesh->mNormals[i]) : &Zero3D;
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
		const aiVector3D* pTangent = paiMesh->HasTangentsAndBitangents()? &(paiMesh->mTangents[i]) : &Zero3D;
		const aiVector3D* pBitangent = paiMesh->HasTangentsAndBitangents()? &(paiMesh->mBitangents[i]) : &Zero3D;
		m_Meshes[Index].Vertices.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
		m_Meshes[Index].Normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
		m_Meshes[Index].TextureCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
		m_Meshes[Index].Tangents.push_back(glm::vec3(pTangent->x,pTangent->y,pTangent->z));
		m_Meshes[Index].Bitangents.push_back(glm::vec3(pBitangent->x,pBitangent->y,pBitangent->z));
    }

	for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++)
	{
        const aiFace Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        m_Meshes[Index].Indices.push_back(glm::vec3(Face.mIndices[0],Face.mIndices[1],Face.mIndices[2]));
    }
}

bool Mesh::InitMaterial(const aiScene* pScene)
{
	NumMaterials=pScene->mNumMaterials;
	m_Materials=new tMaterial*[NumMaterials];
	for (unsigned int i = 0 ; i < pScene->mNumMaterials ; i++) 
	{
		tMaterial *Material=new tMaterial;
        const aiMaterial* pMaterial = pScene->mMaterials[i];
		for (unsigned int j = 0;j < pMaterial->mNumProperties;j++)
		{
 			const aiMaterialProperty* pMaterialProperty=pMaterial->mProperties[j];
			if (pMaterialProperty->mKey==aiString(std::string("$clr.ambient")))
			{
				for (int k = 0;k < 3;k++)
				{
					memcpy(&Material->Ka[k],pMaterialProperty->mData+sizeof(float)*k,sizeof(float));
				}
			}
			else if	(pMaterialProperty->mKey==aiString(std::string("$clr.diffuse")))
			{
				for (int k = 0;k < 3;k++)
					memcpy(&Material->Kd[k],pMaterialProperty->mData+sizeof(float)*k,sizeof(float));
			}
			else if	(pMaterialProperty->mKey==aiString(std::string("$clr.specular")))
			{
				for (int k = 0;k < 3;k++)
					memcpy(&Material->Ks[k],pMaterialProperty->mData+sizeof(float)*k,sizeof(float));
			}
			else if (pMaterialProperty->mKey==aiString(std::string("$mat.shininess")))
			{
				memcpy(&Material->Shininess,pMaterialProperty->mData,sizeof(float));
			}
			else if (pMaterialProperty->mKey==aiString(std::string("$mat.opacity")))
			{
				memcpy(&Material->Opacity,pMaterialProperty->mData,sizeof(float));
			}
		}
		Material->InitDiffuseTexture(pMaterial->GetTextureCount(aiTextureType_DIFFUSE));
		for (unsigned int j=0;j<Material->NumDiffuseTexture;j++)
		{
            aiString Path;
            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				Material->LoadDiffuseTexture2D(j,Path.C_Str());

        }
		Material->InitSpecularTexture(pMaterial->GetTextureCount(aiTextureType_SPECULAR));
		for (unsigned int j=0;j<Material->NumSpecularTexture;j++)
		{
            aiString Path;
            if (pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
				Material->LoadSpecularTexture2D(j,Path.C_Str());

        }
		m_Materials[i]=Material;
	 }
	IsLoad=true;
	return true;
}

#endif