/*
 * Copyright 2011
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ELEMENTIRRLICHT_H_
#define ELEMENTIRRLICHT_H_
#include "ElementItemIrrlicht.h"




class ElementIrrlicht{
public:
	ElementIrrlicht(ElementItemIrrlicht* view);
	ElementIrrlicht();
	virtual ~ElementIrrlicht();

	ElementItemIrrlicht* getView();

    void setView(ElementItemIrrlicht *view);


private:
	ElementItemIrrlicht* view;
};

#endif /* ELEMENTIRRLICHT_H_ */
