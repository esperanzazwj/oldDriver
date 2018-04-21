#ifndef _StAdfx_H_
#define _StAdfx_H_
#define FREEGLUT_STATIC
#define GLEW_STATIC

#pragma comment (lib, "glew32s.lib")
#pragma comment( lib, "glaux.lib")
#pragma comment (lib,"assimp.lib")
#pragma comment(lib,"freeglut_static.lib")

#include <gl\glew.h>
#include <gl\freeglut.h>
#include <gl\glaux.h>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\matrix_inverse.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\noise.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
//#include <opencv2\opencv.hpp>
#include "nv_dds.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <time.h>
//UI
#pragma comment(lib, "freetype262")
#pragma comment(lib, "freetype262d")
#include <ft2build.h>  
#include <freetype/freetype.h> 
#include <freetype/ftglyph.h>
#include FT_FREETYPE_H
#include <tchar.h>
//Music
#include "mmsystem.h"
#include <Digitalv.h>
#pragma comment(lib,"winmm.lib")

#define START_MUSIC 0
#define STOP_MUSIC 1
#define PAUSE_MUSIC 2
#define RESTART_MUSIC 3

#endif