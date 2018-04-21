#ifndef _CSM_H_
#define _CSM_H_
#include "StdAfx.h"
#include <limits>
const int NUM_CASCADES=3;
const int NUM_FRUSTUM_CORNERS=8;
float inline ToRadian(float x)
{
	return x/180.0*3.1415926;
}
class CSM
{
public:
	struct OrthoProjInfo
	{	
		float r;
		float l;
		float b;
		float t;
		float n;
		float f;
	};
	OrthoProjInfo m_shadowOrthoProjInfo[NUM_CASCADES];
	float m_cascadeEnd[NUM_CASCADES+1];
	glm::vec3 LightsPos[NUM_CASCADES];
	GLuint VAO,VBO;
	glm::vec4 test[NUM_FRUSTUM_CORNERS];
public:
	CSM()
	{ 
		m_cascadeEnd[0] = 1.0;
		m_cascadeEnd[1] = 120.0f,
		m_cascadeEnd[2] = 250.0f,
		m_cascadeEnd[3] = 500.0f;
	}
	void CalcOrthoProjs(float FOV, float aspect, float nearz, float farz, glm::vec3 &EyePos, glm::vec3 &Target, glm::vec3 &Up, 
		glm::vec3 &LightPos, glm::vec3 &LightDirection, glm::vec3 LightUp)
	{ 

		m_cascadeEnd[0] = nearz;
		m_cascadeEnd[3] = farz;

		glm::mat4 Cam=glm::lookAt(EyePos,Target,Up);
		glm::mat4 CamInv=glm::inverse(Cam);
    
		float tanHalfHFOV=tanf(ToRadian(FOV*aspect)/2);
		float tanHalfVFOV=tanf(ToRadian(FOV)/2);
		 for (int i=0;i<3 ;i++) 
		{
			float xn=m_cascadeEnd[i]*tanHalfHFOV;
			float xf=m_cascadeEnd[i+1]*tanHalfHFOV;
			float yn=m_cascadeEnd[i]*tanHalfVFOV;
			float yf=m_cascadeEnd[i+1]*tanHalfVFOV;
			glm::vec4 frustumCorners[NUM_FRUSTUM_CORNERS]= 
			{
				//near
				glm::vec4(xn, yn, -m_cascadeEnd[i], 1.0),
				glm::vec4(-xn, yn,- m_cascadeEnd[i], 1.0),
				glm::vec4(xn, -yn, -m_cascadeEnd[i], 1.0),
				glm::vec4(-xn, -yn,- m_cascadeEnd[i], 1.0),
				//far
				glm::vec4(xf, yf, -m_cascadeEnd[i + 1], 1.0),
				glm::vec4(-xf, yf,- m_cascadeEnd[i + 1], 1.0),
				glm::vec4(xf, -yf, -m_cascadeEnd[i + 1], 1.0),
				glm::vec4(-xf, -yf,- m_cascadeEnd[i + 1], 1.0) 
			};
			glm::vec4 frustumCornersL[NUM_FRUSTUM_CORNERS];
			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::min();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::min();
			float minZ = std::numeric_limits<float>::max();
			float maxZ = std::numeric_limits<float>::min();
			glm::vec4 BoxInWorld[NUM_FRUSTUM_CORNERS];
			for (int j=0;j<NUM_FRUSTUM_CORNERS;j++) 
			{
				BoxInWorld[j] = CamInv * frustumCorners[j];			
				//if (i==0) test[j]=BoxInWorld[j];

				minX = min(minX, BoxInWorld[j].x);
				maxX = max(maxX, BoxInWorld[j].x);
				minY = min(minY, BoxInWorld[j].y);
				maxY = max(maxY, BoxInWorld[j].y);
				minZ = min(minZ, BoxInWorld[j].z);
				maxZ = max(maxZ, BoxInWorld[j].z);
			}
			//calc light center
			float centerX,centerY,centerZ;
			centerX=(minX+maxX)/2.0;centerY=(minY+maxY)/2.0;centerZ=(minZ+maxZ)/2.0;
			LightsPos[i]=glm::vec3(centerX,centerY,centerZ);
			
			glm::mat4 LightM=glm::lookAt(LightsPos[i], LightsPos[i]+glm::vec3(-1,-1,0), LightUp);

			minX = std::numeric_limits<float>::max();
			maxX = std::numeric_limits<float>::min();
			minY = std::numeric_limits<float>::max();
			maxY = std::numeric_limits<float>::min();
			minZ = std::numeric_limits<float>::max();
			maxZ = std::numeric_limits<float>::min();

			for (int j = 0 ; j < NUM_FRUSTUM_CORNERS ; j++) 
			{
				frustumCornersL[j] = LightM * BoxInWorld[j];
				minX = min(minX, frustumCornersL[j].x);
				maxX = max(maxX, frustumCornersL[j].x);
				minY = min(minY, frustumCornersL[j].y);
				maxY = max(maxY, frustumCornersL[j].y);
				minZ = min(minZ, frustumCornersL[j].z);
				maxZ = max(maxZ, frustumCornersL[j].z);
			}

			m_shadowOrthoProjInfo[i].l = minX;
			m_shadowOrthoProjInfo[i].r = maxX;
			m_shadowOrthoProjInfo[i].b = minY;
			m_shadowOrthoProjInfo[i].t = maxY;
			m_shadowOrthoProjInfo[i].f = maxZ;
			m_shadowOrthoProjInfo[i].n = minZ;
		}
	}
	glm::mat4 GetOrthProjMat(int i)
	{
		return glm::ortho(m_shadowOrthoProjInfo[i].l,m_shadowOrthoProjInfo[i].r,m_shadowOrthoProjInfo[i].b,
					m_shadowOrthoProjInfo[i].t,m_shadowOrthoProjInfo[i].n,m_shadowOrthoProjInfo[i].f);
	}
	void InitDraw()
	{
		glGenVertexArrays(1,&VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1,&VBO);
		glBindBuffer(GL_ARRAY_BUFFER,VBO);
		glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*24,NULL,GL_DYNAMIC_DRAW);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(const GLvoid*)0);
		glEnableVertexAttribArray(0);
		glBindVertexArray(0);
	}
	void Draw(GLuint program, int i)
	{
		glUseProgram(program);
		GLfloat Vertices[8][3]={
			{test[0].x,test[0].y,test[0].z},
			{test[1].x,test[1].y,test[1].z},
			{test[2].x,test[2].y,test[2].z},
			{test[3].x,test[3].y,test[3].z},
			{test[4].x,test[4].y,test[4].z},
			{test[5].x,test[5].y,test[5].z},
			{test[6].x,test[6].y,test[6].z},
			{test[7].x,test[7].y,test[7].z},
		};
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER,VBO);
		glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(GLfloat)*24,Vertices);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		glDrawArrays(GL_TRIANGLE_STRIP,4,4);
		glBindVertexArray(0);

	}
	void print()
	{
		std::cout<<"l r b t n f\n";
		std::cout<<m_shadowOrthoProjInfo[0].l<<std::endl;
		std::cout<<m_shadowOrthoProjInfo[0].r<<std::endl;
		std::cout<<m_shadowOrthoProjInfo[0].b<<std::endl;
		std::cout<<m_shadowOrthoProjInfo[0].t<<std::endl;
		std::cout<<m_shadowOrthoProjInfo[0].n<<std::endl;
		std::cout<<m_shadowOrthoProjInfo[0].f<<std::endl;
		std::cout<<"world\n";
		for (int i=0;i<8;i++)
			std::cout<<test[i].x<<" "<<test[i].y<<" "<<test[i].z<<" "<<std::endl;
	}
};

#endif