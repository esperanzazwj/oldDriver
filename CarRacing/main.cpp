#include "StdAfx.h"
#include "Texture.h"
#include "Base.h"
#include "Mesh.h"
#include "Lights.h"
#include "type.h"
#include "shader.h"
#include "Car.h"
#include "FBO.h"
#include "CSM.h"
#include "Camera.h"
#include "UI.h"
#include "DeferShading.h"

const float PI=3.1415926f;
//-----------------------------init data---------------------------------//
const int frame_x=1920,frame_y=1080;
//const int frame_x=2560,frame_y=1440;

int wnd_width=1280,wnd_height=720;
//int wnd_width=1920,wnd_height=1080;
float aspect=float(wnd_width)/wnd_height;
float camera_x=262,camera_y=176,camera_z=-356;
float angle_y=PI/2,angle_xz=PI/2;
float step=0.1f;
float _step=step*10;
int pre_x=-1,pre_y=-1;
using namespace std;
//--------------------------------------//
GLuint basic_program;
Mesh mesh;
Base obj[1000];
int opaque_obj[1000];
int NumOpaque;
int transparent_obj[1000];
int NumTransparent;
int NumObj;
//----------------------------------------//
Mesh mesh_road;
Base obj_road[1000];
int NumRoad;
//----------------block------------------//
int blockIndex[1000];
int blockNum;
int CarColliderIndex[10];
int CarColliderNum;

//-----------------------------------------//
Mesh mesh_skybox;
Base obj_box;
//---------------------
GLuint skycube_program;
GLuint wall_VAO;
GLuint wall_buffer[2];
GLuint skycube;
//-----------------------------------------//
GLuint test_program;
GLuint test_VAO;
GLuint test_VBO;
Texture2D test_Tex;
//------------------------------------------------//
GLuint shadow_program;
GLuint depth_texture;
GLuint depth_FBO;
GLuint show_program;
FBO shadowFBO[3];
bool HasShadow;
CSM csm;
//---------------------------------------------//
float speed;
float wheelrotate;
float pos_z;
//---------------water render-------------------
GLuint perlin_program;
Mesh mesh_water;
Base water_obj[10];
int NumWater;
//---------------------------------------------//
Car *car;
bool freeAcc;
bool freeBrake;
bool getbackl,getbackr;
//------------------------------------------------
float rotateXZ;
//--------------------------HDR----------------------------

FBO FBO1,FBO2,FBO3,FBO4,FBO5,FBO6;
GLuint downsampler_program;
GLuint Guass_program;
GLuint ToneMapping_program;
string Guass_fShader;
string ToneMapping_fShader;
bool ifHDR;

bool intersecttest;

Camera *camera;
//--------------UI---------------------
int final;
GLuint fontboard_program;
Font *ft;
FontBoard *fb_Title;
FontBoard *fb_StartGame;
FontBoard *fb_EndGame;
FontBoard *fb_Speed;
FontBoard *fb_RPM;
FontBoard *fb_time;
FontBoard *fb_nitro;
FontBoard *fb_Resume;
FontBoard *fb_Restart;
FontBoard *fb_Gameover;

//-----------particle-------------------
GLuint particle_VAO[2];
GLuint particle_Buffer[2];
GLuint particle_program;
GLuint particle_TFB[2];
GLuint TexSprite;
GLuint Query,Query0;
bool stop_particle;

//----------------state-------------------
bool ifstart,ifpause;
bool ifover;
bool preparetime;
int start_time,current_time;
int nitroRemain;
//------------------music----------------------
bool speedup = false;
MCI_OPEN_PARMS op_bgm,op_speedup,op_running;

//-------------defer shading for test-----------
dfFBO dffbo;
GLuint dfpass1_program;
GLuint dfpass2_program;

//------------------test mode--------------------
bool iftest;

void car_nitro_recover(int);
void playMusic(MCI_OPEN_PARMS&,int,bool);

void StartGame()
{
	ifstart=true;
	ifpause=false;
	ifover=false;
	preparetime=false;
	nitroRemain=200;
	fb_StartGame->SetEnable(false);
	fb_EndGame->SetEnable(false);
	fb_Resume->SetEnable(false);
	fb_Restart->SetEnable(false);
	glutTimerFunc(10000,car_nitro_recover,0);
	playMusic(op_bgm,START_MUSIC,true);
}
void PauseGame()
{
	ifpause=true;
	fb_Resume->SetEnable(true);
	fb_Restart->SetEnable(true);
	playMusic(op_bgm,PAUSE_MUSIC,true);
	if (speedup)
		playMusic(op_speedup,PAUSE_MUSIC,true);
	else
		playMusic(op_running,PAUSE_MUSIC,true);
}
void ResumeGame()
{
	ifpause=false;
	fb_Resume->SetEnable(false);
	fb_Restart->SetEnable(false);
	playMusic(op_bgm,RESTART_MUSIC,true);
	if (speedup)
		playMusic(op_speedup,RESTART_MUSIC,true);
	else
		playMusic(op_running,RESTART_MUSIC,true);
}
void ExitToTitle()
{
	ifstart=false;
	ifpause=false;
	ifover=false;
	preparetime=false;
	fb_StartGame->SetEnable(true);
	fb_EndGame->SetEnable(true);
	fb_Resume->SetEnable(false);
	fb_Restart->SetEnable(false);
	playMusic(op_bgm,STOP_MUSIC,true);
}
void GameOver()
{
	ifover=true;
	freeAcc=true;
	freeBrake=true;
	getbackl=true;
	getbackr=true;
}

void initMusic()
{
	op_speedup.dwCallback = NULL;
	op_speedup.lpstrAlias = NULL;
	op_speedup.lpstrDeviceType = "MPEGAudio";
	op_speedup.lpstrElementName = "speedup.mp3";
	op_speedup.wDeviceID = NULL;
	if (mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT, (DWORD)&op_speedup));
		cout<<"Music init error;"<<endl;
	op_bgm.dwCallback = NULL;
	op_bgm.lpstrAlias = NULL;
	op_bgm.lpstrDeviceType = "MPEGAudio";
	op_bgm.lpstrElementName = "bgm.mp3";
	op_bgm.wDeviceID = NULL;
	if (mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT, (DWORD)&op_bgm));
		cout<<"Music init error;"<<endl;
	op_running.dwCallback = NULL;
	op_running.lpstrAlias = NULL;
	op_running.lpstrDeviceType = "MPEGAudio";
	op_running.lpstrElementName = "running.mp3";
	op_running.wDeviceID = NULL;
	if (mciSendCommand(NULL, MCI_OPEN, MCI_OPEN_ELEMENT, (DWORD)&op_running));
		cout<<"Music init error;"<<endl;
}

void playMusic(MCI_OPEN_PARMS &op, int play, bool ifrepeat)
{
	MCI_PLAY_PARMS pp;
	pp.dwCallback = NULL;
	pp.dwFrom = 0;
	switch (play)
	{
	case START_MUSIC:
		if (ifrepeat)
			mciSendCommand(op.wDeviceID, MCI_PLAY, MCI_FROM|MCI_DGV_PLAY_REPEAT, (DWORD)&pp);
		else
			mciSendCommand(op.wDeviceID, MCI_PLAY, MCI_FROM|MCI_NOTIFY, (DWORD)&pp);
		break;
	case STOP_MUSIC:
		mciSendCommand(op.wDeviceID, MCI_STOP, NULL, (DWORD)&pp);
		break;
	case PAUSE_MUSIC:
		mciSendCommand(op.wDeviceID, MCI_PAUSE, NULL, (DWORD)&pp);
		break;
	case RESTART_MUSIC:
		if (ifrepeat)
			mciSendCommand(op.wDeviceID, MCI_PLAY, MCI_DGV_PLAY_REPEAT, (DWORD)&pp);
		else
			mciSendCommand(op.wDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD)&pp);
		break;
	}
}
void initshadow()
{
	camera=new Camera(wnd_width,wnd_height,glm::vec3(camera_x,camera_y,camera_z),angle_xz,angle_y);
	shadowFBO[0].CreateFBO(frame_x,frame_y,false,true,false);
	shadowFBO[1].CreateFBO(frame_x,frame_y,false,true,false);
	shadowFBO[2].CreateFBO(frame_x,frame_y,false,true,false);
	HasShadow=true;
	csm.InitDraw();
}
void initProgram()
{
	basic_program=glGenProgram(Basic_vShader,NULL,NULL,NULL,Basic_fShader);
	shadow_program=glGenProgram(shadow_vShader,NULL,NULL,NULL,shadow_fShader);
	show_program=glGenProgram(show_vShader,NULL,NULL,NULL,show_fShader);
	downsampler_program=glGenProgram(downsampler_vShader,NULL,NULL,NULL,downsampler_fShader);
	string *Guass_fShader=new string(GetShader("Guass_fShader.txt"));
	Guass_program=glGenProgram(downsampler_vShader,NULL,NULL,NULL,Guass_fShader->c_str());
	string *ToneMapping_fShader=new string(GetShader("ToneMapping_fShader.txt"));
	ToneMapping_program=glGenProgram(downsampler_vShader,NULL,NULL,NULL,ToneMapping_fShader->c_str());
	delete ToneMapping_fShader;
	delete Guass_fShader;
}
void initDeferShadingProgram()
{
	string *dfpass1_vShader=new string(GetShader("dfpass1_vShader.txt"));
	string *dfpass1_fShader=new string(GetShader("dfpass1_fShader.txt"));
	dfpass1_program=glGenProgram(dfpass1_vShader->c_str(),NULL,NULL,NULL,dfpass1_fShader->c_str());
	string *dfpass2_vShader=new string(GetShader("dfpass2_vShader.txt"));
	string *dfpass2_fShader=new string(GetShader("dfpass2_fShader.txt"));
	dfpass2_program=glGenProgram(dfpass2_vShader->c_str(),NULL,NULL,NULL,dfpass2_fShader->c_str());
	delete dfpass1_vShader;delete dfpass1_fShader;
	delete dfpass2_vShader;delete dfpass2_fShader;

}
void initSkybox()
{
	skycube_program=glGenProgram(skycube_vshader,NULL,NULL,NULL,skycube_fshader);
	glGenVertexArrays(1,&wall_VAO);
	glBindVertexArray(wall_VAO);
	glUseProgram(skycube_program);
	glGenBuffers(2,wall_buffer);
	glBindBuffer(GL_ARRAY_BUFFER,wall_buffer[0]);
	GLfloat wall_vertices[][3]={
		{-100000,-100000,-100000},
		{100000,-100000,-100000},
		{100000,100000,-100000},
		{-100000,100000,-100000},
		{-100000,-100000,100000},
		{100000,-100000,100000},
		{100000,100000,100000},
		{-100000,100000,100000}
	};
	glBufferData(GL_ARRAY_BUFFER,sizeof(wall_vertices),wall_vertices,GL_STATIC_DRAW);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(0);
	GLshort wall_index[]={
        0, 1, 3, 2, 7, 6, 4, 5,
        2, 6, 1, 5, 0, 4, 3, 7 
    };
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,wall_buffer[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(wall_index),wall_index,GL_STATIC_DRAW);
	char *filename[]={"pos_x.bmp","neg_x.bmp","neg_y.bmp","pos_y.bmp","pos_z.bmp","neg_z.bmp"};
	glGenTextures(1,&skycube);
	glBindTexture(GL_TEXTURE_CUBE_MAP,skycube);
	glTexStorage2D(GL_TEXTURE_CUBE_MAP,10,GL_RGB8,1024,1024);
	for (int i=0;i<6;i++)
	{
		AUX_RGBImageRec *pImage = NULL;
		pImage = auxDIBImageLoadA(filename[i]);
		if(pImage == NULL)	
		{
			cout<<"cannot open texture file"<<endl;
			continue;
		}
		GLenum target=GL_TEXTURE_CUBE_MAP_POSITIVE_X+i;
		glTexSubImage2D(target,0,0,0,pImage->sizeX,pImage->sizeY,GL_RGB,GL_UNSIGNED_BYTE,pImage->data);		
		free(pImage->data);free(pImage);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP,0);
}
void initTest()
{
	test_program=glGenProgram(test_vShader,NULL,NULL,NULL,test_fShader);
	glUseProgram(test_program);
	GLfloat test_vertices[4][3]={
		{-5,-5,0},
		{-5,5,0},
		{5,-5,0},
		{5,5,0}
	};
	GLfloat test_tex[4][2]={
		{0,0},
		{0,1},
		{1,0},
		{1,1}
	};
	glGenVertexArrays(1,&test_VAO);
	glBindVertexArray(test_VAO);
	glGenBuffers(1,&test_VBO);
	glBindBuffer(GL_ARRAY_BUFFER,test_VBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*20,NULL,GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(GLfloat)*12,test_vertices);
	glBufferSubData(GL_ARRAY_BUFFER,sizeof(GLfloat)*12,sizeof(GLfloat)*8,test_tex);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(const GLvoid*)0);
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(const GLvoid*)sizeof(test_vertices));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	iftest=false;
	//csm.InitDraw();
}
void initWater()
{
	string ssv=GetShader("noise_vshader.txt");
	string ssg=GetShader("noise_gshader.txt");
	string ssf=GetShader("noise_fshader1.txt");
	perlin_program=glGenProgram(ssv.c_str(),NULL,NULL,ssg.c_str(),ssf.c_str());
	mesh_water.LoadFromFile("water.obj");
	NumWater=mesh_water.m_Meshes.size();
	for (int i=0;i<NumWater;i++)
		water_obj[i].Load(mesh_water.m_Meshes[i],mesh_water.m_Materials,perlin_program,false);
}
void initUI()
{
	fontboard_program=glGenProgram(fontboard_vShader,NULL,NULL,NULL,fontboard_fShader);
	ft=new Font("C:\\WINDOWS\\Fonts\\simhei.ttf");
	FontBoard::GetAllAscllChar(ft);

	fb_Title=new FontBoard(ft,50,100,25,50,fontboard_program,wnd_width,wnd_height);
	fb_Title->SetPosition(wnd_width/8.0,wnd_height*0.8);

	fb_StartGame=new FontBoard(ft,50,50,25,20,fontboard_program,wnd_width,wnd_height);
	fb_StartGame->SetPosition(250,400);
	fb_StartGame->SetEventFunc(StartGame);

	fb_EndGame=new FontBoard(ft,50,50,25,20,fontboard_program,wnd_width,wnd_height);
	fb_EndGame->SetPosition(200,300);
	fb_EndGame->SetEventFunc(EndGame);

	fb_Speed=new FontBoard(ft,50,50,25,20,fontboard_program,wnd_width,wnd_height);
	fb_Speed->SetPosition(wnd_width/2,wnd_height*0.9);

	fb_RPM=new FontBoard(ft,50,50,25,20,fontboard_program,wnd_width,wnd_height);
	fb_RPM->SetPosition(wnd_width/2,wnd_height*0.8);

	fb_time=new FontBoard(ft,50,50,25,20,fontboard_program,wnd_width,wnd_height);
	fb_time->SetPosition(wnd_width/6,wnd_height*0.8);

	fb_nitro=new FontBoard(ft,50,50,25,20,fontboard_program,wnd_width,wnd_height);
	fb_nitro->SetPosition(wnd_width/6,wnd_height*0.9);

	fb_Resume=new FontBoard(ft,50,50,25,20,fontboard_program,wnd_width,wnd_height);
	fb_Resume->SetPosition(200,400);
	fb_Resume->SetEnable(false);
	fb_Resume->SetEventFunc(ResumeGame);

	fb_Restart=new FontBoard(ft,50,50,25,20,fontboard_program,wnd_width,wnd_height);
	fb_Restart->SetPosition(200,300);
	fb_Restart->SetEnable(false);
	fb_Restart->SetEventFunc(ExitToTitle);

	fb_Gameover=new FontBoard(ft,50,50,25,50,fontboard_program,wnd_width,wnd_height);
	fb_Gameover->SetPosition(wnd_width/32,wnd_height/2);
	fb_Gameover->SetEnable(false);
	fb_Gameover->SetEventFunc(ExitToTitle);

	ifstart=false;preparetime=false;ifover=false;
	start_time=0;current_time=0;nitroRemain=0;
}
void initParticle()
{
	particle_program=glGenProgram(particle_vshader,NULL,NULL,particle_gshader,particle_fshader);
	glUseProgram(particle_program);
	GLfloat InitData[8]={
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,
		0.0f
	};

	glGenQueries(1,&Query);
	glGenQueries(1,&Query0);

	glGenVertexArrays(2,particle_VAO);
	glGenBuffers(2,particle_Buffer);
	glGenTransformFeedbacks(2,particle_TFB);
	for (int i=0;i<2;i++)
	{
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,particle_TFB[i]);
		glBindVertexArray(particle_VAO[i]);
		glBindBuffer(GL_ARRAY_BUFFER,particle_Buffer[i]);
		glBufferData(GL_ARRAY_BUFFER,sizeof(GLfloat)*1024*1024,NULL,GL_DYNAMIC_COPY);
		if (i==0) glBufferSubData(GL_ARRAY_BUFFER,0,sizeof(GLfloat)*8,InitData);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat)*8,(const void*)0);
		glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat)*8,(const void*)(sizeof(GLfloat)*3));
		glVertexAttribPointer(2,1,GL_FLOAT,GL_FALSE,sizeof(GLfloat)*8,(const void*)(sizeof(GLfloat)*6));
		glVertexAttribPointer(3,1,GL_FLOAT,GL_FALSE,sizeof(GLfloat)*8,(const void*)(sizeof(GLfloat)*7));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,particle_TFB[i]);
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,0,particle_Buffer[i]);
	}
	const GLchar *var[]={
		"NewPosition",
		"NewVelocity",
		"NewType",
		"NewAge"
	};
	glTransformFeedbackVaryings(particle_program,4,var,GL_INTERLEAVED_ATTRIBS);
	glLinkProgram(particle_program);
	GLint linked;
	glGetProgramiv(particle_program,GL_LINK_STATUS,&linked);
	if (linked!=GL_TRUE)
	{
		char infolog[1000];
		glGetProgramInfoLog(particle_program,sizeof(infolog),NULL,infolog);
		std::cout<<"transform feedback link error"<<endl<<infolog<<endl;
	}
	else std::cout<<"transform feedback link sucess"<<endl;
	stop_particle=true;
}
void init()
{
	initLights();
	initshadow();
	initProgram();
	initWater();
	initParticle();
	initMusic();
	initDeferShadingProgram();

	mesh.LoadFromFile("sportscar.3DS");
	mesh_road.LoadFromFile("shanghai.obj");
	mesh_skybox.LoadFromFile("box.obj");
	obj_box.Load(mesh_skybox.m_Meshes[0],mesh_skybox.m_Materials,show_program,false);

	NumRoad=mesh_road.m_Meshes.size();
	for (int i = 0; i < NumRoad; i++)
	{
		if (mesh_road.m_Meshes[i].Name.substr(0, 3) == "Box" || mesh_road.m_Meshes[i].Name=="final")
		{
			obj_road[i].Load(mesh_road.m_Meshes[i], mesh_road.m_Materials, basic_program, true);
			obj_road[i].obb->InitDraw();
			obj_road[i].obb->InitDrawAxis();
			if (mesh_road.m_Meshes[i].Name=="final") final=i;
			else blockIndex[blockNum++] = i;
		}
		else
			obj_road[i].Load(mesh_road.m_Meshes[i], mesh_road.m_Materials, basic_program, false);
	}

	for (int i = 0; i < blockNum; i++)
	{
		glm::mat4 MVmat = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,0.0f))
			*glm::rotate(glm::mat4(1.0f),-PI/2,glm::vec3(0.0f,1.0f,0.0f))
			//*glm::scale(glm::mat4(1.0f),glm::vec3(1.0f,1.0f,1.0f))
			*glm::scale(glm::mat4(1.0f),glm::vec3(4.0f,4.0f,4.0f))
			*obj_road[blockIndex[i]].transformMat;
		obj_road[blockIndex[i]].obb->SetTrans(MVmat);
		obj_road[blockIndex[i]].obb->InitDrawTransform();
	}

	glm::mat4 MVmat_final = glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,0.0f))
			*glm::rotate(glm::mat4(1.0f),-PI/2,glm::vec3(0.0f,1.0f,0.0f))
			//*glm::scale(glm::mat4(1.0f),glm::vec3(1.0f,1.0f,1.0f))
			*glm::scale(glm::mat4(1.0f),glm::vec3(4.0f,4.0f,4.0f))
			*obj_road[final].transformMat;
	obj_road[final].obb->SetTrans(MVmat_final);
	obj_road[final].obb->InitDrawTransform();


	for (int i=0;i<mesh.NumMaterials;i++)
	{
		memcpy(mesh.m_Materials[i]->Ka,mesh.m_Materials[i]->Kd,sizeof(float)*3);//modified
	}
	NumObj=mesh.m_Meshes.size();
	NumOpaque=NumTransparent=0;
	int xx=0;
	for (int i=0;i<NumObj;i++)
	{
		if (mesh.m_Meshes[i].Name=="F430" && (++xx)==1)
		{
			obj[i].Load(mesh.m_Meshes[i],mesh.m_Materials,basic_program,true);
			obj[i].obb->InitDraw();
			CarColliderIndex[CarColliderNum++]=i;
		}
		else 
			obj[i].Load(mesh.m_Meshes[i],mesh.m_Materials,basic_program,false);
		if (mesh.m_Materials[mesh.m_Meshes[i].MaterialIndex]->Opacity!=1.0)
			transparent_obj[NumTransparent++]=i;
		else 
			opaque_obj[NumOpaque++]=i;
	}
	
	intersecttest=false;

	car=new Car;
	car->SetCollisionInfo(obj,CarColliderNum,CarColliderIndex,obj_road,blockNum,blockIndex,&obj_road[final]);

	freeAcc=true;freeBrake=true;getbackl=true;getbackr=true;
	rotateXZ=0;

	initSkybox();
	
	initTest();

	dfFBO::wnd_width=wnd_width;dfFBO::wnd_height=wnd_height;
	dffbo.CreateFBO(frame_x,frame_y);
	dffbo.CreateQuad();

	FBO::wnd_width=wnd_width;FBO::wnd_height=wnd_height;
	FBO1.CreateFBO(frame_x,frame_y,true,true,false);FBO1.CreateQuad();
	FBO2.CreateFBO(frame_x/2,frame_y/2,true,false,false);FBO2.CreateQuad();
	FBO3.CreateFBO(frame_x/4,frame_y/4,true,false,false);FBO3.CreateQuad();
	FBO4.CreateFBO(frame_x/4,frame_y/4,true,false,false);FBO4.CreateQuad();
	FBO5.CreateFBO(frame_x/4,frame_y/4,true,false,false);FBO5.CreateQuad();
	FBO6.CreateFBO(frame_x,frame_y,true,false,false);FBO6.CreateQuad();
	ifHDR=true;

	initUI();
}
void BeginCommonProcess()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	glViewport(0,0,wnd_width,wnd_height);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);
	//FBO6.Begin();
	FBO1.DrawQuad(downsampler_program);
}
void BeginHDRprocess()
{
	glDisable(GL_DEPTH_TEST);
	FBO2.Begin();
	FBO1.DrawQuad(downsampler_program);
	FBO3.Begin();
	FBO2.DrawQuad(downsampler_program);
	FBO4.Begin();
	FBO3.DrawQuadGuass(Guass_program,true);
	FBO5.Begin();
	FBO4.DrawQuadGuass(Guass_program,false);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
	glViewport(0,0,wnd_width,wnd_height);
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	FBO1.DrawToneMapping(ToneMapping_program,FBO5.GetColorTexture(),0.05,0.1,0.95);
	glEnable(GL_DEPTH_TEST);
}

void display()
{
	camera->OnMoveEdge();

	if (ifstart)
	{
		camera->CameraPos=car->position+glm::vec3(0,5,0)-car->carDirection*18.0f;
		camera->Direction=car->carDirection;
		camera->Up=glm::vec3(0,1,0);
	}

	glm::vec3 LightPos(camera->CameraPos);

	csm.CalcOrthoProjs(45.0f,aspect,1.0f,10000.0f,camera->CameraPos,camera->CameraPos+camera->Direction,camera->Up,
		LightPos,LightPos+glm::vec3(-1.0f,-1.0f,0.0f),glm::vec3(-1.0f,1.0f,0.0f));
	glUseProgram(shadow_program);

	//--------------render for shadow-------------//
	for (int ishadow=0;ishadow<3;ishadow++)
	{
		shadowFBO[ishadow].Begin();
		
		glm::mat4 VPmat_shadow=csm.GetOrthProjMat(ishadow)
			*glm::lookAt(csm.LightsPos[ishadow],csm.LightsPos[ishadow]+glm::vec3(-1.0f,-1.0f,0.0f),glm::vec3(-1.0f,1.0f,0.0f));
	
		for (int i=0;i<NumObj;i++)
		{

			glm::mat4 MVPmat_shadow=VPmat_shadow
					*glm::translate(glm::mat4(1.0f),car->GetPos())
					*glm::rotate(glm::mat4(1.0f),car->angleXZ,glm::vec3(0,1,0))
					*glm::rotate(glm::mat4(1.0f),PI/2,glm::vec3(0,1,0))
					*glm::scale(glm::mat4(1.0f),glm::vec3(0.015f,0.015f,0.015f))
					*obj[i].transformMat;
					
			glUniformMatrix4fv(glGetUniformLocation(shadow_program,"MVPmat"),1,GL_FALSE,&MVPmat_shadow[0][0]);

			tMaterial *pMaterial=obj[i].pMaterial;
			if (pMaterial->NumDiffuseTexture>0)
			{
				glActiveTexture(GL_TEXTURE30);
				glBindTexture(GL_TEXTURE_2D,pMaterial->DiffuseTexture[0]->GetTexture());
				glUniform1i(glGetUniformLocation(shadow_program,"tex2D"),30);
			}

			obj[i].Render();
		}
		for (int i=0;i<NumRoad;i++)
		{
			if (obj_road[i].objName.substr(0,3)=="Box" || obj_road[i].objName=="final") continue;
		glm::mat4 MVPmat_shadow=VPmat_shadow
						*glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,0.0f))
						*glm::rotate(glm::mat4(1.0f),-PI/2,glm::vec3(0.0f,1.0f,0.0f))
						//*glm::scale(glm::mat4(1.0f),glm::vec3(1.0f,1.0f,1.0f))
						*glm::scale(glm::mat4(1.0f),glm::vec3(4.0f,4.0f,4.0f))
						*obj_road[i].transformMat;
			glUniformMatrix4fv(glGetUniformLocation(shadow_program,"MVPmat"),1,GL_FALSE,&MVPmat_shadow[0][0]);

			tMaterial *pMaterial=obj_road[i].pMaterial;
			if (pMaterial->NumDiffuseTexture>0)
			{
				glActiveTexture(GL_TEXTURE30);
				glBindTexture(GL_TEXTURE_2D,pMaterial->DiffuseTexture[0]->GetTexture());
				glUniform1i(glGetUniformLocation(shadow_program,"tex2D"),30);
			}


			obj_road[i].Render();
		}
		shadowFBO[ishadow].End();
	}
	
	glm::mat4 ViewMat=glm::perspective(45.0f,aspect,1.0f,1000000.0f)*glm::lookAt(camera->CameraPos,camera->CameraPos+camera->Direction,camera->Up);

	//-----------------------------------------defered shading test pass1-------------------------------------------//
	/*dffbo.Begin();
	glUseProgram(dfpass1_program);
	for (int i=0;i<NumRoad;i++)
	{
		if (obj_road[i].objName.substr(0,3)=="Box" || obj_road[i].objName=="final") continue;
		if (obj_road[i].objName.find("water")!=string::npos) continue;
		glm::mat4 MVmat=glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,0.0f))
			*glm::rotate(glm::mat4(1.0f),-PI/2,glm::vec3(0.0f,1.0f,0.0f))
			//*glm::scale(glm::mat4(1.0f),glm::vec3(1.0f,1.0f,1.0f))
			*glm::scale(glm::mat4(1.0f),glm::vec3(4.0f,4.0f,4.0f))
			*obj_road[i].transformMat;
		glm::mat3 Normalmat;
		for (int i1=0;i1<3;i1++)
			for (int j1=0;j1<3;j1++)
				Normalmat[i1][j1]=MVmat[i1][j1];
		Normalmat=glm::transpose(glm::inverse(Normalmat));
		glm::mat4 MVPmat=ViewMat*MVmat;
		glUniformMatrix4fv(glGetUniformLocation(dfpass1_program,"MVmat"),1,GL_FALSE,&MVmat[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(dfpass1_program,"MVPmat"),1,GL_FALSE,&MVPmat[0][0]);
		glUniformMatrix3fv(glGetUniformLocation(dfpass1_program,"Normalmat"),1,GL_FALSE,&Normalmat[0][0]);

		tMaterial *pMaterial=obj_road[i].pMaterial;

		glUniform3fv(glGetUniformLocation(dfpass1_program,"Ka"),1,pMaterial->Ka);
		glUniform3fv(glGetUniformLocation(dfpass1_program,"Kd"),1,pMaterial->Kd);
		glUniform3fv(glGetUniformLocation(dfpass1_program,"Ks"),1,pMaterial->Ks);
		glUniform1f(glGetUniformLocation(dfpass1_program,"Shininess"),pMaterial->Shininess);
		glUniform1f(glGetUniformLocation(dfpass1_program,"Opacity"),pMaterial->Opacity);
		if (pMaterial->NumDiffuseTexture>0)
		{
			glUniform1i(glGetUniformLocation(dfpass1_program,"HasDiffuseTexture"),1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,pMaterial->DiffuseTexture[0]->GetTexture());
			glUniform1i(glGetUniformLocation(dfpass1_program,"DiffuseTexture"),0);
		}
		else glUniform1i(glGetUniformLocation(dfpass1_program,"HasDiffuseTexture"),0);
		if (pMaterial->NumSpecularTexture>0)
		{
			glUniform1i(glGetUniformLocation(dfpass1_program,"HasSpecularTexture"),1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,pMaterial->SpecularTexture[0]->GetTexture());
			glUniform1i(glGetUniformLocation(dfpass1_program,"SpecularTexture"),0);
		}
		else glUniform1i(glGetUniformLocation(dfpass1_program,"HasSpecularTexture"),0);

		obj_road[i].Render();
	}
	dffbo.End();*/


	FBO1.Begin(); 

	glEnable(GL_LINE_SMOOTH);  
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	//-----------------------------------------defered shading test pass2-------------------------------------------//
	/*glUseProgram(dfpass2_program);
	LoadLightsInfo(dfpass2_program);
	glUniform3fv(glGetUniformLocation(dfpass2_program,"EyePosition"),1,&camera->CameraPos[0]);
	glUniform1i(glGetUniformLocation(dfpass2_program,"HasShadow"),true);
	glUniformMatrix4fv(glGetUniformLocation(dfpass2_program,"WorldMat"),1,GL_FALSE,&ViewMat[0][0]);
	char pname[100];
	for (int i=0;i<3;i++)
	{
		glm::mat4 ShadowMat=csm.GetOrthProjMat(i)
				*glm::lookAt(csm.LightsPos[i],csm.LightsPos[i]+glm::vec3(-1.0f,-1.0f,0.0f),glm::vec3(-1.0f,1.0f,0.0f));
		sprintf(pname,"ShadowMat[%d]\0",i);
		glUniformMatrix4fv(glGetUniformLocation(dfpass2_program,pname),1,GL_FALSE,&ShadowMat[0][0]);
	}
	for (int i=0;i<3;i++)
	{
		glActiveTexture(GL_TEXTURE0+i);
		glBindTexture(GL_TEXTURE_2D,shadowFBO[i].DepthTexture);
		sprintf(pname,"DepthTexture[%d]\0",i);
		glUniform1i(glGetUniformLocation(dfpass2_program,pname),i);
	}
	for (int i=0;i<3;i++)
	{
		sprintf(pname,"CascadeEndClipSpace[%d]\0",i);
		glUniform1f(glGetUniformLocation(dfpass2_program,pname),csm.m_cascadeEnd[i+1]);
	}
	static const char *dfTex[8]={
		"WorldPosTex","NormalTex","DiffuseColorTex","SpecularColorTex",
		"KaTex","KdTex","KsTex","ShininessTex"
	};
	for (int i=0;i<8;i++)
	{
		glActiveTexture(GL_TEXTURE3+i);
		glBindTexture(GL_TEXTURE_2D,dffbo.ColorTexture[i]);
		glUniform1i(glGetUniformLocation(dfpass2_program,dfTex[i]),3+i);
	}
	dffbo.DrawQuad(dfpass2_program);
	*/

	glUseProgram(basic_program);
	LoadLightsInfo(basic_program);
	glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_CUBE_MAP,skycube);
	glUniform1i(glGetUniformLocation(basic_program,"EnvTexture"),31);
	glUniform1f(glGetUniformLocation(basic_program,"EnvFactor"),0.8);
	glUniform1f(glGetUniformLocation(basic_program,"Strength"),1.0f);
	glUniform3fv(glGetUniformLocation(basic_program,"EyePosition"),1,&camera->CameraPos[0]);
	glUniform1i(glGetUniformLocation(basic_program,"HasShadow"),true);

	for (int i=0;i<NumOpaque;i++)
	{
		glm::mat4 MVmat(1.0f);
		//if (obj[opaque_obj[i]].objName=="wheel00" || obj[opaque_obj[i]].objName=="wheel01" 
			//|| obj[opaque_obj[i]].objName=="wheel10" || obj[opaque_obj[i]].objName=="wheel11")
			// MVmat=glm::rotate(glm::mat4(1.0f),wheelrotate,glm::vec3(1.0f,0.0f,0.0f));
		MVmat=glm::translate(glm::mat4(1.0f),car->GetPos())
			*glm::rotate(glm::mat4(1.0f),car->angleXZ,glm::vec3(0,1,0))
			*glm::rotate(glm::mat4(1.0f),PI/2,glm::vec3(0,1,0))
			*glm::scale(glm::mat4(1.0f),glm::vec3(0.015f,0.015f,0.015f))
			*obj[opaque_obj[i]].transformMat*MVmat;
		glm::mat3 Normalmat;
		for (int i1=0;i1<3;i1++)
			for (int j1=0;j1<3;j1++)
				Normalmat[i1][j1]=MVmat[i1][j1];
		Normalmat=glm::transpose(glm::inverse(Normalmat));
		glm::mat4 MVPmat=ViewMat*MVmat;
		glUniformMatrix4fv(glGetUniformLocation(basic_program,"MVmat"),1,GL_FALSE,&MVmat[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(basic_program,"MVPmat"),1,GL_FALSE,&MVPmat[0][0]);
		glUniformMatrix3fv(glGetUniformLocation(basic_program,"Normalmat"),1,GL_FALSE,&Normalmat[0][0]);

		tMaterial *pMaterial=obj[opaque_obj[i]].pMaterial;
		obj[opaque_obj[i]].InitMaterial(pMaterial);

		char pname[100];
		for (int j=0;j<3;j++)
		{
			glm::mat4 ShadowMat=csm.GetOrthProjMat(j)
					*glm::lookAt(csm.LightsPos[j],csm.LightsPos[j]+glm::vec3(-1.0f,-1.0f,0.0f),glm::vec3(-1.0f,1.0f,0.0f))
					*MVmat;
			
			sprintf(pname,"ShadowMat[%d]\0",j);
			glUniformMatrix4fv(glGetUniformLocation(basic_program,pname),1,GL_FALSE,&ShadowMat[0][0]);
		}
		
		for (int j=0;j<3;j++)
		{
			glActiveTexture(GL_TEXTURE0+pMaterial->NumDiffuseTexture+pMaterial->NumSpecularTexture+j);
			glBindTexture(GL_TEXTURE_2D,shadowFBO[j].DepthTexture);
			sprintf(pname,"DepthTexture[%d]\0",j);
			glUniform1i(glGetUniformLocation(basic_program,pname),
				pMaterial->NumDiffuseTexture+pMaterial->NumSpecularTexture+j);
		}
		for (int j=0;j<3;j++)
		{
			sprintf(pname,"CascadeEndClipSpace[%d]\0",j);
			glUniform1f(glGetUniformLocation(basic_program,pname),
				csm.m_cascadeEnd[j+1]);
		}

		
		obj[opaque_obj[i]].Render();
	}
	//-----------------------draw transparent-----------------
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
	for (int i=0;i<NumTransparent;i++)
	{
		glm::mat4 MVmat(1.0f);
		if (obj[opaque_obj[i]].objName=="wheel00" || obj[opaque_obj[i]].objName=="wheel01" 
			|| obj[opaque_obj[i]].objName=="wheel10" || obj[opaque_obj[i]].objName=="wheel11")
			 MVmat=glm::rotate(glm::mat4(1.0f),wheelrotate,glm::vec3(1.0f,0.0f,0.0f));
		MVmat=glm::translate(glm::mat4(1.0f),car->GetPos())
			*glm::rotate(glm::mat4(1.0f),car->angleXZ,glm::vec3(0,1,0))
			*glm::rotate(glm::mat4(1.0f),PI/2,glm::vec3(0,1,0))
			*glm::scale(glm::mat4(1.0f),glm::vec3(0.015f,0.015f,0.015f))
			*obj[transparent_obj[i]].transformMat*MVmat;
		glm::mat3 Normalmat;
		for (int i1=0;i1<3;i1++)
			for (int j1=0;j1<3;j1++)
				Normalmat[i1][j1]=MVmat[i1][j1];
		Normalmat=glm::transpose(glm::inverse(Normalmat));
		glm::mat4 MVPmat=ViewMat*MVmat;
		glUniformMatrix4fv(glGetUniformLocation(basic_program,"MVmat"),1,GL_FALSE,&MVmat[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(basic_program,"MVPmat"),1,GL_FALSE,&MVPmat[0][0]);
		glUniformMatrix3fv(glGetUniformLocation(basic_program,"Normalmat"),1,GL_FALSE,&Normalmat[0][0]);
		tMaterial *pMaterial=obj[transparent_obj[i]].pMaterial;
		obj[transparent_obj[i]].InitMaterial(pMaterial);

		char pname[100];
		for (int j=0;j<3;j++)
		{
			glm::mat4 ShadowMat=csm.GetOrthProjMat(j)
					*glm::lookAt(csm.LightsPos[j],csm.LightsPos[j]+glm::vec3(-1.0f,-1.0f,0.0f),glm::vec3(-1.0f,1.0f,0.0f))
					*MVmat;
			
			sprintf(pname,"ShadowMat[%d]\0",j);
			glUniformMatrix4fv(glGetUniformLocation(basic_program,pname),1,GL_FALSE,&ShadowMat[0][0]);
		}
		
		for (int j=0;j<3;j++)
		{
			glActiveTexture(GL_TEXTURE0+pMaterial->NumDiffuseTexture+pMaterial->NumSpecularTexture+j);
			glBindTexture(GL_TEXTURE_2D,shadowFBO[j].DepthTexture);
			sprintf(pname,"DepthTexture[%d]\0",j);
			glUniform1i(glGetUniformLocation(basic_program,pname),
				pMaterial->NumDiffuseTexture+pMaterial->NumSpecularTexture+j);
		}
		for (int j=0;j<3;j++)
		{
			sprintf(pname,"CascadeEndClipSpace[%d]\0",j);
			glUniform1f(glGetUniformLocation(basic_program,pname),
				csm.m_cascadeEnd[j+1]);
		}

		obj[transparent_obj[i]].Render();
	}
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	

	//-----------------------------------
	glUniform1i(glGetUniformLocation(basic_program,"HasShadow"),HasShadow);
	glUniform1f(glGetUniformLocation(basic_program,"EnvFactor"),0.0);
	for (int i=0;i<NumRoad;i++)
	{
		if (obj_road[i].objName.substr(0,3)=="Box" || obj_road[i].objName=="final") continue;
		if (obj_road[i].objName.find("water")!=string::npos) continue;
		glm::mat4 MVmat=glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,0.0f))
			*glm::rotate(glm::mat4(1.0f),-PI/2,glm::vec3(0.0f,1.0f,0.0f))
			//*glm::scale(glm::mat4(1.0f),glm::vec3(1.0f,1.0f,1.0f))
			*glm::scale(glm::mat4(1.0f),glm::vec3(4.0f,4.0f,4.0f))
			*obj_road[i].transformMat;
		glm::mat3 Normalmat;
		for (int i1=0;i1<3;i1++)
			for (int j1=0;j1<3;j1++)
				Normalmat[i1][j1]=MVmat[i1][j1];
		Normalmat=glm::transpose(glm::inverse(Normalmat));
		glm::mat4 MVPmat=ViewMat*MVmat;
		glUniformMatrix4fv(glGetUniformLocation(basic_program,"MVmat"),1,GL_FALSE,&MVmat[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(basic_program,"MVPmat"),1,GL_FALSE,&MVPmat[0][0]);
		glUniformMatrix3fv(glGetUniformLocation(basic_program,"Normalmat"),1,GL_FALSE,&Normalmat[0][0]);
		tMaterial *pMaterial=obj_road[i].pMaterial;

		char pname[100];
		for (int j=0;j<3;j++)
		{
			glm::mat4 ShadowMat=csm.GetOrthProjMat(j)
					*glm::lookAt(csm.LightsPos[j],csm.LightsPos[j]+glm::vec3(-1.0f,-1.0f,0.0f),glm::vec3(-1.0f,1.0f,0.0f))
					*MVmat;
			
			sprintf(pname,"ShadowMat[%d]\0",j);
			glUniformMatrix4fv(glGetUniformLocation(basic_program,pname),1,GL_FALSE,&ShadowMat[0][0]);
		}
		
		for (int j=0;j<3;j++)
		{
			glActiveTexture(GL_TEXTURE0+pMaterial->NumDiffuseTexture+pMaterial->NumSpecularTexture+j);
			glBindTexture(GL_TEXTURE_2D,shadowFBO[j].DepthTexture);
			sprintf(pname,"DepthTexture[%d]\0",j);
			glUniform1i(glGetUniformLocation(basic_program,pname),
				pMaterial->NumDiffuseTexture+pMaterial->NumSpecularTexture+j);
		}
		for (int j=0;j<3;j++)
		{
			sprintf(pname,"CascadeEndClipSpace[%d]\0",j);
			glUniform1f(glGetUniformLocation(basic_program,pname),
				csm.m_cascadeEnd[j+1]);
		}
		
		obj_road[i].InitMaterial(pMaterial);
		obj_road[i].Render();
	}
	
	//--------------------render water-------------------
	static float time=0;
	glUseProgram(perlin_program);LoadLightsInfo(perlin_program);
	glUniform1i(glGetUniformLocation(perlin_program,"EnvTexture"),31);
	glUniform1f(glGetUniformLocation(perlin_program,"EnvFactor"),0.95);
	for (int i=0;i<NumWater;i++)
	{
		glm::mat4 WorldMat=
			glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,-5.0f,0.0f))
			*glm::rotate(glm::mat4(1.0f),-PI/2,glm::vec3(0.0f,1.0f,0.0f))
			//*glm::scale(glm::mat4(1.0f),glm::vec3(1.0f,1.0f,1.0f))
			*glm::scale(glm::mat4(1.0f),glm::vec3(4.0f,4.0f,4.0f))
			*water_obj[i].transformMat;
		glm::mat3 Normalmat;
		for (int i1=0;i1<3;i1++)
			for (int j1=0;j1<3;j1++)
				Normalmat[i1][j1]=WorldMat[i1][j1];
		Normalmat=glm::transpose(glm::inverse(Normalmat));
		glm::mat4 WVPmat=ViewMat*WorldMat;
		glUniformMatrix4fv(glGetUniformLocation(perlin_program,"WorldMat"),1,GL_FALSE,&WorldMat[0][0]);
		glUniformMatrix3fv(glGetUniformLocation(perlin_program,"Normalmat"),1,GL_FALSE,&Normalmat[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(perlin_program,"WVPmat"),1,GL_FALSE,&WVPmat[0][0]);
		glUniform1f(glGetUniformLocation(perlin_program,"time"),time);
		glUniform3fv(glGetUniformLocation(perlin_program,"EyePosition"),1,&camera->CameraPos[0]);
		tMaterial *pMaterial=water_obj[i].pMaterial;
		water_obj[i].InitMaterial(pMaterial);
		water_obj[i].Render();
	}
	time+=0.0005;

	//------------------skybox--------------------
	glUseProgram(show_program);
	glm::mat4 MVPmat_skybox=ViewMat*glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,-500.0f,0.0f))*glm::scale(glm::mat4(1.0f),glm::vec3(1000.0f,1000.0f,1000.0f))*obj_box.transformMat;
	glUniformMatrix4fv(glGetUniformLocation(show_program,"MVPmat"),1,GL_FALSE,&MVPmat_skybox[0][0]);
	glUniform1i(glGetUniformLocation(show_program,"Tex2D"),0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,mesh_skybox.m_Materials[1]->DiffuseTexture[0]->GetTexture());
	obj_box.Render();


	//---------------paritcle----------------------
	static bool bFirst=true;
	static int TFBx=0;
	static float Seed=0;
	
	bool stop_particle0=(car==NULL) || (car->nitro<=0);

	glEnable(GL_POINT_SPRITE_ARB);
	glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
	
	glUseProgram(particle_program);
	glm::mat4 MVPmat_particle=ViewMat
		*glm::translate(glm::mat4(1.0f),car->GetPos())
		*glm::translate(glm::mat4(1.0f),car->carDirection*(-5.0f)+car->carHorizontal*(-1.8f));
	glUniformMatrix4fv(glGetUniformLocation(particle_program,"MVPmat"),1,GL_FALSE,&MVPmat_particle[0][0]);
	glUniform1f(glGetUniformLocation(particle_program,"LauncherLifeTime"),0.0f);
	glUniform1f(glGetUniformLocation(particle_program,"ShellLifeTime"),0.5f);
	glUniform1f(glGetUniformLocation(particle_program,"ShellSecLifeTime"),1.0f);
	glUniform1f(glGetUniformLocation(particle_program,"Interval"),0.1f);
	glUniform1f(glGetUniformLocation(particle_program,"RandSeed"),Seed);
	glUniform1i(glGetUniformLocation(particle_program,"stop"),stop_particle0);
	glUniform1i(glGetUniformLocation(particle_program,"TexSprite"),0);
	glUniform3fv(glGetUniformLocation(particle_program,"DirectionX"),1,&car->upDirection[0]);
	glUniform3fv(glGetUniformLocation(particle_program,"DirectionY"),1,&(-car->carDirection)[0]);
	glUniform3fv(glGetUniformLocation(particle_program,"DirectionZ"),1,&car->carHorizontal[0]);
	glBindTexture(GL_TEXTURE_2D,TexSprite);

	glBindVertexArray(particle_VAO[TFBx]);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK,particle_TFB[!TFBx]);
	glBeginTransformFeedback(GL_POINTS);
	if (bFirst)
	{
		glDrawArrays(GL_POINTS,0,1);
		bFirst=false;
	}
	else glDrawTransformFeedback(GL_POINTS,particle_TFB[TFBx]);
	glEndTransformFeedback();
	TFBx=!TFBx;Seed+=0.05;

	//------------------------------test------------------------
	if (iftest){
	glUseProgram(test_program);
	for (int i = 0; i < blockNum; i++)
	{
		glUniformMatrix4fv(glGetUniformLocation(test_program, "MVPmat"), 1, GL_FALSE, &ViewMat[0][0]);
		obj_road[blockIndex[i]].obb->Draw();
	}
	for (int i=0;i<CarColliderNum;i++)
	{
		glm::mat4 MVmat = glm::translate(glm::mat4(1.0f),car->GetPos())
				*glm::rotate(glm::mat4(1.0f),car->angleXZ,glm::vec3(0,1,0))
				*glm::rotate(glm::mat4(1.0f),PI/2,glm::vec3(0,1,0))
				*glm::scale(glm::mat4(1.0f),glm::vec3(0.015f,0.015f,0.015f))
				*obj[CarColliderIndex[i]].transformMat;
		glUniformMatrix4fv(glGetUniformLocation(test_program, "MVPmat"), 1, GL_FALSE, &ViewMat[0][0]);
		obj[CarColliderIndex[i]].obb->SetTrans(MVmat);
		obj[CarColliderIndex[i]].obb->InitDrawTransform();
		obj[CarColliderIndex[i]].obb->Draw();
	}

	//glDisable(GL_DEPTH_TEST);
	//glUniformMatrix4fv(glGetUniformLocation(test_program, "MVPmat"), 1, GL_FALSE, &ViewMat[0][0]);
	//csm.Draw(test_program,0);
	//glEnable(GL_DEPTH_TEST);
	}
	//---------------------------------
	if (!ifstart)
	{
		fb_Title->Render("NFS: AN EXPERIENCED CAR DRIVER");
		fb_StartGame->Render("START GAME");
		fb_EndGame->Render("LEAVE THE GAME");
	}
	else
	{
		if (!preparetime)
		{
			if (car!=NULL) delete car;
			car=new Car;
			car->SetCollisionInfo(obj,CarColliderNum,CarColliderIndex,obj_road,blockNum,blockIndex,&obj_road[final]);

			start_time=clock();
			preparetime=true;
		}
		if (!ifpause && !ifover) current_time=clock();
		double interval=(double)(current_time-start_time)/CLOCKS_PER_SEC;
		char text[50];
		sprintf(text,"%.0f Km/h",car->velocity*3.6);
		fb_Speed->Render(text);
		sprintf(text,"%d RPM gear %d",(int)car->RPM,car->gear+1);
		fb_RPM->Render(text);
		sprintf(text,"%.2lf sec",interval);
		fb_time->Render(text);
		sprintf(text,"%d nitro",nitroRemain);
		fb_nitro->Render(text);

		if (ifpause)
		{
			fb_Resume->Render("RESUME");
			fb_Restart->Render("RESTART");
		}
		if (ifover)
		{
			sprintf(text,"You have finished the game within %.2lf sec",interval);
			fb_Gameover->Render(text);
		}
	}
	

	if (ifHDR) BeginHDRprocess();
	else BeginCommonProcess();
	
	glutSwapBuffers();
}
void car_nitro_recover(int value)
{
	if (!ifstart || ifover) return;
	if (ifpause) {glutTimerFunc(10000,car_nitro_recover,0);return;}
	nitroRemain+=10;glutTimerFunc(10000,car_nitro_recover,0);
}
void car_nitro_timer(int value)
{
	if (ifpause) {glutTimerFunc(100,car_nitro_timer,0);return;}
	if (car!=NULL)
	{
		car->nitro-=10;
		if (car->nitro<=0)
			car->nitro=0;
		else glutTimerFunc(100,car_nitro_timer,0);
	}
}
void car_timer(int value)
{
	glutTimerFunc(10,car_timer,0);
	if (!ifstart || ifpause) return;
	if (!car->update(0.01) && !ifover)
		GameOver();
	if (freeAcc)
		car->setThrottle(-0.1);
	if (freeBrake)
		car->setBrake(-0.1);
	if (getbackl)
		if (!car->nonzero(car->angleTyre))
			car->angleTyre=0.0f;
		else if (car->angleTyre>0)
			car->turn(-1.0/180*PI);
	if (getbackr)
		if (!car->nonzero(car->angleTyre))
			car->angleTyre=0.0f;
		else if (car->angleTyre<0)
			car->turn(1.0/180*PI);
	
}
void reshape(int width, int height)
{
	wnd_width=width;wnd_height=height;
	FBO::wnd_width=width;FBO::wnd_height=height;
	aspect=float(width)/height;
	glViewport(0,0,wnd_width,wnd_height);
	fb_StartGame->SetScreen(wnd_width,wnd_height);
	fb_EndGame->SetScreen(wnd_width,wnd_height);
	fb_Title->SetScreen(wnd_width,wnd_height);
	fb_EndGame->SetScreen(wnd_width,wnd_height);
	fb_Speed->SetScreen(wnd_width,wnd_height);
	fb_RPM->SetScreen(wnd_width,wnd_height);
	fb_time->SetScreen(wnd_width,wnd_height);
	fb_nitro->SetScreen(wnd_width,wnd_height);
	fb_Restart->SetScreen(wnd_width,wnd_height);
	fb_Gameover->SetScreen(wnd_width,wnd_height);

}
void kbdown(unsigned char key,int x,int y)
{
	switch (key)
	{
	case 'w': camera->MoveXZ(30);
			break;
	case 'a':camera->MoveXZ_LR(30);
			break;
	case 's':camera->MoveXZ(-30);
			break;
	case 'd':camera->MoveXZ_LR(-30);
			break;
	case 27: if (ifstart)
			 {
				 if (ifpause) ResumeGame();
				 else PauseGame();
			 }
			break;
	case 'p':cout<<"current pos: "<<camera->CameraPos.x<<" "<<camera->CameraPos.y<<" "<<camera->CameraPos.z<<endl;
			cout<<"cur direction: "<<camera->Direction.x<<" "<<camera->Direction.y<<" "<<camera->Direction.z<<endl;
			cout<<"cur up: "<<camera->Up.x<<" "<<camera->Up.y<<" "<<camera->Up.z<<" "<<endl;
			csm.print();
			break;
	case 'x':{//stop_particle=!stop_particle;
		if (car!=NULL && nitroRemain>=100)
		{
			car->nitro+=100;
			nitroRemain-=100;
			glutTimerFunc(100,car_nitro_timer,0);
		}
		break;
			 }
	case 't':iftest=!iftest;
		break;
	case 'z':car->print();
		break;
	case 'h':ifHDR=!ifHDR;break;
	case 'm':HasShadow=!HasShadow;break;
	}
}
void kbspdown(int key, int x, int y)
{
	if (!ifstart || ifpause || ifover) return;
	switch (key)
	{
	case GLUT_KEY_UP:
			car->setThrottle(0.1);
			freeAcc=false;
			if (speedup==false)
			{ 
				playMusic(op_speedup,START_MUSIC,true);
				playMusic(op_running,STOP_MUSIC,true);
				speedup=true;
			}
			break;
	case GLUT_KEY_DOWN: 
			car->setBrake(0.1);
			freeBrake=false;
			speed-=0.1;
			break;
	case GLUT_KEY_LEFT:
			car->turn(2.0/180*PI);
			rotateXZ+=0.1;
			getbackl=false;
			break;
	case GLUT_KEY_RIGHT:
			car->turn(-2.0/180*PI);
			rotateXZ-=0.1;
			getbackr=false;
			break;
	}
}
void kbspup(int key, int x, int y)
{
	if (!ifstart || ifpause || ifover) return;
	switch (key)
	{
	case GLUT_KEY_UP:freeAcc=true;
		if (speedup==true)
		{
			playMusic(op_running,START_MUSIC,true);
			playMusic(op_speedup,STOP_MUSIC,true);
			speedup = false;
		}
			break;
	case GLUT_KEY_DOWN:freeBrake=true;
			break;
	case GLUT_KEY_LEFT:getbackl=true;
			break;
	case GLUT_KEY_RIGHT:getbackr=true;
			break;
	}
}
void MouseMoveEvent(int x,int y)
{
	//camera->OnMouseMove(x,y);
}
void MouseFunc(int button, int state, int x, int y)
{
	if (button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
	{
		fb_StartGame->OnMouseDown(x,y);
		fb_EndGame->OnMouseDown(x,y);
		fb_Resume->OnMouseDown(x,y);
		fb_Restart->OnMouseDown(x,y);
	}
	if (button==GLUT_LEFT_BUTTON && state==GLUT_UP)
	{
		fb_StartGame->OnMouseUp(x,y);
		fb_EndGame->OnMouseUp(x,y);
		fb_Resume->OnMouseUp(x,y);
		fb_Restart->OnMouseUp(x,y);
	}
}

int main(int argc, char **argv)
{
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowSize(wnd_width,wnd_height);
	//glutInitContextVersion(4,0);
	//glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow(argv[0]);
	
	//if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE))
		//glutEnterGameMode();

	if (glewInit())
	{
		cerr<<"Fail to initialize GLEW"<<endl;
		exit(0);
	}
	init();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(kbdown);
	glutSpecialFunc(kbspdown);
	glutSpecialUpFunc(kbspup);
	glutPassiveMotionFunc(MouseMoveEvent);
	glutMouseFunc(MouseFunc);
	glutIdleFunc(display);
	glutTimerFunc(10,car_timer,0);
	glutMainLoop();
}