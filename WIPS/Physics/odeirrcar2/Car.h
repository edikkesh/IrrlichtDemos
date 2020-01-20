#pragma once
#include "includes.h"
#include "MyEventReceiver.h"
#include "Wheel.h"
#define GT() dev->getTimer()->getTime() 

class Car
{									// ������ ������ ������ �� ������������� ������ � �������� ��������� (�������� ����������� ���� ���� ��������� � ������ �������)
									// �� ���� �� ������� ������ ����� ���� �������� �� �����
protected:
public:
	core::vector3df rotation;		// ������ �������� ������ �� ����� �������(������������ ������ ��� ����� ���,
									//������� �������� �������� �� ������� � ����� ����������)
	static const f32 SLOW_COEF;		// ����������� ���������� ��� �������� ���������� ������� = 0.01
									// �.� �������� ���������� �������� ���������� ������ 10 �� �� ��� ���������� �������
									//� ���������� � ������� (�/�) ������������ ���� �����
	static const f32 REAL_COEF;		// ����������� ��� ����������� �������� (����� �������� ����������� ������ �������� �����. �� �������

	char* name;						// ��� ������ ����� �� ������������
	f32 weight;						// ��� ������ ����� ����� ��� ���������� ����������� ��������
	f32 acceleration;				//��������� ��������� �������� � �/�2
	f32 brakeDeceleration;			//���������� � �/�2
	f32 collisionDamage;			// ���� ������ ��� ��������� ������� �� ������������ (����� 3-� ������������)
	f32 speed;						// �������� ������
	const f32 maxSpeed;				// ������������ �������� ������ ����� �������� 120 ��/� ��� 35 �/�
	f32 steeringCoef;				// ����������� ������� - ���� �������� ���� � ��������
	u32 lastActionTime;				// �� ����������� �������������� �����
	f32 collisionCoef;				// ����������� ������ � ������� ����� ����� ��� ���������� ����������� ��������

	IAnimatedMesh* mesh;			// ������ ������
	scene::ISceneNode *sceneNode;	// ������������� ����� ����� ������
	ITexture* texture;				// �������� ������
	
	CIrrOdeBody * carBody;                                       /*����� ����� ��������� ����(��������)*/
	CIrrOdeGeomBox *carGeom;                                    /*����� ����� ��������� ����-�������(��������)*/
	CIrrOdeWorld *worldNode;


	
	IrrlichtDevice* dev;			//�������� �� ������ ��� �������������
	video::IVideoDriver* driver;	//��� ��������� ��������� �������� (����� �� ������������ dev->getVideoDriver() ������ ���
	scene::ISceneManager* smgr;		// ��� ��������� ��������
	
	Wheel *wheels[4];

private:

public:
	MyEventReceiver* receiver;// �� ������������ �������������� �������������
	Car(IrrlichtDevice* irrDevice,CIrrOdeWorld *world, const irr::io::path & modelFileName = "" ,const irr::io::path & textureFileName = "", MyEventReceiver* rec = 0);

	void SetMesh(const irr::io::path & modelFileName);// ������� ��������� ������ (� ������ ����� ������ ���������� ����� ����� ������� ������)

	void SetTexture(const irr::io::path & textureFileName );// ��������� �������� ��� ������� ������
	
	void OnCollision();				// ��� ��������� ������� ��� ������������ � �������� ������������
	void setCameraToFollowing();	//���������� ������ ����� �� ������ ���������� �� �������
	void Car::ChangeCameraMode();	// �������� ������ �� ����������� � ������ ��� ���
	virtual void Update();				//��������� ��������� ������ �������� �� �� ��������, ���� � �.�.
	
	
	vector3df getPosition(){return sceneNode->getPosition();}
	bool IsCollision(ISceneNode* one)
	{
	 return (one->getTransformedBoundingBox().intersectsWithBox(this->sceneNode->getTransformedBoundingBox()));
	}
	~Car();
	
};

