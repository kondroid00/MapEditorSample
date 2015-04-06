//
//  MainLayer.h
//  MapEditorSample
//
//  Created by kondo on 2015/04/02.
//
//

#ifndef __MapEditorSample__MainLayer__
#define __MapEditorSample__MainLayer__

#include "cocos2d.h"


class MES_Map;
class MapGraphGenerator;
class TrackCircle;

class MainLayer : public cocos2d::Layer
{
private:
    
    static const float TOUCH_SENSOR_DISTANCE;
    static const float TOUCH_SENSOR_DEGREE;
    static const float HOLDING_INTERVAL_TIME;
    
public:
    
    enum class State
    {
        track_circle,
        map_graph_generator,
        transition_to_track_circle,
        transition_to_map_graph_generator,
        using_circle,
        idling
    };
    
public:
    
    inline std::string showState()
    {
        switch(m_State)
        {
            case State::track_circle:
                return "MainLayer : State track_circle";
                break;
                
            case State::map_graph_generator:
                return "MainLayer : State map_graph_generator";
                break;
                
            case State::transition_to_track_circle:
                return "MainLayer : State transition_to_track_circle";
                break;
                
            case State::transition_to_map_graph_generator:
                return "MainLayer : State transition_to_map_graph_generator";
                break;
                
            case State::using_circle:
                return "MainLayer : State using_circle";
                break;
                
            case State::idling:
                return "MainLayer : State idling";
                break;
                
            default:
                return "MainLayer : None";
                break;
        }
    }
 
private:
    
    State m_State;
    
    MES_Map *m_pMap;
    
    MapGraphGenerator *m_pMapGraphGenerator;
    
    cocos2d::LayerColor *m_pBackground;
    
    cocos2d::RenderTexture *m_pRenderTexture;
    
    cocos2d::Vec2 m_pBeganTouchPosition;
    cocos2d::Vec2 m_pStartingTransitionTouchPosition;
    
    TrackCircle *m_pTrackCircle;
    
    bool m_CheckHolding;
    std::chrono::system_clock::time_point m_TimeAtTouchBegan;
    
    void createScreen();
    void renderWall();
    
    bool checkForCallingTrackCircle(cocos2d::Touch *pTouch);
    bool checkForCallingMapGraphGenerator(cocos2d::Touch *pTouch);
    bool checkForHolding();
    
public:
    
    MainLayer()
    :m_State(State::map_graph_generator)
    ,m_CheckHolding(false)
    {}
    
    ~MainLayer()
    {
        delete  m_pMap;
        delete  m_pMapGraphGenerator;
    };
    
    virtual bool init();
    
    static cocos2d::Scene* createScene();
    
    CREATE_FUNC(MainLayer);
    
    void update(float dt);
    
    virtual bool onTouchBegan(cocos2d::Touch *pTouch,
                              cocos2d::Event *pEvent);
    virtual void onTouchMoved(cocos2d::Touch *pTouch,
                              cocos2d::Event *pEvent);
    virtual void onTouchEnded(cocos2d::Touch *pTouch,
                              cocos2d::Event *pEvent);
    
    void clearMap();
    
    cocos2d::RenderTexture* getRenderTexture()const{return m_pRenderTexture;}
    
    State getState()const{return m_State;}
    void setState(State state){m_State = state;}
    
};

#endif /* defined(__MapEditorSample__MainLayer__) */
