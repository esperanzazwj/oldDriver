#ifndef _TYPE_H_
#define _TYPE_H_
#include "StdAfx.h"
#include "Texture.h"
typedef struct Mesh_tag
{
	std::string Name;
	int MaterialIndex;
	aiMatrix4x4 TransformMat;
	std::vector<glm::vec3> Vertices;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec2> TextureCoords;
	std::vector<glm::vec3> Tangents;
	std::vector<glm::vec3> Bitangents;
	std::vector<glm::uvec3> Indices;
} tMesh;
class tMaterial
{
public:
	float Ka[3];
	float Kd[3];
	float Ks[3];
	float Shininess;
	float Opacity;
	unsigned int NumDiffuseTexture;
	Texture2D **DiffuseTexture;
	unsigned int NumSpecularTexture;
	Texture2D **SpecularTexture;
public:
	tMaterial():NumDiffuseTexture(0),NumSpecularTexture(0),Shininess(0),Opacity(1){}
	tMaterial(const tMaterial &cpy)
	{
		for (int i=0;i<3;i++)
		{
			Ka[i]=cpy.Ka[i];Kd[i]=cpy.Kd[i];Ks[i]=cpy.Ks[i];
		}
		Shininess=cpy.Shininess;
		NumDiffuseTexture=cpy.NumDiffuseTexture;
		DiffuseTexture=new Texture2D*[NumDiffuseTexture];
		for (int i=0;i<NumDiffuseTexture;i++) 
			DiffuseTexture[i]=new Texture2D(*cpy.DiffuseTexture[i]);
		NumSpecularTexture=cpy.NumSpecularTexture;
		SpecularTexture=new Texture2D*[NumSpecularTexture];
		for (int i=0;i<NumSpecularTexture;i++)
			SpecularTexture[i]=new Texture2D(*cpy.SpecularTexture[i]);
	}
	void InitDiffuseTexture(int num)
	{
		ClearDiffuseTexture();
		NumDiffuseTexture=num;
		DiffuseTexture=new Texture2D*[NumDiffuseTexture];
		for (int i=0;i<NumDiffuseTexture;i++) DiffuseTexture[i]=NULL;
	}
	bool LoadDiffuseTexture2D(int index, std::string Filename)
	{
		if (DiffuseTexture[index]!=NULL) return false;
		DiffuseTexture[index]=new Texture2D(Filename);
		return true;
	}
	void InitSpecularTexture(int num)
	{
		ClearSpecularTexture();
		NumSpecularTexture=num;
		SpecularTexture=new Texture2D*[NumSpecularTexture];
		for (int i=0;i<NumSpecularTexture;i++) SpecularTexture[i]=NULL;
	}
	bool LoadSpecularTexture2D(int index, std::string Filename)
	{
		if (SpecularTexture[index]!=NULL) return false;
		SpecularTexture[index]=new Texture2D(Filename);
		return true;
	}
private:
	void ClearDiffuseTexture()
	{
		for (int i=0;i<NumDiffuseTexture;i++)
			if (DiffuseTexture[i]!=NULL)
				delete DiffuseTexture[i];
		if (NumDiffuseTexture!=0) delete []DiffuseTexture;
		NumDiffuseTexture=0;
	}
	void ClearSpecularTexture()
	{
		for (int i=0;i<NumSpecularTexture;i++)
			if (SpecularTexture[i]!=NULL)
				delete SpecularTexture[i];
		if (NumSpecularTexture!=0) delete []SpecularTexture;
		NumSpecularTexture=0;
	}
};
#endif