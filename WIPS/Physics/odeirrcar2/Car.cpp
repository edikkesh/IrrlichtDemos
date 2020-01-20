#include "Car.h"


	Car::Car(IrrlichtDevice* irrDevice,CIrrOdeWorld *world, const irr::io::path & modelFileName  ,const irr::io::path & textureFileName , MyEventReceiver* rec ) : maxSpeed(35)
	{
		worldNode = world;
		this->receiver = rec;
		dev = irrDevice;
		driver = dev->getVideoDriver();
		smgr = dev->getSceneManager();

		mesh = smgr->getMesh(modelFileName);			//���������� ������ �� �����
		texture = driver->getTexture(textureFileName);	//���������� �������� �� �����
		
				

	


		carBody = reinterpret_cast<CIrrOdeBody *>(smgr->addSceneNode(CIrrOdeSceneNode::nodeNameToC8(IRR_ODE_BODY_NAME),worldNode));
		carBody->setPosition(vector3df(0,0,0));

		sceneNode = smgr->addAnimatedMeshSceneNode(mesh,carBody);
		
		
		sceneNode->setScale(sceneNode->getScale()*30.f);
		sceneNode->setMaterialTexture(0,texture);
		sceneNode->setMaterialFlag(video::EMF_LIGHTING, false);

		
	
    
		
		carGeom = reinterpret_cast<CIrrOdeGeomBox *>(smgr->addSceneNode(CIrrOdeSceneNode::nodeNameToC8(IRR_ODE_GEOM_BOX_NAME),sceneNode));
		carGeom->getSurfaceParameters(0)->setMu(1000);		
		//carGeom->setCollide(true);       
		//carGeom->setMassTotal(10);
		carGeom->drop();

			//***************��������� ������*************************
			vector3df wheelsarr[4];// ������ � ������������ �� �����
			/*
			wheelsarr[0] = vector3df(10.5,-6,18);		// ������ ��������
			wheelsarr[1] = vector3df(-10,-6,18);			// ����� ��������
			wheelsarr[2] = vector3df(10.5,-6,-16);		//����� ������
			wheelsarr[3] = vector3df(-10,-6,-16);		//��...
			*/

			wheelsarr[0] = vector3df(15.5,-6,25);		// ������ ��������
			wheelsarr[1] = vector3df(-20,-6,30);			// ����� ��������
			wheelsarr[2] = vector3df(20.5,-6,-26);		//����� ������
			wheelsarr[3] = vector3df(-20,-6,-26);

			for(int i = 0; i < 4; i++)
			{
				wheels[i] = new Wheel(dev,carBody,sceneNode->getPosition(),wheelsarr[i]);
			}
		
			
		//************************************************************
		
		acceleration = 5.3;	//��������� � �/� = 20 ��/�2 �� ���� �� ����� �� 5 ������	
		speed = 0;
		brakeDeceleration = 14;
		collisionDamage = 0;
		
		steeringCoef = 25;// ����������� �������� ���� � ������� � ��������
		rotation.set(0,90,0);
		u32 lastActionTime = GT();		// �� ������������
		collisionCoef = 0.8;		//�������� ����� ������ ������ ��������
	}

const f32 Car::SLOW_COEF = 0.01;
const f32 Car::REAL_COEF = 3;	
	void Car::SetMesh(const irr::io::path & modelFileName)
	{		
		
		mesh = smgr->getMesh(modelFileName);	// ��������� �����
		if(mesh)
		{
			sceneNode->remove();			//������� ������ ������
			sceneNode = smgr->addAnimatedMeshSceneNode(mesh);	
		}

	}
	void Car::SetTexture(const irr::io::path & textureFileName )
	{		
		
		texture = driver->getTexture(textureFileName);			// ���������� ����� ��������
		if(texture)
		sceneNode->setMaterialTexture(0,texture);
	}
	void Car::OnCollision()
	{
		if(++collisionDamage > 3)		//����� ���� ������������ �������� ��������
			sceneNode->setMaterialTexture(0,driver->getTexture("../../media/car_05_damaged.psd"));// ���� ��� ���
	}

	void Car::Update()// ���������� ��� ����� � ����� ��� ����������� ������
	{
		//cout<<speed*3.6<<"kmH"<<endl;
		//sceneNode->setRotation(vector3df(0,rotation.Z,0));
		/*
			vector3df nextPos = sceneNode->getPosition();
			//if( GT() - lastActionTime > 10)		//������ 10 �� ������� �������� �� ������
				//speed = (speed/abs(speed))*abs((abs(speed)-collisionCoef*acceleration*SLOW_COEF));

			nextPos.X += REAL_COEF*SLOW_COEF*speed*sin(rotation.Z/ 57.295785);			
			nextPos.Z += REAL_COEF*SLOW_COEF*speed*cos(rotation.Z/ 57.295785);
			//carBody->setPosition(sceneNode->getPosition());
		//cout<<"np.x = "<<sin(rotation.Z/ 57.295785)<<endl;
		//	sceneNode->setPosition(nextPos);
			for(int i = 0; i < 4; i++)
				wheels[i]->Rotate(sceneNode->getPosition(), /*pow(-1.0,i+1)**///rotation, pow(-1.0,i)*speed);
									
		

	}

	Car::~Car()
	{

		sceneNode->remove();
		for(int i = 0; i < 4; i++)
				delete wheels[i];
		//mesh->drop();
	//	texture->drop();
		
		//sceneNode->drop();
		//delete [] name;
	}
