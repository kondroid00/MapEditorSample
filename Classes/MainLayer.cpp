//
//  MainLayer.cpp
//  MapEditorSample
//
//  Created by kondo on 2015/04/02.
//
//

#include "MainLayer.h"
#include "Screen.h"
#include "Geometry.h"
#include "Utils.h"
#include "Wall.h"
#include "MES_Map.h"
#include "MapGraphGenerator.h"
#include "TrackCircle.h"

#include <chrono>

using namespace cocos2d;


const float MainLayer::TOUCH_SENSOR_DISTANCE = 30.0f;
const float MainLayer::TOUCH_SENSOR_DEGREE   = 20.0f;
const float MainLayer::HOLDING_INTERVAL_TIME = 1.5f;


Scene* MainLayer::createScene()
{
    Scene *pScene = Scene::create();
    Layer *pLayer = MainLayer::create();
    pScene->addChild(pLayer);
    return pScene;
}


bool MainLayer::init()
{
    if(!Layer::init()) return false;
    
    
    auto pListener = EventListenerTouchOneByOne::create();
    pListener->setSwallowTouches(false);
    pListener->onTouchBegan = CC_CALLBACK_2(MainLayer::onTouchBegan, this);
    pListener->onTouchMoved = CC_CALLBACK_2(MainLayer::onTouchMoved, this);
    pListener->onTouchEnded = CC_CALLBACK_2(MainLayer::onTouchEnded, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(pListener, this);
    
    this->scheduleUpdate();
    
    m_pMap = new MES_Map();
    auto wallFileName = "Map/wall.map";
    m_pMap->loadWallFile(FileUtils::getInstance()->fullPathForFilename(wallFileName),
                         35,
                         210);
    
    m_pMapGraphGenerator = new MapGraphGenerator(m_pMap->getWalls(),
                                                 m_pMap->getNodes(),
                                                 m_pMap->getEdges(),
                                                 this);
    
    this->createScreen();
    
    m_TimeAtTouchBegan = std::chrono::system_clock::now();
    
    return true;
}


void MainLayer::createScreen()
{
    m_pBackground = LayerColor::create(Color4B(0, 0, 0, 255),
                                       Sw,
                                       Sh);
    this->addChild(m_pBackground, Screen::Back_Back);
    
    m_pRenderTexture = RenderTexture::create(Sw, Sh);
    m_pRenderTexture->setPosition(Sw * 0.5f, Sh * 0.5f);
    this->addChild(m_pRenderTexture, Screen::Back_Middle);
    
    m_pTrackCircle = TrackCircle::create(this);
    m_pTrackCircle->setAnchorPoint(Vec2::ZERO);
    m_pTrackCircle->setPosition(Vec2(Sw * 0.0f, Sh * 1.0f));
    this->addChild(m_pTrackCircle, Screen::Middle_Back);
    
    renderWall();
}


void MainLayer::clearMap()
{
    m_pRenderTexture->clear(0.0f, 0.0f, 0.0f, 1.0f);
    m_pMap->clearGraph();
    m_pMapGraphGenerator->clear();
    renderWall();
}


void MainLayer::renderWall()
{
    auto color = Color4F(1.0f, 1.0f, 1.0f, 1.0f);
    
    m_pRenderTexture->begin();
    
    for(auto pWall : m_pMap->getWalls())
    {
        auto pDrawLineObject = DrawNode::create();
        pDrawLineObject->drawSegment(pWall->getFrom(),
                                     pWall->getTo(),
                                     2.0f,
                                     color);
        pDrawLineObject->visit();
    }
    
    m_pRenderTexture->end();
}


void MainLayer::update(float dt)
{
    switch(m_State)
    {
        case State::map_graph_generator:
            if(m_CheckHolding && checkForHolding())
            {
                m_State = State::idling;
                clearMap();
                break;
            }
        case State::track_circle:
        case State::transition_to_track_circle:
        case State::transition_to_map_graph_generator:
        case State::using_circle:
            m_pMapGraphGenerator->update();
            m_pTrackCircle->update(dt);
            break;
            
        case State::idling:
            break;
            
        default:
            break;
    }
    log("%-33s %-52s %-40s",
        m_pTrackCircle->showState().c_str(),
        this->showState().c_str(),
        m_pMapGraphGenerator->showState().c_str());
}


bool MainLayer::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    m_pBeganTouchPosition = pTouch->getLocation();
    m_pTrackCircle->onTouchBegan(pTouch);
    m_TimeAtTouchBegan = std::chrono::system_clock::now();
    m_CheckHolding = true;
    return true;
}


void MainLayer::onTouchMoved(cocos2d::Touch *pTouch, cocos2d::Event *pEvent)
{
    switch(m_State)
    {
        case State::map_graph_generator:
            if(checkForCallingTrackCircle(pTouch))
            {
                m_State = State::transition_to_track_circle;
                m_pStartingTransitionTouchPosition = pTouch->getLocation();
                m_pTrackCircle->onTouchBegan(pTouch);
            }
            break;
            
        case State::track_circle:
            if(checkForCallingMapGraphGenerator(pTouch))
            {
                m_State = State::transition_to_map_graph_generator;
                m_pStartingTransitionTouchPosition = pTouch->getLocation();
                m_pTrackCircle->onTouchBegan(pTouch);
            }
            break;
            
        case State::transition_to_track_circle:
        case State::transition_to_map_graph_generator:
        case State::using_circle:
            m_pTrackCircle->onTouchMoved(pTouch);
            break;
            
        case State::idling:
            break;
            
        default:
            break;
    }
}


void MainLayer::onTouchEnded(cocos2d::Touch *pTouch, cocos2d::Event *pEvent)
{
    switch(m_State)
    {
        case State::map_graph_generator:
            clearMap();
            m_pMapGraphGenerator->setSeed(pTouch->getLocation(), m_pTrackCircle->getNodeDistance());
            break;
            
        case State::track_circle:
            break;
            
        case State::transition_to_track_circle:
        case State::transition_to_map_graph_generator:
        case State::using_circle:
            m_pTrackCircle->onTouchEnded(pTouch);
            break;
            
        case State::idling:
            m_State = State::map_graph_generator;
            break;
            
        default:
            break;
    }
    m_CheckHolding = false;
}


bool MainLayer::checkForCallingTrackCircle(Touch *pTouch)
{
    auto vec = pTouch->getLocation() - m_pBeganTouchPosition;
    
    if(vec.getLength() > TOUCH_SENSOR_DISTANCE &&
       Geometry::crossWithinDegree(vec, Vec2(0, -1), TOUCH_SENSOR_DEGREE))
    {
        return true;
    }
    return false;
}


bool MainLayer::checkForCallingMapGraphGenerator(Touch *pTouch)
{
    auto vec = pTouch->getLocation() - m_pBeganTouchPosition;
    
    if(vec.getLength() > TOUCH_SENSOR_DISTANCE &&
       Geometry::crossWithinDegree(vec, Vec2(0, 1), TOUCH_SENSOR_DEGREE))
    {
        return true;
    }
    return false;
}


bool MainLayer::checkForHolding()
{
    using namespace std::chrono;
    auto currentTime = system_clock::now();
    duration<float, std::ratio<1, 1> > diff(duration_cast<seconds>(currentTime - m_TimeAtTouchBegan));
    if(diff.count() > HOLDING_INTERVAL_TIME)
    {
        return true;
    }
    return false;
}

