#ifndef _DEFERSHADING_
#define _DEFERSHADING_
#include "StdAfx.h"
class dfFBO
{
public:
	static int wnd_width, wnd_height;
	GLuint _FBO;
	int FBO_width, FBO_height;
	GLuint ColorTexture[10],DepthTexture;
	bool IsLoad;
	//--------ext-------------
	GLuint VAO;
	GLuint VBO;
	bool HasQuad;
public:
	dfFBO(){_FBO=0;IsLoad=false;HasQuad=false;}
	dfFBO(int _FBO_width,int _FBO_height)
	{
		 CreateFBO(_FBO_width, _FBO_height);HasQuad=false;
	}
	bool CreateFBO(int _FBO_width,int _FBO_height)
	{
		if (IsLoad) return false;
		FBO_width=_FBO_width;FBO_height=_FBO_height;
		glGenFramebuffers(1,&_FBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER,_FBO);
		//color buffer
		glGenTextures(8,ColorTexture);
		for (int i=0;i<8;i++)
		{
		
			glBindTexture(GL_TEXTURE_2D,ColorTexture[i]);
			glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA16F,FBO_width,FBO_height,0,GL_RGBA,GL_FLOAT,NULL);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
			glBindTexture(GL_TEXTURE_2D,0);
			glFramebufferTexture(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0+i,ColorTexture[i],0);			
		}
		GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,  
			GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6, GL_COLOR_ATTACHMENT7};
		glDrawBuffers(8,DrawBuffers);
		glClearColor(0.0f,0.0f,0.0f,0.0f);

		glGenTextures(1,&DepthTexture);
		glBindTexture(GL_TEXTURE_2D,DepthTexture);
		glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT32,FBO_width,FBO_height,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_COMPARE_REF_TO_TEXTURE);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FUNC,GL_LEQUAL);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
		glBindTexture(GL_TEXTURE_2D,0);
		glFramebufferTexture(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,DepthTexture,0);
		glClearDepth(1.0f);
		glEnable(GL_DEPTH_TEST);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout<<"init FBO failed"<<std::endl;
		else std::cout<<"init FBO successs"<<std::endl;
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
		return IsLoad=true;
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
	void DrawQuad(GLuint program)
	{
		glBindVertexArray(VAO);
		glm::mat4 WVPmat=glm::ortho(0.0f, (float)FBO_width, 0.0f, (float)FBO_height, -1.0f, 1.0f);
		glUniformMatrix4fv(glGetUniformLocation(program,"WVPmat"),1,GL_FALSE,&WVPmat[0][0]);
		glDrawArrays(GL_TRIANGLE_STRIP,0,4);
		glBindTexture(GL_TEXTURE_2D,0);
		glBindVertexArray(0);
	}
	void Begin()
	{
		if (!IsLoad) return;
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER,_FBO);
		glViewport(0,0, FBO_width,FBO_height);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		glClear(GL_STENCIL_BUFFER_BIT);
	}
	void End()
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	}
	void Bilt(int index, GLuint Target)
	{
		if (!IsLoad) return;
		glBindFramebuffer(GL_READ_FRAMEBUFFER,_FBO);
		glReadBuffer(GL_COLOR_ATTACHMENT0+index);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER,Target);
		glBlitFramebuffer(0,0, FBO_width,FBO_height,0,0,wnd_width,wnd_height,GL_COLOR_BUFFER_BIT,GL_LINEAR);
	}
};
int dfFBO::wnd_width=1920;
int dfFBO::wnd_height=1080;
#endif