#ifndef _LIGHTS_H_
#define _LIGHTS_H_
#include "StdAfx.h"

const int MaxLights=10;
int NumLights=0;

struct LightProperties
{
	bool isEnabled;
	bool isLocal;
	bool isSpot;
	glm::vec3 ambient;
	glm::vec3 color;
	glm::vec3 position;
	glm::vec3 halfVector;
	glm::vec3 coneDirection;
	float spotCosCutoff;
	float spotExponent;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
} Lights[MaxLights];

void initLights()
{
	NumLights=3;

	Lights[0].isEnabled=true;
	Lights[0].isLocal=false;
	Lights[0].isSpot=false;
	Lights[0].ambient=glm::vec3(0.5f,0.5f,0.5f);
	Lights[0].color=glm::vec3(1.0f,1.0f,1.0f);
	Lights[0].position=glm::vec3(1.0f,1.0f,0.0f);
	Lights[0].halfVector=glm::vec3(0.0f,0.0f,0.0f);
	Lights[0].coneDirection=glm::vec3(-1.0f,-1.0f,0.0f);;
	Lights[0].spotCosCutoff=0;
	Lights[0].spotExponent=0;
	Lights[0].constantAttenuation=1.0f;
	Lights[0].linearAttenuation=0.0f;
	Lights[0].quadraticAttenuation=0.0f;

	Lights[1].isEnabled=true;
	Lights[1].isLocal=true;
	Lights[1].isSpot=true;
	Lights[1].ambient=glm::vec3(0.0f,0.0f,0.0f);
	Lights[1].color=glm::vec3(1.0f,1.0f,1.0f);
	Lights[1].position=glm::vec3(0.0f,100.0f,0.0f);
	Lights[1].halfVector=glm::vec3(0.0f,0.0f,0.0f);
	Lights[1].coneDirection=glm::normalize(glm::vec3(0.0f,-1.0f,0.0f));
	Lights[1].spotCosCutoff=0;
	Lights[1].spotExponent=1;
	Lights[1].constantAttenuation=1.0f;
	Lights[1].linearAttenuation=0.0f;
	Lights[1].quadraticAttenuation=0.0f;
	
	Lights[2].isEnabled=false;
	Lights[2].isLocal=true;
	Lights[2].isSpot=true;
	Lights[2].ambient=glm::vec3(0.0f,0.0f,0.0f);
	Lights[2].color=glm::vec3(1.0f,1.0f,1.0f);
	Lights[2].position=glm::vec3(0.0f,10.0f,900.0f);
	Lights[2].halfVector=glm::vec3(0.0f,0.0f,0.0f);
	Lights[2].coneDirection=glm::vec3(0.0f,0.0f,-1.0f);
	Lights[2].spotCosCutoff=0.9;
	Lights[2].spotExponent=100;
	Lights[2].constantAttenuation=1.0f;
	Lights[2].linearAttenuation=0.0f;
	Lights[2].quadraticAttenuation=0.0f;

}
void LoadLightsInfo(GLuint basic_program)
{
	glUniform1i(glGetUniformLocation(basic_program,"NumLights"),NumLights);
	char LightsPropertiesName[100];
	for (int i=0;i<NumLights;i++)
	{
		sprintf(LightsPropertiesName,"Lights[%d].isEnabled\0",i);
		glUniform1i(glGetUniformLocation(basic_program,LightsPropertiesName),Lights[i].isEnabled);
		sprintf(LightsPropertiesName,"Lights[%d].isLocal\0",i);
		glUniform1i(glGetUniformLocation(basic_program,LightsPropertiesName),Lights[i].isLocal);
		sprintf(LightsPropertiesName,"Lights[%d].isSpot\0",i);
		glUniform1i(glGetUniformLocation(basic_program,LightsPropertiesName),Lights[i].isSpot);
		sprintf(LightsPropertiesName,"Lights[%d].ambient\0",i);
		glUniform3fv(glGetUniformLocation(basic_program,LightsPropertiesName),1,&Lights[i].ambient[0]);
		sprintf(LightsPropertiesName,"Lights[%d].color\0",i);
		glUniform3fv(glGetUniformLocation(basic_program,LightsPropertiesName),1,&Lights[i].color[0]);
		sprintf(LightsPropertiesName,"Lights[%d].position\0",i);
		glUniform3fv(glGetUniformLocation(basic_program,LightsPropertiesName),1,&Lights[i].position[0]);
		sprintf(LightsPropertiesName,"Lights[%d].coneDirection\0",i);
		glUniform3fv(glGetUniformLocation(basic_program,LightsPropertiesName),1,&Lights[i].coneDirection[0]);
		sprintf(LightsPropertiesName,"Lights[%d].spotCosCutoff\0",i);
		glUniform1f(glGetUniformLocation(basic_program,LightsPropertiesName),Lights[i].spotCosCutoff);
		sprintf(LightsPropertiesName,"Lights[%d].spotExponent\0",i);
		glUniform1f(glGetUniformLocation(basic_program,LightsPropertiesName),Lights[i].spotExponent);
		sprintf(LightsPropertiesName,"Lights[%d].constantAttenuation\0",i);
		glUniform1f(glGetUniformLocation(basic_program,LightsPropertiesName),Lights[i].constantAttenuation);
		sprintf(LightsPropertiesName,"Lights[%d].linearAttenuation\0",i);
		glUniform1f(glGetUniformLocation(basic_program,LightsPropertiesName),Lights[i].linearAttenuation);
		sprintf(LightsPropertiesName,"Lights[%d].quadraticAttenuation\0",i);
		glUniform1f(glGetUniformLocation(basic_program,LightsPropertiesName),Lights[i].quadraticAttenuation);
	}
}

#endif