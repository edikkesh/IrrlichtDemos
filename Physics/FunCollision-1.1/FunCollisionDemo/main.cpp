/* -------------------------------------------------------------------------------
    FunCollisionDemo -- Demo application for "FunCollision"

    Created by Peter Szollosi, 2009-2010.

    This software is provided 'as-is', without any express or implied
    warranty. In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgement in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be clearly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
-------------------------------------------------------------------------------- */

#include <irrlicht.h>
using namespace irr;

#include "CGame.h"
using namespace GAME;

int main()
{
    CGame game;

    int ret = game.init();

    if (!ret) ret = game.run();

    return ret;
}
