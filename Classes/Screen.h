//
//  Screen.h
//  MapEditorSample
//
//  Created by kondo on 2014/07/15.
//
//

#ifndef MapEditorSample_Screen_h
#define MapEditorSample_Screen_h

#include "cocos2d.h"


#define Sw cocos2d::Director::getInstance()->getWinSize().width
#define Sh cocos2d::Director::getInstance()->getWinSize().height
#define CenterAnchor cocos2d::Vec2(0.5f, 0.5f)

namespace Screen
{
    enum
    {
        Back_Back,
        Back_Middle,
        Back_Fore,
        Middle_Back,
        Middle_Middle,
        Middle_Fore,
        Fore_Back,
        Fore_Middle,
        Fore_Fore
    };
}

namespace GameScreenSize
{
    const int width = 750;
    const int height = 840;
    const int offsetX = 0;
    const int offsetY = 240;
    const float diagonal = 1126.1f;
}

#endif
