//
//  Wall.h
//  MapEditorSample
//
//  Author: Mat Buckland 2002 (fup@ai-junkie.com)
//
//

#ifndef MapEditorSample_Wall_h
#define MapEditorSample_Wall_h

#include "cocos2d.h"
#include "Screen.h"

#include <fstream>


class Wall
{
protected:
    
    cocos2d::Vec2 m_From;
    cocos2d::Vec2 m_To;
    cocos2d::Vec2 m_NormalVector;
    
    void calculateNormalVector()
    {
        cocos2d::Vec2 tempVector = m_To - m_From;
        m_NormalVector = tempVector.getPerp();
    }
    
public:
    
    Wall(){}
    
    Wall(cocos2d::Vec2 From, cocos2d::Vec2 To)
    :m_From(From)
    ,m_To(To)
    {
        calculateNormalVector();
    }
    
    Wall(cocos2d::Vec2 From, cocos2d::Vec2 To, cocos2d::Vec2 normalVector)
    :m_From(From)
    ,m_To(To)
    ,m_NormalVector(normalVector)
    {}
    
    Wall(std::ifstream &ifs,
         float offsetX = 0.0f,
         float offsetY = 0.0f)
    {
        read(ifs, offsetX, offsetY);
    }
    
    cocos2d::Vec2 getFrom()const{return m_From;}
    void setFrom(const cocos2d::Vec2 &point){m_From = point; calculateNormalVector();}
    
    cocos2d::Vec2 getTo()const{return m_To;}
    void setTo(const cocos2d::Vec2 &point){m_To = point; calculateNormalVector();}
    
    cocos2d::Vec2 getNormalVector()const{return m_NormalVector;}
    
    cocos2d::Vec2 getCenter()const{return (m_From+m_To)/2.0f;}
    
    void read(std::ifstream &ifs, float offsetX, float offsetY)
    {
        float x, y;
        
        ifs >> x >> y;
        x += offsetX;
        y += offsetY;
        m_From = cocos2d::Vec2(x, y);
        
        ifs >> x >> y;
        x += offsetX;
        y += offsetY;
        m_To = cocos2d::Vec2(x, y);
        
        ifs >> x >> y;
        m_NormalVector = cocos2d::Vec2(x, y);
    }
};

#endif
