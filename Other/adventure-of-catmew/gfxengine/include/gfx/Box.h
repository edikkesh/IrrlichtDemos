// Copyright (C) 2007 Kevin van Leeuwen
// This file is part of the "LeeuwenGfx" Library.
// For conditions of distribution and use, see copyright notice in LeeuwenGfx.h

#ifndef _BoxGfxHeader_
#define _BoxGfxHeader_

#include "gfx/Vector.h"

namespace lew
{
	enum E_POINT
	{
		EP_TLEFT,
		EP_TRIGHT,
		EP_BRIGHT,
		EP_BLEFT,
		
		EP_COUNT		
	};

	namespace gfx
	{
		/*
		OPENGL COORDINATE SYSTEM
		-------------------------


					1	|	 / -1
						|	/
					  Y	|  /  Z
						| /
					-1	|/
			-1	--------- 1
			        X
		*/

		class Box
		{
		public:
			Box(const Vector p1, const Vector p2, const Vector p3,
				const Vector p4)
			{
				point[0] = p1;	//Top Left
				point[1] = p2;	//Top Right
				point[2] = p3;	//Bottom Right
				point[3] = p4;	//Bottom Left
				
				TopLeft = &point[0];
				TopRight = &point[1];
				BottomRight = &point[2];
				BottomLeft = &point[3];
			}
			
			Box(const Vector pMin = Vector(-0.5f, -0.5f), 
				const Vector pMax = Vector(0.5f, 0.5f))
			{
				point[0] = Vector(pMin.x, pMax.y);		//Top Left
				point[1] = pMax;						//Top Right
				point[2] = Vector(pMax.x, pMin.y);		//Bottom Right
				point[3] = pMin;						//Bottom Left
				
				TopLeft = &point[0];
				TopRight = &point[1];
				BottomRight = &point[2];
				BottomLeft = &point[3];
			}
			
			Vector getMin()
			{
				return point[3];
			}
			
			Vector getMax()
			{
				return point[1];
			}
			
			Vector getExtent()
			{
				return getMin().difference(getMax());
			}

			bool isPointInside(const Vector pPoint) const
			{
				if(pPoint.x > point[0].x && pPoint.x < point[1].x)
				{
					if(pPoint.y > point[2].y && pPoint.y < point[1].y)
						return true;
				}

				return false;
			}

			bool isBoxInside(const gfx::Box pBox) const
			{
				for(int i = 0; i != 4; ++i)
				{
					if(!isPointInside(pBox.point[i]))
						return false;
				}

				return true;
			}

			bool isBoxIntersecting(const gfx::Box pBox) const
			{
				for(int i = 0; i != 4; ++i)
				{
					if(isPointInside(pBox.point[i]))
						return true;
				}
				
				//in case the edges of the two boxes intersect but 
				//there are no points inside
				/*
				if (TopLeft < other.BottomRight) and if (BottomRight > other.TopLeft)
					intersecting edges
				*/
				
				if(point[0].x <= pBox.point[2].x && point[0].y >= pBox.point[2].y)
				{
					if(point[2].x >= pBox.point[0].x && point[2].y <= pBox.point[0].y)
						return true;
				}
				
				return false;
			}
			
			void rotate(const float pAngle)
			{
				for(int i = 0; i != 4; ++i)
				{
					point[i] = Vector(
						point[i].x*cos(pAngle*DEG2RAD) - point[i].y*sin(pAngle*DEG2RAD),
						point[i].x*sin(pAngle*DEG2RAD) + point[i].y*cos(pAngle*DEG2RAD));
				}
			}

			Vector point[4];
			
			Vector* TopLeft;
			Vector* TopRight;
			Vector* BottomRight;
			Vector* BottomLeft;
		};
	};
};

#endif

