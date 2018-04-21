#ifndef _COMPILER_H_
#define _COMPILER_H_
#include "StdAfx.h"
std::string GetShader(const char* Filename)
{
	std::fstream fs(Filename);
	std::stringstream ss;
	ss << fs.rdbuf();
	return ss.str(); 
}
GLuint glGenProgram(const char* vShader, const char* tcShader, const char* teShader, const char* gShader, const char* fShader)
{
	GLuint program = glCreateProgram();
    GLuint shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shader,1,&vShader,NULL);
	glCompileShader(shader);
	GLint compiled;
	glGetShaderiv(shader,GL_COMPILE_STATUS,&compiled);
	if (compiled!=GL_TRUE)
	{
		char infolog[1000];
		glGetShaderInfoLog(shader,sizeof(infolog),NULL,infolog);
		std::cout<<"vertex shader error"<<std::endl<<infolog<<std::endl;;
	}
	else std::cout<<"vertex shader compile sucess"<<std::endl;
	glAttachShader(program,shader);

	if (tcShader!=NULL)
	{
		shader = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(shader,1,&tcShader,NULL);
		glCompileShader(shader);
		glGetShaderiv(shader,GL_COMPILE_STATUS,&compiled);
		if (compiled!=GL_TRUE)
		{
			char infolog[1000];
			glGetShaderInfoLog(shader,sizeof(infolog),NULL,infolog);
			std::cout<<"tess control shader error"<<std::endl<<infolog<<std::endl;;
		}
		else std::cout<<"tess control shader compile sucess"<<std::endl;
		glAttachShader(program,shader);
	}

	if (teShader!=NULL)
	{
		shader = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(shader,1,&teShader,NULL);
		glCompileShader(shader);
		glGetShaderiv(shader,GL_COMPILE_STATUS,&compiled);
		if (compiled!=GL_TRUE)
		{
			char infolog[1000];
			glGetShaderInfoLog(shader,sizeof(infolog),NULL,infolog);
			std::cout<<"tess evaluate shader error"<<std::endl<<infolog<<std::endl;;
		}
		else std::cout<<"tess evaluate shader compile sucess"<<std::endl;
		glAttachShader(program,shader);
	}

	if (gShader!=NULL)
	{
		shader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(shader,1,&gShader,NULL);
		glCompileShader(shader);
		glGetShaderiv(shader,GL_COMPILE_STATUS,&compiled);
		if (compiled!=GL_TRUE)
		{
			char infolog[1000];
			glGetShaderInfoLog(shader,sizeof(infolog),NULL,infolog);
			std::cout<<"geometry shader error"<<std::endl<<infolog<<std::endl;;
		}
		else std::cout<<"geometry shader compile sucess"<<std::endl;
		glAttachShader(program,shader);
	}

	shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shader,1,&fShader,NULL);
	glCompileShader(shader);
	glGetShaderiv(shader,GL_COMPILE_STATUS,&compiled);
	if (compiled!=GL_TRUE)
	{
		char infolog[1000];
		glGetShaderInfoLog(shader,sizeof(infolog),NULL,infolog);
		std::cout<<"fragment shader error"<<std::endl<<infolog<<std::endl;
	}
	else std::cout<<"fragment shader compile sucess"<<std::endl;
	glAttachShader(program,shader);
	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program,GL_LINK_STATUS,&linked);
	if (linked!=GL_TRUE)
	{
		char infolog[1000];
		glGetProgramInfoLog(program,sizeof(infolog),NULL,infolog);
		std::cout<<"link error"<<std::endl<<infolog<<std::endl;
	}
	else std::cout<<"program link sucess"<<std::endl;
	return program;
}
//------------------------------------Shader---------------------------------------//
const char* skycube_vshader={
	"#version 400 core\n"
	"uniform mat4 MVPmat;"
	"layout(location=0) in vec4 vPosition;"
	"out vec3 tex_coord;"
	"void main()"
	"{"
	"gl_Position=MVPmat*vPosition;"
	"tex_coord=vPosition.xyz;"
	"}"
};
const char* skycube_fshader={
	"#version 400 core\n"
	"in vec3 tex_coord;"
	"layout(location=0) out vec4 fcolor;"
	"uniform samplerCube tex;"
	"void main()"
	"{"
	"fcolor=texture(tex,tex_coord);"
	"}"
};

const char* Basic_vShader={
	"#version 400 core\n"
	"uniform mat4 MVmat;"
	"uniform mat4 MVPmat;"
	"uniform mat3 Normalmat;"
	"uniform mat4 ShadowMat[3];"
	"layout(location=0) in vec3 VertexPosition;"
	"layout(location=1) in vec3 VertexNormal;"
	"layout(location=2) in vec2 VertexTexCoord;"
	"out vec4 Position;"
	"out vec2 TexCoord;"
	"out vec3 Normal;"
	"out vec4 LightSpacePos[3];"
	"out float ClipSpacePosZ;"
	"void main()"
	"{"
		"vec4 VertexPosition4=vec4(VertexPosition,1.0);"
		"Position=MVmat*VertexPosition4;"
		"TexCoord=VertexTexCoord;"
		"Normal=normalize(Normalmat*VertexNormal);"
		"for (int i=0;i<3;i++)"
		"LightSpacePos[i]=ShadowMat[i]*vec4(VertexPosition,1.0);"
		"gl_Position=MVPmat*VertexPosition4;"
		"ClipSpacePosZ = gl_Position.z;"
	"}"
};
const char* Basic_fShader={
	"#version 400 core\n"
	"struct LightProperties\n"
	"{\n"
		"bool isEnabled;\n"
		"bool isLocal;\n"
		"bool isSpot;\n"
		"vec3 ambient;\n"
		"vec3 color;\n"
		"vec3 position;\n"
		"vec3 coneDirection;\n"
		"float spotCosCutoff;\n"
		"float spotExponent;\n"
		"float constantAttenuation;\n"
		"float linearAttenuation;\n"
		"float quadraticAttenuation;\n"
	"};\n"
	"const int MaxLights=10;\n"
	"const int MaxDiffuseTextures=2;\n"
	"const int MaxSpecularTextures=2;\n"
	"uniform int NumLights;\n"
	"uniform int NumDiffuseTextures;\n"
	"uniform int NumSpecularTextures;\n"
	"uniform LightProperties Lights[MaxLights];\n"
	"uniform float Shininess;\n"
	"uniform float Strength;\n"
	"uniform vec3 EyePosition;\n"
	"uniform vec3 Ka;\n"
	"uniform vec3 Kd;\n"
	"uniform vec3 Ks;\n"
	"uniform float Opacity;\n"
	"uniform float EnvFactor;\n"
	"uniform bool HasShadow;\n"
	"uniform float CascadeEndClipSpace[3];\n"
	"in vec4 Position;\n"
	"in vec3 Normal;\n"
	"in vec2 TexCoord;\n"
	"in vec4 LightSpacePos[3];\n"
	"in float ClipSpacePosZ ;"
	"layout(location=0) out vec4 FragColor;\n"
	"uniform sampler2D DiffuseTexture[MaxDiffuseTextures];\n"
	"uniform sampler2D SpecularTexture[MaxSpecularTextures];\n"
	"uniform sampler2DShadow DepthTexture[3];"
	"uniform samplerCube EnvTexture;"
	"float CalcShadowFactor(int index, vec4 _LightSpacePos)"
	"{"
	"const float EPSILON=0.002;"
    "vec3 ProjCoords = _LightSpacePos.xyz / _LightSpacePos.w;"
    "vec2 UVCoords;"
    "UVCoords.x = 0.5 * ProjCoords.x + 0.5;"
    "UVCoords.y = 0.5 * ProjCoords.y + 0.5;"
    "float z = 0.5 * ProjCoords.z + 0.5;"
    //"float xOffset = 1.0/gMapSize.x;"
    //"float yOffset = 1.0/gMapSize.y;"
	//"float xOffset = 1.0/4096.0;"
   // "float yOffset = 1.0/4096.0;"
   	"float xOffset = 1.0/1920.0;"
	"float yOffset = 1.0/1080.0;"
    "float Factor = 0.0;"
    "for (int y = -1 ; y <= 1 ; y++) {"
        "for (int x = -1 ; x <= 1 ; x++) {"
            "vec2 Offsets = vec2(x * xOffset, y * yOffset);"
            "vec3 UVC = vec3(UVCoords + Offsets, z - EPSILON);"
			" Factor += texture(DepthTexture[index], UVC);"
       "}"
    "}"
    "return Factor / 9.0;"
	//"return 0.0;"
	"}" 
	"void main()\n"
	"{\n"
		"vec4 tColor=texture(DiffuseTexture[0],TexCoord);\n"
		"if (tColor.a<=0.0001) discard;"
		"vec3 ambientLight=vec3(0.0,0.0,0.0);"
		"vec3 scatteredLight=vec3(0.0,0.0,0.0);\n"
		"vec3 reflectedLight=vec3(0.0,0.0,0.0);\n"
		"for (int i=0;i<NumLights;i++)\n"
		"{\n"
			"if (!Lights[i].isEnabled) continue;\n"
			"vec3 halfVector;\n"
			"vec3 lightDirection=Lights[i].position;\n"
			"vec3 EyeDirection=EyePosition-Position.xyz;\n"
			"float attenuation=1.0;\n"
			"if (Lights[i].isLocal)\n"
			"{\n"
				"lightDirection=lightDirection-Position.xyz;\n"
				"float lightDistance=length(lightDirection);\n"
				"lightDirection=lightDirection/lightDistance;\n"
				"attenuation=1.0/(Lights[i].constantAttenuation+Lights[i].linearAttenuation*lightDistance+Lights[i].quadraticAttenuation*lightDistance*lightDistance);\n"
				"if (Lights[i].isSpot)\n"
				"{\n"
					"float spotCos=dot(lightDirection,-Lights[i].coneDirection);\n"
					"if (spotCos<Lights[i].spotCosCutoff)\n"
						"attenuation=0.0;\n"
					"else attenuation*=pow(spotCos,Lights[i].spotExponent);\n"
				"}\n"
				"halfVector=normalize(lightDirection+EyeDirection);"
			"}\n"
			"else halfVector=normalize(lightDirection+EyeDirection);\n"
			"float diffuse=max(0.0,dot(Normal,lightDirection));\n"
			"float specular=max(0.0,dot(Normal,halfVector));\n"
			"if (diffuse==0.0 || Shininess==0.0)\n"
				"specular=0.0;\n"
			"else\n"
				"specular=pow(specular,Shininess);\n"
			"ambientLight+=Ka*Lights[i].color*Lights[i].ambient*attenuation;"
			"scatteredLight+=Kd*Lights[i].color*diffuse*attenuation;\n"
			"reflectedLight+=Ks*Lights[i].color*specular*attenuation;\n"
		"}\n"
		"vec4 DiffuseColor=vec4(0.0,0.0,0.0,0.0);\n"
		"if (NumDiffuseTextures>0)\n"
			"for (int i=0;i<NumDiffuseTextures;i++)\n"
				"DiffuseColor+=texture(DiffuseTexture[i],TexCoord);\n"
		"else\n"
			"DiffuseColor=vec4(1.0,1.0,1.0,0.0);\n"
		"vec4 ambientColor=DiffuseColor;"
		"vec4 SpecularColor=vec4(0.0,0.0,0.0,0.0);\n"
		"if (NumSpecularTextures>0)\n"
			"for (int i=0;i<NumSpecularTextures;i++)\n"
				"SpecularColor+=texture(SpecularTexture[i],TexCoord);"
		"else\n"
			"SpecularColor=vec4(1.0,1.0,1.0,0.0);\n"
		"vec3 EnvTexCoord=reflect(-EyePosition+Position.xyz,Normal);"
		"vec4 EnvColor=texture(EnvTexture,EnvTexCoord);"
		
		"float shadow=1.0;\n"
		"if (HasShadow)"
		"{"
			"for (int i = 0 ; i < 3 ; i++)"
			"if ( ClipSpacePosZ <= CascadeEndClipSpace[i])" 
				"{"
					"shadow = CalcShadowFactor(i, LightSpacePos[i]);"
					"break;"
				"}"
		 "}"
		 "vec3 ambientColorMixed=mix(ambientColor.rgb,EnvColor.rgb,EnvFactor);"
		 "vec3 DiffuseColorMixed=mix(DiffuseColor.rgb,EnvColor.rgb,EnvFactor);"
		 "vec3 SpecularColorMixed=mix(SpecularColor.rgb,EnvColor.rgb,EnvFactor);"
		 "vec3 rgb=DiffuseColorMixed*scatteredLight+SpecularColorMixed.rgb*reflectedLight;"
		"FragColor=vec4(ambientColorMixed.rgb*ambientLight,0)+shadow*vec4(rgb,Opacity);\n"
	"}"
};
const char* show_vShader={
	"#version 400 core\n"
	"uniform mat4 MVPmat;"
	"layout(location=0) in vec3 vPosition;"
	"layout(location=2) in vec2 tex_coord_v;"
	"out vec2 tex_coord;"
	"void main()"
	"{"
	"gl_Position=MVPmat*vec4(vPosition,1.0);"
	"tex_coord=tex_coord_v;"
	"}"
};
const char *show_fShader={
	"#version 400 core\n"
	"in vec2 tex_coord;"
	"layout(location=0) out vec4 fcolor;"
	"uniform sampler2D tex2D;"
	"void main()"
	"{"
	"fcolor=texture(tex2D,tex_coord);"
	"}"
};
const char* shadow_vShader={
	"#version 400 core\n"
	"uniform mat4 MVPmat;"
	"layout(location=0) in vec4 vPosition;"
	"layout(location=2) in vec2 vTexCoord;"
	"out vec2 TexCoord;"
	"void main()"
	"{"
	"gl_Position=MVPmat*vPosition;"
	"TexCoord=vTexCoord;"
	"}"
};
const char* shadow_fShader={
	"#version 400 core\n"
	"in vec2 TexCoord;"
	"layout(location=0) out vec4 fcolor;"
	"uniform sampler2D tex2D;"
	"void main()"
	"{"
	"vec4 tColor=texture(tex2D,TexCoord);\n"
	"if (tColor.a<=0.0001) discard;"
	"fcolor=vec4(1.0);"
	"}"
};


const char *test_vShader={
	"#version 400 core\n"
	"uniform mat4 MVPmat;"
	"layout(location=0) in vec4 vPosition;"
	//"layout(location=1) in vec2 tex_coord_v;"
	//"out vec2 tex_coord;"
	"void main()"
	"{"
	"gl_Position=MVPmat*vPosition;"
	//"tex_coord=tex_coord_v;"
	"}"
};
const char *test_fShader={
	"#version 400 core\n"
	//"in vec2 tex_coord;"
	"layout(location=0) out vec4 fcolor;"
	"uniform sampler2D tex2D;"
	"void main()"
	"{"
	//"fcolor=texture(tex2D,tex_coord);"
	"fcolor=vec4(1,0,0,0);"
	"}"
};

const char *downsampler_vShader={
	"#version 400 core\n"
	"uniform mat4 WVPmat;"
	"layout(location=0) in vec2 Position;"
	"layout(location=1) in vec2 TexCoord_v;"
	"out vec2 TexCoord_f;"
	"void main()"
	"{"
	"gl_Position=WVPmat*vec4(Position,0.0,1.0);"
	"TexCoord_f=TexCoord_v;"
	"}"
};

const char *downsampler_fShader={
	"#version 400 core\n"
	"in vec2 TexCoord_f;"
	"layout(location=0) out vec4 fcolor;"
	"uniform sampler2D tex2D;"
	"void main()"
	"{"
	"fcolor=texture(tex2D,TexCoord_f);"
	//"fcolor=vec4(0,1,0,0);"
	"}"	
};
const char *fontboard_vShader={
	"#version 400 core\n"
	"uniform mat4 WVPmat;"
	"layout(location=0) in vec2 Position_v;"
	"layout(location=1) in vec2 TexCoord_v;"
	"layout(location=2) in float CharIndex_v;"
	"out vec2 TexCoord_f;"
	"out float Position_f;"
	"out float CharIndex_f;"
	"void main()"
	"{"
	"gl_Position=WVPmat*vec4(Position_v,0.0,1.0);"
	"TexCoord_f=TexCoord_v;"
	"Position_f=Position_v.x;"
	"CharIndex_f=CharIndex_v;"
	"}"
};
const char *fontboard_fShader={
	"#version 400 core\n"
	"const int maxLen=100;"
	"in vec2 TexCoord_f;"
	"in float Position_f;"
	"in float CharIndex_f;"
	"layout(location=0) out vec4 fcolor;"
	"uniform sampler2DArray tex2DArray;"
	"uniform int text[maxLen];"
	"uniform int Length;"
	"uniform float SingleCharWidth;"
	"uniform vec3 Color;"
	"void main()"
	"{"
	"int CharPos=int(CharIndex_f);"
	"vec4 font=vec4(0,0,0,0);"
	"if (CharPos<Length)"
	"font=texture(tex2DArray,vec3(TexCoord_f,text[CharPos]));"
	"fcolor=vec4(Color*font.r,font.r);"
	"}"
};

const char* particle_vshader={
	"#version 400 core\n"
	"layout(location=0) in vec3 vPosition;"
	"layout(location=1) in vec3 vVelocity;"
	"layout(location=2) in float vType;"
	"layout(location=3) in float vAge;"
	"out vec3 Position;"
	"out vec3 Velocity;"
	"out float Type;"
	"out float Age;"
	"void main()"
	"{"
		"Position=vPosition;"
		"Velocity=vVelocity;"
		"Type=vType;"
		"Age=vAge;"
	"}"
};
const char* particle_gshader={
	"#version 400 core\n"
	"#define PARTICLE_LAUNCHER 0.0 \n"
	"#define PARTICLE_SHELL 1.0 \n"
	"#define PARTICLE_SHELL_SEC 2.0 \n"
	"layout(points) in;\n"
	"layout(points,max_vertices=100) out;\n"
	"in vec3 Position[];\n"
	"in vec3 Velocity[];\n"
	"in float Type[];\n"
	"in float Age[];\n"
	"out vec3 NewPosition;\n"
	"out vec3 NewVelocity;\n"
	"out float NewType;\n"
	"out float NewAge;\n"
	"out vec3 color;\n"
	"uniform bool stop;"
	"uniform vec3 DirectionY;"
	"uniform vec3 DirectionX;"
	"uniform vec3 DirectionZ;"
	"uniform mat4 MVPmat;\n"
	"uniform float LauncherLifeTime;\n"
	"uniform float ShellLifeTime;\n"
	"uniform float ShellSecLifeTime;\n"
	"uniform float Interval;\n" 
	"uniform float RandSeed;\n"
	"float random(float x, float y)"
	"{"
		"return fract(cos(x * (12.9898) + y * (4.1414)) * 43758.5453);"
	"}"
	"vec3 GetNewVelocity(float x, float y)\n"
	"{\n"
		"vec3 Dir;"
		"Dir.x=random(fract(x/31.0),fract(y/31.0))*2.0-1.0;"
		"Dir.y=random(fract(x/59.0),fract(y/59.0))*2.0+5.0;"
		"Dir.z=random(fract(x/79.0),fract(y/79.0))*2.0-1.0;"
		"return DirectionX*Dir.x+DirectionY*Dir.y+DirectionZ*Dir.z;"
	"}\n"
	"void main()\n"
	"{\n"
		"vec3 VelocityNext=Velocity[0]+DirectionY*(-5.0)*Interval;\n"
		"vec3 PositionNext=Position[0]+VelocityNext*Interval;\n"
		"float AgeNext=Age[0]+Interval;\n"
		"if (Type[0]==PARTICLE_LAUNCHER)\n"
		"{\n"
			"if (AgeNext>LauncherLifeTime && !stop)\n"
			"{\n"
				"for (int i=0;i<50;i++){"
				"NewPosition=Position[0];\n"
				"NewVelocity=GetNewVelocity(fract(RandSeed/17.0),i/50.0);\n"
				"NewType=PARTICLE_SHELL;\n"
				"NewAge=0.0;\n"
				"gl_Position=MVPmat*vec4(NewPosition,1.0);\n"
				"gl_PointSize=20;"
				"color=vec3(0.607,0.031,0.976);"
				"EmitVertex();\n"
				"EndPrimitive();\n"
				"}"
			"}\n"
			"NewPosition=Position[0];\n"
			"NewVelocity=Velocity[0];\n"
			"NewType=Type[0];\n"
			"NewAge=AgeNext;\n"
			"gl_Position=MVPmat*vec4(NewPosition,1.0);\n"
			"gl_PointSize=10;"
			"color=vec3(0.0,0.0,0.0);"
			"EmitVertex();\n"
			"EndPrimitive();\n"
		"}\n"
		"else if (Type[0]==PARTICLE_SHELL)\n"
		"{\n"
			"if (AgeNext>ShellLifeTime)\n"
				"for (int i=0;i<10;i++)\n"
				"{\n"
					"NewPosition=Position[0];\n"
					"NewVelocity=GetNewVelocity(RandSeed,i);\n"
					"NewType=PARTICLE_SHELL_SEC;\n"
					"NewAge=0.0;\n"	
					"gl_Position=MVPmat*vec4(NewPosition,1.0);\n"
					"gl_PointSize=10;"
					"color=vec3(0.24,0.18,0.98);"
					"EmitVertex();\n"
					"EndPrimitive()\n;"
				"}\n"
			"else\n"
			"{\n"
				"NewPosition=PositionNext;\n"
				"NewVelocity=VelocityNext;\n"
				"NewType=Type[0];\n"
				"NewAge=AgeNext;\n"
				"gl_Position=MVPmat*vec4(NewPosition,1.0);\n"
				"gl_PointSize=20;"
				"color=vec3(0.607,0.031,0.976);"
				"EmitVertex();\n"
				"EndPrimitive();\n"				
			"}\n"
		"}\n"
		"else if (AgeNext<ShellSecLifeTime)\n"
		"{\n"
			"NewPosition=PositionNext;\n"
			"NewVelocity=VelocityNext;\n"
			"NewType=Type[0];\n"
			"NewAge=AgeNext;\n"
			"gl_Position=MVPmat*vec4(NewPosition,1.0);\n"
			"gl_PointSize=10;"
			"color=vec3(0.24,0.18,0.98);"
			"EmitVertex();\n"
			"EndPrimitive();\n"
		"}\n"
	"}\n"
};
const char* particle_fshader={
	"#version 400 core\n"
	"in vec3 color;"
	"out vec4 FragColor;"
	"uniform sampler2D TexSprite;"
	"void main()"
	"{"
	"vec2 tmp=gl_PointCoord-vec2(0.5);"
	"float f=dot(tmp,tmp);"
	"if (f>0.25) discard;"
	//"FragColor=texture(TexSprite,gl_PointCoord);"
	"FragColor=(1-texture(TexSprite,gl_PointCoord).r)*vec4(mix(color,vec3(0.8,0.5,0.1),f),1.0);"
	"}"
};

#endif