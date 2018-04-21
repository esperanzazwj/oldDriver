#ifndef _CAMERA_H_
#define _CAMERA_H_
#include "StdAfx.h"
class Camera
{
public:
	static float PI;
	static float AngleVel_XZ;
	static float AngleVel_Y;
	static float Vel_XZ;
	static float Margin;
public:
	int WindowWidth,WindowHeight;
	glm::vec3 CameraPos;
	glm::vec3 Direction;
	glm::vec3 DirectionXZ;
	glm::vec3 HorizonXZ;
	glm::vec3 Up;
	int MousePos_x,MousePos_y;
	float Angle_XZ,Angle_Y;
	bool OnLeftEdge,OnRightEdge,OnTopEdge, OnButtonEdge;
public:
	Camera(int WindowWidth, int WindowHeight, glm::vec3 CameraPos, float Angle_XZ, float Angle_Y):
		WindowWidth(WindowWidth),WindowHeight(WindowHeight),CameraPos(CameraPos),Angle_XZ(Angle_XZ),Angle_Y(Angle_Y)
	{
		CalculateVector();
		MousePos_x=WindowWidth/2;
		MousePos_y=WindowHeight/2;
		glutWarpPointer(MousePos_x, MousePos_y);
	}
	void CalculateVector()
	{
		Direction=glm::vec3(sin(Angle_Y)*cos(Angle_XZ),cos(Angle_Y),sin(Angle_Y)*sin(Angle_XZ));
		DirectionXZ=glm::vec3(Direction.x,0.0,Direction.z);
		HorizonXZ=glm::cross(glm::vec3(Direction.x,fabs(Direction.y),Direction.z),DirectionXZ);
		Up=glm::cross(Direction,HorizonXZ);
	}
	void OnMouseMove(int x,int y)
	{
		int delta_x=x-MousePos_x;
		int delta_y=y-MousePos_y;
		MousePos_x=x;MousePos_y=y;
		Angle_XZ=Angle_XZ+delta_x*AngleVel_XZ;

		float Angle_Y_Next=Angle_Y+delta_y*AngleVel_Y;
		if (Angle_Y_Next>=0 && Angle_Y_Next<=PI)
			Angle_Y=Angle_Y_Next;

		OnLeftEdge=false;
		OnRightEdge=false;
		OnTopEdge=false;
		OnButtonEdge=false;
		if (delta_x==0)
		{
			if (x<Margin)
				OnLeftEdge=true;
			else if (x>WindowWidth-Margin)
				OnRightEdge=true;
		}
		if (delta_y==0)
		{
			if (y<Margin)
				OnTopEdge=true;
			else if (y>WindowHeight-Margin)
				OnButtonEdge=true;
		}

		CalculateVector();
	}
	void OnMoveEdge()
	{
		bool Moved=false;
		if (OnLeftEdge)
		{
			Angle_XZ-=AngleVel_XZ;
			Moved=true;
		}
		else if (OnRightEdge)
		{
			Angle_XZ+=AngleVel_XZ;
			Moved=true;
		}
		else if (OnTopEdge)
		{
			if (Angle_Y-AngleVel_Y>=0)
				Angle_Y-=AngleVel_Y;
			Moved=true;
		}
		else if (OnButtonEdge)
		{
			if (Angle_Y+AngleVel_Y<=PI)
				Angle_Y+=AngleVel_Y;
			Moved=true;
		}
		if (Moved)
			CalculateVector();
	}
	void MoveXZ(int step)
	{
		CameraPos+=step*Vel_XZ*glm::normalize(Direction);
	}
	void MoveXZ_LR(int step)
	{
		CameraPos+=step*Vel_XZ*glm::normalize(HorizonXZ);
	}
	~Camera()
	{
	}
};
float Camera::PI=3.1415926;
float Camera::AngleVel_XZ=0.01;
float Camera::AngleVel_Y=0.01;
float Camera::Vel_XZ=0.1;
float Camera::Margin=10;
#endif