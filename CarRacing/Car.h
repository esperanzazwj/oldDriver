#ifndef _CAR_H_
#define _CAR_H_
#include "StdAfx.h"
typedef float (*FuncType)(float);
typedef struct CarInfo_tag
{
	float Mass;
	float Inertia;
	float frontTorque;
	float rearTorque;
	float totTorque;
	float suspendHeight;
	float gearRatio[5];
	float differentialRatio;
	float efficiency;
	float tyreRadius;
	float cRR;
	float dragFactor;//-0.5*p*A*Cd;
	float brakeFactor;

} CarInfo;

CarInfo DefaultCarInfo=
{
	1500,
	30000,
	1.25,
	1.25,
	2.5,
	0.5,
	{2.66,1.78,1.30,1.0,0.74},
	3.38,
	0.9,
	0.33,
	0.1,
	0.46,
	10000,
};

float DefaultFuncRPM(float RPM)
{
	if (RPM<=1000)
		return 300;
	else if (RPM>1000 && RPM<=4000)
		return RPM/30+266.667;
	else return -RPM/20+600;
}
float DefaultFuncLateralForce(float slipAngle)
{
	const float PI=3.1415926;
	float slipAngleDegree=slipAngle/PI*180;
	if (slipAngleDegree<-5) slipAngleDegree=-5;//5
	else if (slipAngleDegree>5) slipAngleDegree=5;
	return 0.24*slipAngleDegree;
}

class Car
{
public:
	const static float g;
	const static float PI;
	const static float PI2;
	const static float eps;
	CarInfo ci;
	float RPM;
	float minRPM;
	int gear;
	float throttle;
	float brake;
	glm::vec3 position;
	glm::vec3 carDirection;
	glm::vec3 upDirection;
	glm::vec3 carHorizontal;
	glm::vec3 velocityDirection;
	float velocity;
	float accelaration;
	float angleXZ;
	float angleVelXZ;
	float angleY;
	float angleTyre;
	FuncType FuncRPM;
	FuncType FuncLateralForce;
	glm::vec4 orgDirection;
	int nitro;
public:
	//collide
	Base *CarObjPointer;
	int CarColliderNum;
	int *CarColliderIndex;
	Base *roadObjPointer;
	int blockNum;
	int *blockIndex;
	Base *final;
	//set
	bool preserveOldPos;
	glm::vec3 oldposition;
	float oldangleXZ;
public:
	inline bool nonzero(glm::vec3 vec)
	{
		if (vec.x<eps && vec.x>-eps 
			&& vec.y<eps && vec.y>-eps
			&& vec.z<eps && vec.z>-eps)
			return false;
		else return true;
	}
	inline bool nonzero(float x)
	{
		if (x<eps && x>-eps)
			return false;
		else return true;
	}
	inline bool nonone(float x)
	{
		if (x>1.0-eps && x<1.0+eps)
			return false;
		else return true;
	}
public:
	void SetCollisionInfo(Base *_CarObjPointer, int _CarColliderNum, int *_CarColliderIndex,
		Base *_roadObjPointer, int _blockNum, int *_blockIndex, Base *_final)
	{
		CarObjPointer=_CarObjPointer;
		CarColliderNum=_CarColliderNum;
		CarColliderIndex=_CarColliderIndex;
		roadObjPointer=_roadObjPointer;
		blockNum=_blockNum;
		blockIndex=_blockIndex;
		final=_final;
	}
	Car(const CarInfo &ci=DefaultCarInfo, FuncType FuncRPM=DefaultFuncRPM, 
		FuncType FuncLateralForce=DefaultFuncLateralForce, glm::vec3 orgDirection=glm::vec3(0.0f,0.0f,1.0f)):
		 ci(ci),FuncRPM(FuncRPM),FuncLateralForce(FuncLateralForce)
	{
		minRPM=1000;
		RPM=0;
		gear=0;
		throttle=0;
		brake=0;
		//position=glm::vec3(0.0f,0.0f,0.0f);
		position=glm::vec3(707.092f,0.8f,-1381.39f);
		orgDirection=glm::vec3(1.0f,0.0f,0.0f);
		orgDirection=glm::normalize(orgDirection);
		velocityDirection=orgDirection;
		velocity=0.0f;
		accelaration=0.0f;
		velocity=0.0f;
		angleXZ=0;
		angleVelXZ=0;
		angleY=0;
		angleTyre=0;
		this->orgDirection=glm::vec4(orgDirection,0.0f);
		carDirection=orgDirection;
		//---------------
		CarObjPointer=NULL;
		CarColliderNum=0;
		CarColliderIndex=NULL;
		roadObjPointer=NULL;
		blockNum=0;
		blockIndex=NULL;
		//-----------------
		preserveOldPos=true;
		oldposition=position;
		nitro=0;
	}
	void turn(float angle)
	{
		angleTyre+=angle;
		if (angleTyre>PI/6) angleTyre=PI/6;
		else if (angleTyre<-PI/6) angleTyre=-PI/6;
	}
	void setThrottle(float _throttle)
	{
		throttle+=_throttle;
		if (throttle>1) throttle=1;
		else if (throttle<0) throttle=0;
	}
	void setBrake(float _brake)
	{
		brake+=_brake;
		if (brake>1) brake=1;
		else if (brake<0) brake=0;
	}
	bool update(float delta)
	{
		upDirection=glm::vec3(0.0f,1.0f,0.0f);
		glm::vec4 carDirection4=glm::rotate(glm::mat4(1.0f),angleXZ,upDirection)*orgDirection;
		carDirection=glm::normalize(glm::vec3(carDirection4.x,carDirection4.y,carDirection4.z));
		carHorizontal=glm::normalize(glm::cross(upDirection,carDirection));
		glm::mat3 carTransform(carDirection,carHorizontal,upDirection);
		glm::mat3 carTransformT(glm::transpose(carTransform));
		float wheelLoadFront=ci.Mass*g*ci.rearTorque/ci.totTorque-ci.suspendHeight/ci.totTorque*ci.Mass*accelaration;
		float wheelLoadRear=ci.Mass*g*ci.frontTorque/ci.totTorque+ci.suspendHeight/ci.totTorque*ci.Mass*accelaration;
		glm::vec3 velLocal=carTransformT*velocityDirection*velocity;
		float vLong=velLocal.x;
		float vLat=velLocal.y;
		float dragLong=-ci.dragFactor*vLong*fabs(vLong);
		float dragLat=-ci.dragFactor*vLat*fabs(vLat);
		float rollingResistanceFrontLong=0;
		float rollingResistanceFrontLat=0;
		float rollingResistanceRearLong=0;
		if (nonzero(velocity))
		{
			rollingResistanceFrontLong=-sgn(vLong)*wheelLoadFront * ci.cRR * cos(angleTyre);
			rollingResistanceFrontLat=-sgn(vLat)*wheelLoadFront * ci.cRR * sin(angleTyre);
			rollingResistanceRearLong=-sgn(vLong)*wheelLoadRear * ci.cRR;
		}
		float fraction=FuncRPM(RPM)*ci.gearRatio[gear]*ci.differentialRatio*ci.efficiency/ci.tyreRadius*throttle;
		float brakeForce=0;
		//if (nonzero(velocity))
			//brakeForce=-sgn(vLong)*ci.brakeFactor*brake;
		brakeForce=-ci.brakeFactor*brake;
		float slipAngleFront=0;
		if (nonzero(vLong))
			slipAngleFront=atan((vLat + angleVelXZ*ci.frontTorque)/fabs(vLong))-sgn(vLong)*angleTyre;
		float fnFront=FuncLateralForce(slipAngleFront);
		float lateralForceFrontLong=fnFront*wheelLoadFront*sin(angleTyre);
		float lateralForceFrontLat=-fnFront*wheelLoadFront*cos(angleTyre);
		float slipAngleRear=0; 
		if (nonzero(vLong))
			slipAngleRear=atan((vLat-angleVelXZ*ci.frontTorque)/fabs(vLong));
		float fnRear=FuncLateralForce(slipAngleRear);
		float lateralForceRear=-fnRear*wheelLoadRear;

		float nitroAcc=nitro>0?50000.0f:0.0f;

		float netForceLong=rollingResistanceFrontLong+lateralForceFrontLong+rollingResistanceRearLong+dragLong+fraction+brakeForce+nitroAcc;
		float netForceLat=rollingResistanceFrontLat+lateralForceFrontLat+dragLat+lateralForceRear;//+centripetalForce;
		float aLong=netForceLong/ci.Mass;
		float aLat=netForceLat/ci.Mass;
		accelaration=aLong;
		vLong+=aLong*delta;
		vLat+=aLat*delta;
		glm::vec3 new_velocity_local(vLong,vLat,0.0f);
		glm::vec3 new_velocity=carTransform*new_velocity_local;
		if (nonzero(new_velocity))
		{
			velocityDirection=glm::normalize(new_velocity);
			velocity=glm::dot(velocityDirection,new_velocity);
		}
		else
		{
			velocityDirection=glm::vec3(orgDirection.x,orgDirection.y,orgDirection.z);
			velocity=0.0f;
		}
		float torqueBody=(lateralForceFrontLat+rollingResistanceFrontLat) * ci.frontTorque-lateralForceRear*ci.rearTorque;
		float angleAcc=torqueBody/ci.Inertia;

		float carDirectionVelocity=glm::dot(velocityDirection,carDirection)*velocity;
		if (nonzero(carDirectionVelocity))
			angleVelXZ+=angleAcc*delta;
		else angleVelXZ=0.0f;

		glm::vec3 new_position=position+new_velocity*delta*20.0f;
		float new_angleXZ=angleXZ+angleVelXZ*delta*20.0f;
		//---------------------collide test------------------
		for (int i=0;i<CarColliderNum;i++)
		{
			glm::mat4 TransformMat=glm::translate(glm::mat4(1.0f),new_position)
						*glm::rotate(glm::mat4(1.0f),new_angleXZ,glm::vec3(0,1,0))
						*glm::rotate(glm::mat4(1.0f),PI/2,glm::vec3(0,1,0))
						*glm::scale(glm::mat4(1.0f),glm::vec3(0.015f,0.015f,0.015f))
						*CarObjPointer[CarColliderIndex[i]].transformMat;
			CarObjPointer[CarColliderIndex[i]].obb->SetTrans(TransformMat);
		}
		bool ifintersect=false;int blockNumber=0;
		for (int i=0;i<CarColliderNum;i++)
			for (int j=0;j<blockNum;j++)
				if (CarObjPointer[CarColliderIndex[i]].obb->intersect(*(roadObjPointer[blockIndex[j]].obb)))
				{
					ifintersect=true;blockNumber=j;
					break;
				}
		if (!ifintersect)
		{
			position=new_position;
			angleXZ=new_angleXZ;
		}
		else 
		{
			glm::vec3 normal=roadObjPointer[blockIndex[blockNumber]].obb->Normal;
			glm::vec3 vel=velocity*velocityDirection;
			vel-=1.2f*glm::dot(vel,normal)*normal;
			velocityDirection=glm::normalize(vel);
			velocity=glm::dot(velocityDirection,vel);
		}
		//-----------------test final---------------
		bool running=true;
		for (int i=0;i<CarColliderNum;i++)
			if (CarObjPointer[CarColliderIndex[i]].obb->intersect(*final->obb))
			{
				running=false;
				break;
			}
		//angleXZ+=angleVelXZ*delta*10.0f;
		if (angleXZ<0) angleXZ+=(int(-angleXZ/PI2)+1)*PI2;
		if (angleXZ>2*PI) angleXZ-=int(angleXZ/PI2)*PI2;
		while (calRPM());
		return running;
	}
	bool calRPM()
	{
		float wheelRotationRate=velocity/ci.tyreRadius;
		RPM=wheelRotationRate*ci.gearRatio[gear]*ci.differentialRatio*60/(2*PI);
		if (RPM<minRPM) 
		{
			if (gear==0)
				RPM=minRPM;
			else 
			{
				gear--;
				return true;
			}
		}
		else if (RPM>7000 && gear<5) 
				{
					gear++;return true;
			}
		return false;
	}
	glm::vec3 GetPos()
	{
		return glm::vec3(position.x,position.y,position.z);
	}
	void print()
	{
		using namespace std;
		cout<<"state:"<<endl;
		cout<<"gear"<<gear<<endl;
		cout<<"RPM: "<<RPM<<endl;
		cout<<"throttle"<<throttle<<endl;
		cout<<"brake"<<brake<<endl;
		cout<<"position: ";
		for (int i=0;i<3;i++)
			cout<<position[i]<<" ";
		cout<<endl;
		cout<<"velocityDirection: ";
		for (int i=0;i<3;i++)
			cout<<velocityDirection[i]<<" ";
		cout<<endl;
		cout<<"velocity"<<velocity<<endl;
		cout<<"XZ"<<angleXZ<<endl;
		cout<<"angleVelXZ"<<angleVelXZ<<endl;
		cout<<"angleY"<<angleY<<endl;
		cout<<"angleTyre"<<angleTyre<<endl;
	}
	~Car()
	{
	}
};

const float Car::g=9.78;
const float Car::PI=3.1415926;
const float Car::PI2=2*PI;
const float Car::eps=0.005;
#endif