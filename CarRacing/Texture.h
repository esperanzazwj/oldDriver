#ifndef _TEXTURE_H_
#define _TEXTURE_H_
#include "StdAfx.h"
class Texture
{
public:
	std::string PathName;
protected:
	GLuint Tex;
	
	bool IsLoad;
public:
	Texture(){Tex=0;IsLoad=false;}
	Texture(const Texture &cpy)
	{
		Tex=cpy.Tex;IsLoad=cpy.IsLoad;
	}
	virtual GLuint GetTexture(){return Tex;};
	virtual void LoadTexture(std::string Filename)=0;
	virtual ~Texture(){}
};
class Texture2D: public Texture
{
public:
	Texture2D(){}
	Texture2D(const Texture2D &cpy):Texture(cpy){}
	Texture2D(std::string Filename)
	{
		LoadTexture(Filename);
	}
	void LoadTexture(std::string Filename)
	{
		if (IsLoad) return;
		const char *ext = &Filename.c_str()[Filename.length()-4];
		if (!strcmp(ext,".bmp") || !strcmp(ext,".BMP"))
		{
			AUX_RGBImageRec *pImage = NULL;
			pImage = auxDIBImageLoadA(Filename.c_str());
			if (pImage==NULL) return;
			glGenTextures(1,&Tex);
			glBindTexture(GL_TEXTURE_2D,Tex);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);		
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
			gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGB,pImage->sizeX,pImage->sizeY,GL_RGB,GL_UNSIGNED_BYTE,pImage->data);
			glBindTexture(GL_TEXTURE_2D,0);
			free(pImage->data);free(pImage);
		}
		else if (!strcmp(ext,".dds") || !strcmp(ext,".DDS")) //Ñ¹Ëõ¸ñÊ½µÄdds	
		{
			nv_dds::CDDSImage image;
			if(image.load(Filename))
			{
				glGenTextures(1, &Tex);
				glBindTexture(GL_TEXTURE_2D, Tex);

				glCompressedTexImage2D(GL_TEXTURE_2D, 0, image.get_format(), 
					image.get_width(), image.get_height(), 0, image.get_size(), image);

				for (int i = 0; i < image.get_num_mipmaps(); i++)
				{
					glCompressedTexImage2D(GL_TEXTURE_2D, i+1, image.get_format(), 
					image.get_mipmap(i).get_width(), image.get_mipmap(i).get_height(), 0, 
					image.get_mipmap(i).get_size(), image.get_mipmap(i));
				} 
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);		
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
				glBindTexture(GL_TEXTURE_2D,0);
			}
		}
		else {std::cout<<"Unidentified file type!";return;}
		std::cout<<"Loaded new texture "<<Filename<<" "<<Tex<<std::endl;
		PathName=Filename;
		IsLoad=true;
	}
	virtual ~Texture2D()
	{

	}
};

#endif