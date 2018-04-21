#ifndef _COLLISION_H_
#define _COLLISION_H_
#include "StdAfx.h"
#include "type.h"
#include "eigval.h"
class OBB
{
public:
	glm::vec3 center;
	glm::vec3 AxisX;
	glm::vec3 AxisY;
	glm::vec3 AxisZ;
	glm::vec3 center_cur;
	glm::vec3 AxisX_cur;
	glm::vec3 AxisY_cur;
	glm::vec3 AxisZ_cur;
	glm::mat4 TransMat;
	glm::vec3 Normal;
	float LenX,LenY,LenZ;
	float LenX_cur,LenY_cur,LenZ_cur;
	GLuint VAO;
	GLuint VAO_Axis;
	GLuint VBO_Axis;
	GLuint Buffers[2];
public:
	OBB(const tMesh &Mesh, std::string &name)
	{
		center=glm::vec3(0.0f,0.0f,0.0f);
		for (int i=0;i<Mesh.Vertices.size();i++)
			center+=Mesh.Vertices[i];
		center/=Mesh.Vertices.size();
		if (name.substr(0,3)=="Box")
		{
			const float eps=1e-3;
			for (int i=0;i<Mesh.Indices.size();i++)
			{
			glm::vec3 line0=Mesh.Vertices[Mesh.Indices[i].x]-Mesh.Vertices[Mesh.Indices[i].y];
			glm::vec3 line1=Mesh.Vertices[Mesh.Indices[i].x]-Mesh.Vertices[Mesh.Indices[i].z];
			glm::vec3 line2=Mesh.Vertices[Mesh.Indices[i].y]-Mesh.Vertices[Mesh.Indices[i].z];
			if (fabs(glm::dot(line0,line1))<eps)
			{
				AxisX=glm::normalize(line0);
				AxisY=glm::normalize(line1);
				AxisZ=glm::normalize(glm::cross(line0,line1));
				break;
			}
			else if (fabs(glm::dot(line0,line2))<eps)
			{
				AxisX=glm::normalize(line0);
				AxisY=glm::normalize(line2);
				AxisZ=glm::normalize(glm::cross(line0,line2));
				break;
			}
			else if (fabs(glm::dot(line1,line2))<eps)
			{
				AxisX=glm::normalize(line1);
				AxisY=glm::normalize(line2);
				AxisZ=glm::normalize(glm::cross(line1,line2));
				break;
			}
			}
		}
		else
		{
			glm::mat3 var(0.0f);
			for (int i=0;i<Mesh.Vertices.size();i++)
			{
				glm::vec3 t=Mesh.Vertices[i]-center;
				var+=glm::outerProduct(t,t);
			}
			var/=(Mesh.Vertices.size()-1);
			MATRIX VarMat(3,3),EigvecMat(3,3);
			for (int i=0;i<3;i++)
				for (int j=0;j<3;j++)
					VarMat[i+1][j+1]=var[j][i];
			eig_Jacobi(VarMat,EigvecMat);
			AxisX=glm::normalize(glm::vec3(EigvecMat[1][1],EigvecMat[1][2],EigvecMat[1][3]));
			AxisY=glm::normalize(glm::vec3(EigvecMat[2][1],EigvecMat[2][2],EigvecMat[2][3]));
			AxisZ=glm::normalize(glm::vec3(EigvecMat[3][1],EigvecMat[3][2],EigvecMat[3][3]));
		}
		LenX=LenY=LenZ=0;
		for (int i=0;i<Mesh.Vertices.size();i++)
		{
			float t;
			t=fabs(glm::dot(Mesh.Vertices[i]-center,AxisX));
			if (t>LenX) LenX=t;
			t=fabs(glm::dot(Mesh.Vertices[i]-center,AxisY));
			if (t>LenY) LenY=t;
			t=fabs(glm::dot(Mesh.Vertices[i]-center,AxisZ));
			if (t>LenZ) LenZ=t;
		}


		float t=LenX;Normal=AxisX;
		if (LenY<t){t=LenY;Normal=AxisY;}
		if (LenZ<t){t=LenZ;Normal=AxisZ;}
		Normal;

		TransMat=glm::mat4(1.0f);
		center_cur=center;
		AxisX_cur=AxisX;AxisY_cur=AxisY;AxisZ_cur=AxisZ;
		LenX_cur=LenX;LenY_cur=LenY;LenZ_cur=LenZ;
	}
	void InitDrawAxis()
	{
		glm::vec3 x0=center+AxisX*20.0f;
		glm::vec3 x1=center-AxisX*20.0f;
		glm::vec3 y0=center+AxisY*20.0f;
		glm::vec3 y1=center-AxisY*20.0f;
		glm::vec3 z0=center+AxisZ*20.0f;
		glm::vec3 z1=center-AxisZ*20.0f;
		GLfloat Vertices[6][3]={
			{x0[0],x0[1],x0[2]},
			{x1[0],x1[1],x1[2]},
			{y0[0],y0[1],y0[2]},
			{y1[0],y1[1],y1[2]},
			{z0[0],z0[1],z0[2]},
			{z1[0],z1[1],z1[2]},
		};
		glGenVertexArrays(1,&VAO_Axis);
		glBindVertexArray(VAO_Axis);
		glGenBuffers(1,&VBO_Axis);
		glBindBuffer(GL_ARRAY_BUFFER,VBO_Axis);
		glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*18,(const void*)Vertices,GL_STATIC_DRAW);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(const GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}
	void InitDraw()
	{
		glm::vec3 v000=center+AxisX*LenX+AxisY*LenY+AxisZ*LenZ;
		glm::vec3 v001=center+AxisX*LenX+AxisY*LenY-AxisZ*LenZ;
		glm::vec3 v010=center+AxisX*LenX-AxisY*LenY+AxisZ*LenZ;
		glm::vec3 v011=center+AxisX*LenX-AxisY*LenY-AxisZ*LenZ;
		glm::vec3 v100=center-AxisX*LenX+AxisY*LenY+AxisZ*LenZ;
		glm::vec3 v101=center-AxisX*LenX+AxisY*LenY-AxisZ*LenZ;
		glm::vec3 v110=center-AxisX*LenX-AxisY*LenY+AxisZ*LenZ;
		glm::vec3 v111=center-AxisX*LenX-AxisY*LenY-AxisZ*LenZ;
		GLfloat Vertices[8][3]={
			{v000[0],v000[1],v000[2]},
			{v001[0],v001[1],v001[2]},
			{v010[0],v010[1],v010[2]},
			{v011[0],v011[1],v011[2]},
			{v100[0],v100[1],v100[2]},
			{v101[0],v101[1],v101[2]},
			{v110[0],v110[1],v110[2]},
			{v111[0],v111[1],v111[2]},
		};
		GLuint Indices[12][2]={
			{0,1},
			{2,3},
			{0,2},
			{1,3},
			{4,5},
			{6,7},
			{4,6},
			{5,7},
			{4,0},
			{6,2},
			{5,1},
			{7,3}
		};
		glGenVertexArrays(1,&VAO);
		glBindVertexArray(VAO);
		glGenBuffers(2,Buffers);
		glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
		glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*24,(const void*)Vertices,GL_STATIC_DRAW);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(const GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,Buffers[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint)*24,(const void*)Indices,GL_STATIC_DRAW);
		glBindVertexArray(0);
	}
	void InitDrawTransform()
	{
		glm::vec3 v000=center_cur+AxisX_cur*LenX_cur+AxisY_cur*LenY_cur+AxisZ_cur*LenZ_cur;
		glm::vec3 v001=center_cur+AxisX_cur*LenX_cur+AxisY_cur*LenY_cur-AxisZ_cur*LenZ_cur;
		glm::vec3 v010=center_cur+AxisX_cur*LenX_cur-AxisY_cur*LenY_cur+AxisZ_cur*LenZ_cur;
		glm::vec3 v011=center_cur+AxisX_cur*LenX_cur-AxisY_cur*LenY_cur-AxisZ_cur*LenZ_cur;
		glm::vec3 v100=center_cur-AxisX_cur*LenX_cur+AxisY_cur*LenY_cur+AxisZ_cur*LenZ_cur;
		glm::vec3 v101=center_cur-AxisX_cur*LenX_cur+AxisY_cur*LenY_cur-AxisZ_cur*LenZ_cur;
		glm::vec3 v110=center_cur-AxisX_cur*LenX_cur-AxisY_cur*LenY_cur+AxisZ_cur*LenZ_cur;
		glm::vec3 v111=center_cur-AxisX_cur*LenX_cur-AxisY_cur*LenY_cur-AxisZ_cur*LenZ_cur;
		GLfloat Vertices[8][3]={
			{v000[0],v000[1],v000[2]},
			{v001[0],v001[1],v001[2]},
			{v010[0],v010[1],v010[2]},
			{v011[0],v011[1],v011[2]},
			{v100[0],v100[1],v100[2]},
			{v101[0],v101[1],v101[2]},
			{v110[0],v110[1],v110[2]},
			{v111[0],v111[1],v111[2]},
		};
		glBindBuffer(GL_ARRAY_BUFFER,Buffers[0]);
		glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(GLfloat)*24,Vertices);
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}
	void Draw()
	{
		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,Buffers[1]);
		glDrawElements(GL_LINES,24,GL_UNSIGNED_INT,(const void*)0);
	}
	void DrawAxis()
	{
		glBindVertexArray(VAO_Axis);
		glDrawArrays(GL_LINES,0,18);
	}
	void SetTrans(const glm::mat4 &transMat)
	{
		TransMat=transMat;
		glm::vec4 tmp;glm::vec3 tmp3;
		tmp=TransMat*glm::vec4(AxisX,0.0f)*LenX;
		tmp3=glm::vec3(tmp.x,tmp.y,tmp.z);
		AxisX_cur=glm::normalize(tmp3);
		LenX_cur=glm::dot(AxisX_cur,tmp3);

		tmp=TransMat*glm::vec4(AxisY,0.0f)*LenY;
		tmp3=glm::vec3(tmp.x,tmp.y,tmp.z);
		AxisY_cur=glm::normalize(tmp3);
		LenY_cur=glm::dot(AxisY_cur,tmp3);

		tmp=TransMat*glm::vec4(AxisZ,0.0f)*LenZ;
		tmp3=glm::vec3(tmp.x,tmp.y,tmp.z);
		AxisZ_cur=glm::normalize(tmp3);
		LenZ_cur=glm::dot(AxisZ_cur,tmp3);

		tmp=TransMat*glm::vec4(Normal,0.0f);
		tmp3=glm::vec3(tmp.x,tmp.y,tmp.z);
		Normal=glm::normalize(tmp3);

		tmp=TransMat*glm::vec4(center,1.0f);
		center_cur=glm::vec3(tmp.x,tmp.y,tmp.z);
	}
	bool intersect(const OBB &obb)
	{
		glm::vec3 SeparateAxis;
		SeparateAxis=AxisX_cur;
		if (!SeparateAxisTest(SeparateAxis,obb)) //1-6
			return false;
		SeparateAxis=AxisY_cur;
		if (!SeparateAxisTest(SeparateAxis,obb))
			return false;
		SeparateAxis=AxisZ_cur;
		if (!SeparateAxisTest(SeparateAxis,obb))
			return false;
		SeparateAxis=obb.AxisX_cur;
		if (!SeparateAxisTest(SeparateAxis,obb))
			return false;
		SeparateAxis=obb.AxisY_cur;
		if (!SeparateAxisTest(SeparateAxis,obb))
			return false;
		SeparateAxis=obb.AxisZ_cur;
		if (!SeparateAxisTest(SeparateAxis,obb))
			return false;
		SeparateAxis=glm::cross(AxisX_cur,obb.AxisX_cur); //7-15
		if (!SeparateAxisTest(SeparateAxis,obb))
			return false;
		SeparateAxis=glm::cross(AxisX_cur,obb.AxisY_cur);
		if (!SeparateAxisTest(SeparateAxis,obb))
			return false;
		SeparateAxis=glm::cross(AxisX_cur,obb.AxisZ_cur);
		if (!SeparateAxisTest(SeparateAxis,obb))
			return false;
		SeparateAxis=glm::cross(AxisY_cur,obb.AxisX_cur);
		if (!SeparateAxisTest(SeparateAxis,obb))
			return false;
		SeparateAxis=glm::cross(AxisY_cur,obb.AxisY_cur);
		if (!SeparateAxisTest(SeparateAxis,obb))
			return false;
		SeparateAxis=glm::cross(AxisY_cur,obb.AxisZ_cur);
		if (!SeparateAxisTest(SeparateAxis,obb))
			return false;
		SeparateAxis=glm::cross(AxisZ_cur,obb.AxisX_cur);
		if (!SeparateAxisTest(SeparateAxis,obb))
			return false;
		SeparateAxis=glm::cross(AxisZ_cur,obb.AxisY_cur);
		if (!SeparateAxisTest(SeparateAxis,obb))
			return false;
		SeparateAxis=glm::cross(AxisZ_cur,obb.AxisZ_cur);
		if (!SeparateAxisTest(SeparateAxis,obb))
			return false;
		return true;
	}
	bool SeparateAxisTest(const glm::vec3 &SeparateAxis, const OBB &obb)
	{
		float dist,r1,r2;
		dist=fabs(glm::dot(SeparateAxis,center_cur-obb.center_cur));
		r1=0;//unnecessary detection for test 
		for (int i=-1;i<=1;i+=2)
			for (int j=-1;j<=1;j+=2)
				for (int k=-1;k<=1;k+=2)
				{
					float t=fabs(glm::dot(SeparateAxis,AxisX_cur*LenX_cur*float(i)+
						AxisY_cur*LenY_cur*float(j)+AxisZ_cur*LenZ_cur*float(k)));
					if (t>r1) r1=t;
				}
		r2=0;
		for (int i=-1;i<=1;i+=2)
			for (int j=-1;j<=1;j+=2)
				for (int k=-1;k<=1;k+=2)
				{
					float t=fabs(glm::dot(SeparateAxis,obb.AxisX_cur*obb.LenX_cur*float(i)+
						obb.AxisY_cur*obb.LenY_cur*float(j)+obb.AxisZ_cur*obb.LenZ_cur*float(k)));
					if (t>r2) r2=t;
				}
		//r2=fabs(glm::dot(SeparateAxis,obb.AxisX_cur*obb.LenX+obb.AxisY_cur*obb.LenY+obb.AxisZ_cur*obb.LenZ));
		//std::cout<<"dist: "<<dist<<"r1: "<<r1<<"r2: "<<r2<<std::endl;
		if (dist>=r1+r2) return false; else return true;
	}
};
#endif