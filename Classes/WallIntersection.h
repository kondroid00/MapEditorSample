//
//  WallIntersection.h
//  MapEditorSample
//
//  Author: Mat Buckland 2002 (fup@ai-junkie.com)
//
//

#ifndef MapEditorSample_WallIntersection_h
#define MapEditorSample_WallIntersection_h

#include "cocos2d.h"
#include "Wall.h"
#include "Geometry.h"
#include "Utils.h"

#include <vector>
#include <iostream>


namespace WallIntersection
{
    template<class T_walls>
    inline Wall* getClosestWall(const cocos2d::Vec2 &position,
                                float &distance,
                                const T_walls &walls)
    {
        Wall *pClosestWall = nullptr;
        float closestDistance = Utils::MAX_FLOAT;
        float dist = Utils::MAX_FLOAT;
        
        for(typename T_walls::const_iterator currentWall = walls.begin();
            currentWall != walls.end();
            ++currentWall)
        {
            dist = Geometry::distToLineSegment((*currentWall)->getFrom(),
                                               (*currentWall)->getTo(),
                                               position);
            
            if(!Utils::isEqual(dist, closestDistance) &&
               dist < closestDistance)
            {
                pClosestWall = *currentWall;
                closestDistance = dist;
            }
        }
        
        distance = closestDistance;
        
        return pClosestWall;
    }
    
    
    template<class T_walls>
    inline bool pointLocatedOnWalls(const cocos2d::Vec2 &point,
                                    const T_walls &walls)
    {
        for(typename T_walls::const_iterator currentWall = walls.begin();
            currentWall != walls.end();
            ++currentWall)
        {
            float distance = Geometry::distToLineSegment((*currentWall)->getFrom(),
                                                         (*currentWall)->getTo(),
                                                         point);
            if(Utils::isZero(distance))
            {
                return true;
            }
        }
        
        return false;
    }
    
    
    template<class T_walls>
    inline bool findClosestPointOfIntersectionWithWalls(const cocos2d::Vec2 &A,
                                                        const cocos2d::Vec2 &B,
                                                        float &distance,
                                                        cocos2d::Vec2 &intersectionPoint,
                                                        const T_walls &walls)
    {
        distance = Utils::MAX_FLOAT;
        bool judge = false;
        
        for(typename T_walls::const_iterator currentWall = walls.begin();
            currentWall != walls.end();
            ++currentWall)
        {
            float tempDistance = 0.0f;
            cocos2d::Vec2 point = cocos2d::Vec2::ZERO;
            
            if(Geometry::lineIntersection(A,
                                          B,
                                          (*currentWall)->getFrom(),
                                          (*currentWall)->getTo(),
                                          tempDistance,
                                          point))
            {
                if(tempDistance < distance)
                {
                    distance = tempDistance;
                    intersectionPoint = point;
                }
                judge = true;
            }
        }
        
        return judge;
    }
}

#endif
