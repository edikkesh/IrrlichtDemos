#pragma once
#include "Car.h"


class PlayerCar: public Car
{									// ������ ������ ������ �� ������������� ������ � �������� ��������� (�������� ����������� ���� ���� ��������� � ������ �������)
									// �� ���� �� ������� ������ ����� ���� �������� �� �����
private:
	
	
	bool isCameraLocked;	

	
public:
	
	PlayerCar(IrrlichtDevice* irrDevice,CIrrOdeWorld *world, const irr::io::path & modelFileName = "" ,const irr::io::path & textureFileName = "", MyEventReceiver* rec = 0):Car(irrDevice,world, modelFileName,textureFileName,rec)
	{
		isCameraLocked = 0; // ���������� ����������� ��������� ������ - ��������� � ������ ��� ���
		//sceneNode->setPosition(vector3df(2155,220,600));
	};

	


	
	
	void setCameraToFollowing();	//���������� ������ ����� �� ������ ���������� �� �������
	void ChangeCameraMode();	// �������� ������ �� ����������� � ������ ��� ���

	void PRESSED_KEY_W();			//������� ������ �� ������� ������������� ������ ( ���, ������, �������)
	void PRESSED_KEY_S();
	void PRESSED_KEY_A();	
	void PRESSED_KEY_D();
	void PRESSED_KEY_SPACE();
	
	void Update();
	
	
};

