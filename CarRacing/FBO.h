#ifndef _FBO_H_
#define _FBO_H_
#include "StdAfx.h"
class FBO
{
public:
	static int wnd_width, wnd_height;
	GLuint _FBO;
	int FBO_width, FBO_height;
	GLuint ColorTexture,DepthTexture,StencilTexture;
	bool bColor,bDepth,bStencil;
	bool IsLoad;
	//--------ext-------------
	GLuint VAO;
	GLuint VBO;
	bool HasQuad;
public:
	FBO(){_FBO=0;IsLoad=false;HasQuad=false;}
	FBO(int _FBO_width,int _FBO_height,bool HasColor, bool HasDepth,bool HasStencil)
	{
		CreateFBO(_FBO_width,_FBO_height,HasColor,HasDepth,HasStencil);
	}
	bool CreateFBO(int _FBO_width,int _FBO_height,bool HasColor, bool HasDepth,bool HasStencil)
	{
		if (IsLoad) return false;
		FBO_width= _FBO_width;FBO_height=_FBO_height;
		bColor=HasColor;bDepth=HasDepth;bStencil=HasStencil;
		glGenFramebuffers(1,&_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER,_FBO);
		if (HasColor)
		{
			glGenTextures(1,&ColorTexture);
			glBindTexture(GL_TEXTURE_2D,ColorTexture);
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16F,FBO_width,FBO_height,0,GL_RGBA,GL_FLOAT,NULL);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D,0);
			glFramebufferTexture(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,ColorTexture,0);
			glDrawBuffer(GL_COLOR_ATTACHMENT0);
		}
		else glDrawBuffer(GL_NONE);
		
		if (HasDepth)
		{
			glGenTextures(1,&DepthTexture);
			glBindTexture(GL_TEXTURE_2D,DepthTexture);
			glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,FBO_width,FBO_height,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_COMPARE_REF_TO_TEXTURE);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FUNC,GL_LEQUAL);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D,0);
			glFramebufferTexture(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,DepthTexture,0);
			glClearDepth(1.0f);
			glEnable(GL_DEPTH_TEST);
		}
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout<<"init FBO failed"<<std::endl;
		else std::cout<<"init FBO successs"<<std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER,0);
		return IsLoad=true;
	}
	GLuint GetColorTexture()
	{
		return ColorTexture;
	}
	bool CreateQuad()
	{
		if (HasQuad) return false;
		GLfloat Vertices[4][2]={
			{0.0,0.0},
			{0.0,FBO_height},
			{FBO_width,0.0},
			{FBO_width,FBO_height}
		};
		GLfloat TexCoord[4][2]={
			{0.0,0.0},
			{0.0,1.0},
			{1.0,0.0},
			{1.0,1.0}
		};
		glGenVertexArrays(1,&VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1,&VBO);
		glBindBuffer(GL_ARRAY_BUFFER,VBO);
		glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*16,NULL,GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(GLfloat)*8,Vertices);
		glBufferSubData(GL_ARRAY_BUFFER,sizeof(GLfloat)*8,sizeof(GLfloat)*8,TexCoord);
		glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,(const GLvoid*)0);
		glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(const GLvoid*)sizeof(Vertices));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER,0);glBindVertexArray(0);
		
		return (HasQuad=true);
	}
	void DrawToneMapping(GLuint program, GLuint g_BlurTexture, float g_fBlurAmount, float g_fRadialEffectAmount, float g_fExposure)
	{
		glUseProgram(program);
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,ColorTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,g_BlurTexture);
		glm::mat4 WVPmat=glm::ortho(0.0f, (float)FBO_width, 0.0f, (float)FBO_height, -1.0f, 1.0f);
		glUniformMatrix4fv(glGetUniformLocation(program,"WVPmat"),1,GL_FALSE,&WVPmat[0][0]);
		glUniform1i(glGetUniformLocation(program,"g_SceneTexture"),0);
		glUniform1i(glGetUniformLocation(program,"g_BlurTexture"),1);
		glUniform1f(glGetUniformLocation(program,"g_fBlurAmount"),g_fBlurAmount);
		glUniform1f(glGetUniformLocation(program,"g_fRadialEffectAmount"),g_fRadialEffectAmount);
		glUniform1f(glGetUniformLocation(program,"g_fExposure"),g_fExposure);
		//glViewport(0,0, FBO_width,FBO_height);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		glBindTexture(GL_TEXTURE_2D,0);
		glBindVertexArray(0);

	}
	void DrawQuadGuass(GLuint program, bool ifWidth)
	{
		int Len;
		glUseProgram(program);
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,ColorTexture);
		glm::mat4 WVPmat=glm::ortho(0.0f, (float)FBO_width, 0.0f, (float)FBO_height, -1.0f, 1.0f);
			//glm::lookAt(glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,0.0f,-1.0f),glm::vec3(0.0f,1.0f,0.0f));
		glUniformMatrix4fv(glGetUniformLocation(program,"WVPmat"),1,GL_FALSE,&WVPmat[0][0]);
		glUniform1i(glGetUniformLocation(program,"tex2D"),0);
		if (ifWidth) Len=FBO_width; else Len=FBO_height;
		glUniform1i(glGetUniformLocation(program,"ifWidth"),ifWidth);
		glUniform1i(glGetUniformLocation(program,"Len"),Len);
		//glViewport(0,0, FBO_width,FBO_height);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		glBindTexture(GL_TEXTURE_2D,0);
		glBindVertexArray(0);
	}
	void DrawQuad(GLuint program)
	{
		glUseProgram(program);
		glBindVertexArray(VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,ColorTexture);
		glm::mat4 WVPmat=glm::ortho(0.0f, (float)FBO_width, 0.0f, (float)FBO_height, -1.0f, 1.0f);
			//glm::lookAt(glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,0.0f,-1.0f),glm::vec3(0.0f,1.0f,0.0f));
		glUniformMatrix4fv(glGetUniformLocation(program,"WVPmat"),1,GL_FALSE,&WVPmat[0][0]);
		glUniform1i(glGetUniformLocation(program,"tex2D"),0);
		//glViewport(0,0, FBO_width,FBO_height);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		glBindTexture(GL_TEXTURE_2D,0);
		glBindVertexArray(0);

	}
	void Begin()
	{
		if (!IsLoad) return;
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER,_FBO);
		//glViewport(0,0,wnd_width,wnd_height);
		glViewport(0,0, FBO_width,FBO_height);
		if (bColor)
			glClear(GL_COLOR_BUFFER_BIT);
		if (bDepth)
			glClear(GL_DEPTH_BUFFER_BIT);
		if (bStencil)
			glClear(GL_STENCIL_BUFFER_BIT);

	}
	void End()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	}
	void Bilt(GLuint Target)
	{
		if (!IsLoad) return;
		glBindFramebuffer(GL_READ_FRAMEBUFFER,_FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER,Target);
		//glViewport(0,0,wnd_width,wnd_height);
		//glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
		glBlitFramebuffer(0,0, FBO_width,FBO_height,0,0,wnd_width,wnd_height,GL_COLOR_BUFFER_BIT,GL_LINEAR);
	}
};
int FBO::wnd_width=800;
int FBO::wnd_height=600;
#endif