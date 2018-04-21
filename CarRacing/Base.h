#ifndef _BASE_H_
#define _BASE_H_
#include "StdAfx.h"
#include "type.h"
#include "collision.h"
class Base
{
public:
	GLuint VAO;
	GLuint Buffers[2];
	GLuint basic_program;
	GLuint count;
	std::string objName;
	tMaterial *pMaterial;
	glm::mat4 transformMat;
	bool HasIndices;
	bool IsLoad;
	OBB *obb;
public:	
	Base():VAO(0),basic_program(0),count(0),IsLoad(false),obb(NULL)
	{
		Buffers[0]=Buffers[1]=0;
	}
	Base(tMesh &Mesh, tMaterial **Materials, GLuint program, bool GenOBB)
	{
		Load(Mesh,Materials,program,GenOBB);
	}
	void Load(tMesh &Mesh, tMaterial **Materials, GLuint program, bool GenOBB)
	{
		if (IsLoad) return;
		this->basic_program=program;
		objName=Mesh.Name;
		pMaterial=Materials[Mesh.MaterialIndex];
		for (int i=0;i<4;i++)
			for (int j=0;j<4;j++)
				this->transformMat[i][j]=Mesh.TransformMat[j][i];
		GLuint VSize=Mesh.Vertices.size()*3*sizeof(GLfloat);
		GLuint NSize=Mesh.Normals.size()*3*sizeof(GLfloat);
		GLuint TSize=Mesh.TextureCoords.size()*2*sizeof(GLfloat);
		GLuint ISize=Mesh.Indices.size()*3*sizeof(GLuint);
		GLuint TGSize=Mesh.Tangents.size()*3*sizeof(GLfloat);
		GLuint BTGSize=Mesh.Bitangents.size()*3*sizeof(GLfloat);
		count=Mesh.Indices.size()*3;HasIndices=true;
		glUseProgram(program);
		glGenVertexArrays(1,&VAO);
		glBindVertexArray(VAO);
		glGenBuffers(2,Buffers);
		glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
		//glBufferData(GL_ARRAY_BUFFER,VSize+NSize+TSize+TGSize+BTGSize,NULL,GL_STATIC_DRAW);
		glBufferData(GL_ARRAY_BUFFER,VSize+NSize+TSize,NULL,GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER,0,VSize,(const void*)&Mesh.Vertices[0]);
		glBufferSubData(GL_ARRAY_BUFFER,VSize,NSize,(const void*)&Mesh.Normals[0]);
		glBufferSubData(GL_ARRAY_BUFFER,VSize+NSize,TSize,(const void*)&Mesh.TextureCoords[0]);
		//glBufferSubData(GL_ARRAY_BUFFER,VSize+NSize+TSize,TGSize,(const void*)&Mesh.Tangents[0]);
		//glBufferSubData(GL_ARRAY_BUFFER,VSize+NSize+TSize+TGSize,BTGSize,(const void*)&Mesh.Bitangents[0]);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(const GLvoid*)0);
		glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(const GLvoid*)VSize);
		glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,0,(const GLvoid*)(VSize+NSize));
		//glVertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,0,(const GLvoid*)(VSize+NSize+TSize));
		//glVertexAttribPointer(4,3,GL_FLOAT,GL_FALSE,0,(const GLvoid*)(VSize+NSize+TSize+TGSize));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		//glEnableVertexAttribArray(3);
		//glEnableVertexAttribArray(4);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,Buffers[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,ISize,(const void*)&Mesh.Indices[0],GL_STATIC_DRAW);
		if (GenOBB)
			obb=new OBB(Mesh,objName);
		else obb=NULL;
		IsLoad=true;
	}
	void InitMaterial(const tMaterial* pMaterial)
	{
		glUniform3fv(glGetUniformLocation(basic_program,"Ka"),1,pMaterial->Ka);
		glUniform3fv(glGetUniformLocation(basic_program,"Kd"),1,pMaterial->Kd);
		glUniform3fv(glGetUniformLocation(basic_program,"Ks"),1,pMaterial->Ks);
		glUniform1f(glGetUniformLocation(basic_program,"Shininess"),pMaterial->Shininess);
		glUniform1f(glGetUniformLocation(basic_program,"Opacity"),pMaterial->Opacity);
		glUniform1i(glGetUniformLocation(basic_program,"NumDiffuseTextures"),pMaterial->NumDiffuseTexture);
		char TextureName[100];
		for (int i=0;i<pMaterial->NumDiffuseTexture;i++)
		{
			glActiveTexture(GL_TEXTURE0+i);
			glBindTexture(GL_TEXTURE_2D,pMaterial->DiffuseTexture[i]->GetTexture());
			sprintf(TextureName,"DiffuseTexture[%d]\0",i);
			glUniform1i(glGetUniformLocation(basic_program,TextureName),i);
		}
		glUniform1i(glGetUniformLocation(basic_program,"NumSpecularTextures"),pMaterial->NumSpecularTexture);
		for (int i=0;i<pMaterial->NumSpecularTexture;i++)
		{
			glActiveTexture(GL_TEXTURE0+pMaterial->NumSpecularTexture+i);
			glBindTexture(GL_TEXTURE_2D,pMaterial->SpecularTexture[i]->GetTexture());
			sprintf(TextureName,"SpecularTexture[%d]\0",i);
			glUniform1i(glGetUniformLocation(basic_program,TextureName),pMaterial->NumSpecularTexture+i);
		}
	}
	void Render()
	{
		if (!IsLoad) return;
		//glUseProgram(basic_program);
		glBindVertexArray(VAO);
		if (HasIndices)
			glDrawElements(GL_TRIANGLES,count,GL_UNSIGNED_INT,(const void*)0);
		else glDrawArrays(GL_TRIANGLES,0,count);
	}
	~Base()
	{
		if (obb!=NULL) delete obb;
	}
};

#endif