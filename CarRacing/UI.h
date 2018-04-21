#ifndef _UI_H_
#define _UI_H_
#include "StdAfx.h"
class Font
{
public:
	FT_Library library; 
	FT_Face face; 
	FT_Matrix matrix;FT_Vector pen;
	GLuint texture;
	unsigned int width,height,depth;
private:
	Font(){};
	inline int next_p2 (int a )
	{
		int rval=1;
		while(rval<a) rval<<=1;
		return rval;
	}
	void GenTexture()
	{
		width=256,height=256,depth=256;
		glGenTextures(1,&texture);
		glBindTexture(GL_TEXTURE_2D_ARRAY,texture);
		glTexImage3D(GL_TEXTURE_2D_ARRAY,0,GL_RED,width,height,depth,0,GL_RED,GL_UNSIGNED_BYTE,NULL);
		glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MIN_FILTER,GL_NEAREST);		
		glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D_ARRAY,GL_TEXTURE_WRAP_T,GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D_ARRAY,0);
	}
public:
	Font(const char *FontPath)
	{
		FT_Init_FreeType(&library);  
		FT_New_Face(library, FontPath, 0, &face);  
		FT_Select_Charmap(face,FT_ENCODING_UNICODE);
		FT_Set_Pixel_Sizes(face,256,256);
		float angle=3.14159262;
		matrix.xx = (FT_Fixed)( cos( angle ) * 0x10000L );
		matrix.xy = (FT_Fixed)(-sin( angle ) * 0x10000L );
		matrix.yx = (FT_Fixed)( sin( angle ) * 0x10000L );
		matrix.yy = (FT_Fixed)( cos( angle ) * 0x10000L );
		GenTexture();
	}

	void Bind()
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY,texture);
	}
	void Release()
	{
		glBindTexture(GL_TEXTURE_2D_ARRAY,0);
	}
	void AddFont(unsigned int i, wchar_t wChar)
	{
		FT_UInt glyph_index = FT_Get_Char_Index(face, wChar);
		FT_Set_Transform( face, &matrix, &pen );
		FT_Load_Char( face, wChar, FT_LOAD_RENDER );
		int font_width = next_p2( face->glyph->bitmap.width );
		int font_height = next_p2( face->glyph->bitmap.rows );
		GLubyte* data = new GLubyte[font_width*font_height];
		for(int j=0; j <font_height;j++) 
		{
			for(int i=0; i <font_width; i++)
			{
				data[i+j*font_width]=(i>=face->glyph->bitmap.width || j>=face->glyph->bitmap.rows) ?
					0 : face->glyph->bitmap.buffer[i + face->glyph->bitmap.width*j];
			}
		}
		glBindTexture(GL_TEXTURE_2D_ARRAY,texture);
		glTexSubImage3D(GL_TEXTURE_2D_ARRAY,0,(width-font_width)/2.0,0,i,font_width,font_height,1,GL_RED,GL_UNSIGNED_BYTE,data);
		glBindTexture(GL_TEXTURE_2D_ARRAY,0);
		delete []data;
	}
};
class FontBoard
{
private:
	Font *ft;
	float width, height, density;
	int length;
	int maxLen;	
	GLuint VAO;
	GLuint VBO;
	GLuint VBOindex;
	int x,y;
	GLuint program;
	int wnd_height,wnd_width;
	glm::vec3 color;
	glm::vec3 CommonColor;
	glm::vec3 onClickColor;
	bool OnClick;
	bool enable;
	int count;
	void (*EventFunc)();
private:
	FontBoard(){};
	void GenBoard()
	{
		count=maxLen*6;
		GLfloat *Vertices=new GLfloat[4*maxLen*2];
		GLfloat *TexCoord=new GLfloat[4*maxLen*2];
		GLfloat *CharIndex=new GLfloat[4*maxLen];
		GLuint *Index=new GLuint[6*maxLen];
		for (int i=0;i<maxLen;i++)
		{
			Vertices[i*8]=width*i-density*i;
			Vertices[i*8+1]=0.0;
			Vertices[i*8+2]=width*i-density*i;
			Vertices[i*8+3]=height;
			Vertices[i*8+4]=width*(i+1)-density*i;
			Vertices[i*8+5]=0.0;
			Vertices[i*8+6]=width*(i+1)-density*i;
			Vertices[i*8+7]=height;
			TexCoord[i*8+0]=1.0;
			TexCoord[i*8+1]=0.0;
			TexCoord[i*8+2]=1.0;
			TexCoord[i*8+3]=1.0;
			TexCoord[i*8+4]=0.0;
			TexCoord[i*8+5]=0.0;
			TexCoord[i*8+6]=0.0;
			TexCoord[i*8+7]=1.0;
			CharIndex[i*4]=CharIndex[i*4+1]=CharIndex[i*4+2]=CharIndex[i*4+3]=i;
			Index[i*6]=4*i;
			Index[i*6+1]=4*i+1;
			Index[i*6+2]=4*i+2;
			Index[i*6+3]=4*i+1;
			Index[i*6+4]=4*i+2;
			Index[i*6+5]=4*i+3;
		}
		glGenVertexArrays(1,&VAO);
		glBindVertexArray(VAO);
		glGenBuffers(1,&VBO);
		glBindBuffer(GL_ARRAY_BUFFER,VBO);
		glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*16*maxLen+sizeof(GLfloat)*4*maxLen,NULL,GL_STATIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(GLfloat)*8*maxLen,Vertices);
		glBufferSubData(GL_ARRAY_BUFFER,sizeof(GLfloat)*8*maxLen,sizeof(GLfloat)*8*maxLen,TexCoord);
		glBufferSubData(GL_ARRAY_BUFFER,sizeof(GLfloat)*16*maxLen,sizeof(GLfloat)*4*maxLen,CharIndex);
		glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,(const GLvoid*)0);
		glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(const GLvoid*)(sizeof(GLfloat)*8*maxLen));
		glVertexAttribPointer(2,1,GL_FLOAT,GL_FALSE,0,(const GLvoid*)(sizeof(GLfloat)*16*maxLen));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glGenBuffers(1,&VBOindex);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,VBOindex);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(GLuint)*6*maxLen,Index,GL_STATIC_DRAW);
		glBindVertexArray(0);
		delete Vertices;delete TexCoord;delete Index;
	}
	static wchar_t* CharToWchar(const char* c)  
	{  
		wchar_t *m_wchar;
		int len = MultiByteToWideChar(CP_ACP,0,c,strlen(c),NULL,0);  
		m_wchar=new wchar_t[len+1];  
		MultiByteToWideChar(CP_ACP,0,c,strlen(c),m_wchar,len);  
		m_wchar[len]='\0';  
		return m_wchar;  
	}  
public:
	FontBoard(Font *_ft, float _width, float _height, float _density, int _maxLen, GLuint _program, int _wnd_width, int _wnd_height)
	{
		ft=_ft;
		width=_width;height=_height;density=_density;maxLen=_maxLen;
		x=y=0;length=0;
		program=_program;wnd_width=_wnd_width;wnd_height=_wnd_height;
		CommonColor=glm::vec3(1.0,1.0,0.0);onClickColor=glm::vec3(1.0,1.0,1.0);
		color=CommonColor;
		EventFunc=NULL;OnClick=false;enable=true;
		GenBoard();
	}
	void SetColor(glm::vec3 c)
	{
		color=c;
	}
	void SetOnClickColor(glm::vec3 c)
	{
		onClickColor=c;
	}
	void SetPosition(int _x, int _y)
	{
		x=_x;y=_y;
	}
	void SetScreen(int _wnd_width, int _wnd_height)
	{
		wnd_width=_wnd_width;wnd_height=_wnd_height;
	}
	void SetEventFunc(void (*_EventFunc)())
	{
		EventFunc=_EventFunc;
	}
	void SetEnable(bool state)
	{
		enable=state;
	}
	static void GetAllAscllChar(Font *ft)
	{
		for (int i=0;i<128;i++)
		{
			char ch=i;
			wchar_t *wch=CharToWchar(&ch);
			ft->AddFont(i,wch[0]);
			delete wch;
		}
	}
	void Render(const char *text)
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);
		glUseProgram(program);
		glm::mat4 WVPmat=glm::ortho(0.0f, (float)wnd_width, 0.0f, (float)wnd_height, -1.0f, 1.0f)*glm::translate(glm::mat4(1.0f),glm::vec3(x,y,0.0f));
		glUniformMatrix4fv(glGetUniformLocation(program,"WVPmat"),1,GL_FALSE,&WVPmat[0][0]);
		length=strlen(text);
		int *data=new int[length];
		for (int i=0;i<length;i++)
			data[i]=text[i];
		glUniform1iv(glGetUniformLocation(program,"text"),length,data);
		glUniform1i(glGetUniformLocation(program,"Length"),length);
		glUniform1f(glGetUniformLocation(program,"SingleCharWidth"),width);
		glUniform3fv(glGetUniformLocation(program,"Color"),1,&color[0]);
		glBindVertexArray(VAO);ft->Bind();
		glDrawElements(GL_TRIANGLES,count,GL_UNSIGNED_INT,(const void*)0);
		glBindVertexArray(0);ft->Release();
		delete data;
		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}
	void OnMouseDown(int xx, int yy)
	{
		yy=wnd_height-yy;
		if (xx<x || xx>x+length*width-length*density|| yy<y || yy>y+height)
			return;
		OnClick=true;
		color=onClickColor; 
	}
	void OnMouseUp(int xx, int yy)
	{
		color=CommonColor; 
		if (OnClick==true && enable)
			if (EventFunc!=NULL)
				EventFunc();
		OnClick=false;
	}
	~FontBoard()
	{
	}
};

void EndGame()
{
	glutLeaveMainLoop();
}
void FullScreen()
{
	glutFullScreen();
}
#endif