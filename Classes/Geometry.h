//
//  Geometry.h
//  MapEditorSample
//
//  Created by kondo on 2014/08/04.
//
//

#ifndef MapEditorSample_Geometry_h
#define MapEditorSample_Geometry_h

#include "cocos2d.h"
#include "Utils.h"

#include <math.h>


namespace Geometry
{
    inline float distToLineSegment(const cocos2d::Vec2 &A,
                                   const cocos2d::Vec2 &B,
                                   const cocos2d::Vec2 &P)
    {
        float dotA = (P.x-A.x)*(B.x-A.x)+(P.y-A.y)*(B.y-A.y);
        
        if(dotA <= 0.0f) return A.getDistance(P);
        
        float dotB = (P.x-B.x)*(A.x-B.x)+(P.y-B.y)*(A.y-B.y);
        
        if(dotB <= 0.0f) return B.getDistance(P);
        
        cocos2d::Vec2 point = A + (B - A) * (dotA / (dotA + dotB));
        
        return P.distance(point);
    }
    
    
    inline bool lineIntersection(const cocos2d::Vec2 &A,
                                 const cocos2d::Vec2 &B,
                                 const cocos2d::Vec2 &C,
                                 const cocos2d::Vec2 &D,
                                 float &dist,
                                 cocos2d::Vec2 &point)
    {
        float rTop = (A.y-C.y)*(D.x-C.x)-(A.x-C.x)*(D.y-C.y);
        float sTop = (A.y-C.y)*(B.x-A.x)-(A.x-C.x)*(B.y-A.y);
        float bot = (B.x-A.x)*(D.y-C.y)-(B.y-A.y)*(D.x-C.x);
        
        if (bot == 0)
        {
            return false;
        }
        
        float r = rTop/bot;
        float s = sTop/bot;
        
        if( (r > 0) && (r < 1) && (s > 0) && (s < 1) )
        {
            dist = A.distance(B) * r;
            point = A + r * (B - A);
            return true;
        }
        else
        {
            dist = 0;
            return false;
        }
    }
    
    
    inline bool crossWithinDegree(const cocos2d::Vec2 &A,
                                  const cocos2d::Vec2 &B,
                                  float degree)
    {
        auto NA = A.getNormalized();
        auto NB = B.getNormalized();
        
        auto dot = NA.x * NB.x + NA.y * NB.y;
        auto a = cosf(Utils::degreeToRadian(degree));
        
        if(dot > cosf(Utils::degreeToRadian(degree)))
        {
            return true;
        }
        return false;
    }
}

#endif
