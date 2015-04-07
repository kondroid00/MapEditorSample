//
//  TrackCircle.h
//  MapEditorSample
//
//  Created by kondo on 2015/04/05.
//
//

#ifndef __MapEditorSample__TrackCircle__
#define __MapEditorSample__TrackCircle__

#include "cocos2d.h"


class MainLayer;

class TrackCircle : public cocos2d::Layer
{
private:
    
    static const float CIRCLE_RADIUS;
    static const float UPPER_LIMMIT_POS_Y;
    static const float LOWER_LIMMIT_POS_Y;
    static const float SENSORD_POS_Y;
    static const float ACCELERATION;
    static const float MAX_SPEED;
    static const float SENSORD_MARGIN;
    static const float MAX_NODE_DISTANCE;
    static const float MIN_NODE_DISTANCE;
    
private:
    
    enum class State
    {
        hide,
        show,
        using_circle,
        idling
    };
    
public:
    
    inline std::string showState()
    {
        switch(m_State)
        {
            case State::hide:
                return "TrackCircle : State hide";
                break;
                
            case State::show:
                return "TrackCircle : State show";
                break;
                
            case State::using_circle:
                return "TrackCircle : State using_circle";
                break;
                
            case State::idling:
                return "TrackCircle : State idling";
                
            default:
                return "TrackCircle : None";
                break;
        }
    }
    
private:
    
    State m_State;
    
    MainLayer *m_pMainLayer;
    
    cocos2d::DrawNode *m_pCircle;
    cocos2d::Label *m_pDistanceLabel;
    
    cocos2d::DrawNode *m_pLeftDot;
    cocos2d::DrawNode *m_pRightDot;
    cocos2d::DrawNode *m_pLine;
    
    float m_NodeDistance;
    
    float m_FormerTouchPointY;
    
    float m_Acceleration;
    float m_Speed;
    
    std::chrono::system_clock::time_point m_FormerTouchTime;
    
    cocos2d::Vec2 m_FormerVecFromCenter;
    
    void moveToShow(float dt);
    void moveToHide(float dt);
    
    float getInitialSpeed(float currentPosY)const;
    
    void changeCircleColor(const cocos2d::Color4F &color);
    
    bool checkTouchingCircle(cocos2d::Touch *pTouch)const;
    void changeNodeDistance(cocos2d::Touch *pTouch);
    void drawDistanceNode();
    
public:
    
    TrackCircle()
    :m_NodeDistance(30.0f)
    ,m_FormerTouchPointY(0.0f)
    ,m_Speed(0.0f)
    ,m_Acceleration(ACCELERATION)
    ,m_State(State::idling)
    {}
    
    ~TrackCircle()
    {}
    
    static TrackCircle* create(MainLayer *pMainLayer);
    
    virtual bool init(MainLayer *pMainLayer);
    
    void update(float dt);
    
    //cocos2d::Layerのタッチメソッドをオーバライドしない
    bool onTouchBegan(cocos2d::Touch *pTouch);
    void onTouchMoved(cocos2d::Touch *pTouch);
    void onTouchEnded(cocos2d::Touch *pTouch);
    
    float getNodeDistance()const{return m_NodeDistance;}
};

#endif /* defined(__MapEditorSample__TrackCircle__) */
