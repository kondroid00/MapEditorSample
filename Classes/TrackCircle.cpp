//
//  TrackCircle.cpp
//  MapEditorSample
//
//  Created by kondo on 2015/04/05.
//
//

#include "TrackCircle.h"
#include "Screen.h"
#include "Utils.h"
#include "Geometry.h"
#include "MainLayer.h"

#include <chrono>


using namespace cocos2d;


const float TrackCircle::CIRCLE_RADIUS      = 150.0f;
const float TrackCircle::UPPER_LIMMIT_POS_Y = 0.9f;
const float TrackCircle::LOWER_LIMMIT_POS_Y = 0.25f;
const float TrackCircle::SENSORD_POS_Y      = 0.05f;
const float TrackCircle::ACCELERATION       = 20000.0f;
const float TrackCircle::MAX_SPEED          = 5000.0f;
const float TrackCircle::SENSORD_MARGIN     = 20.0f;
const float TrackCircle::MAX_NODE_DISTANCE  = 100.0f;
const float TrackCircle::MIN_NODE_DISTANCE  = 10.0f;


TrackCircle* TrackCircle::create(MainLayer *pMainLayer)
{
    auto pTrackCircle = new(std::nothrow) TrackCircle();
    if(pTrackCircle && pTrackCircle->init(pMainLayer))
    {
        pTrackCircle->autorelease();
        return pTrackCircle;
    }
    delete pTrackCircle;
    pTrackCircle = nullptr;
    return nullptr;
}


bool TrackCircle::init(MainLayer *pMainLayer)
{
    m_pMainLayer = pMainLayer;
    
    auto pLayerColor = LayerColor::create(Color4B(0, 0, 0, 230),
                                          Sw,
                                          Sh * 0.35f);
    pLayerColor->setPosition(Vec2(Sw * 0.0f, Sh * 0.1f));
    this->addChild(pLayerColor);
    
    m_pCircle = DrawNode::create();
    m_pCircle->drawCircle(Vec2::ZERO,
                          CIRCLE_RADIUS,
                          0.0f,
                          100,
                          false,
                          Color4F(1.0f, 1.0f, 1.0f, 1.0f));
    m_pCircle->setAnchorPoint(CenterAnchor);
    m_pCircle->setPosition(Vec2(Sw * 0.5f,
                                Sh * 0.25f));
    this->addChild(m_pCircle);
    
    m_pLeftDot = DrawNode::create();
    m_pLeftDot->drawDot(Vec2(Sw * 0.5f - m_NodeDistance * 0.5f, Sh * 0.4f),
                        3.0f,
                        Color4F(1.0f, 1.0f, 1.0f, 1.0f));
    this->addChild(m_pLeftDot);
    
    m_pRightDot = DrawNode::create();
    m_pRightDot->drawDot(Vec2(Sw * 0.5f + m_NodeDistance * 0.5f, Sh * 0.4f),
                         3.0f,
                         Color4F(1.0f, 1.0f, 1.0f, 1.0f));
    this->addChild(m_pRightDot);
    
    m_pLine = DrawNode::create();
    m_pLine->drawLine(Vec2(Sw * 0.5f - m_NodeDistance * 0.5f, Sh * 0.4f),
                      Vec2(Sw * 0.5f + m_NodeDistance * 0.5f, Sh * 0.4f),
                      Color4F(1.0f, 1.0f, 1.0f, 1.0f));
    this->addChild(m_pLine);
    
    m_pDistanceLabel = Label::createWithTTF(Utils::decimalsToString(m_NodeDistance, 1),
                                            "fonts/arial.ttf",
                                            100.0f);
    m_pDistanceLabel->setAnchorPoint(CenterAnchor);
    m_pDistanceLabel->setPosition(Vec2(Sw * 0.5f,
                                       Sh * 0.25f));
    this->addChild(m_pDistanceLabel);
    
    return true;
}


void TrackCircle::update(float dt)
{
    switch(m_State)
    {
        case State::hide:
            this->moveToHide(dt);
            break;
            
        case State::show:
            this->moveToShow(dt);
            break;
            
        case State::using_circle:
            m_pDistanceLabel->setString(Utils::decimalsToString(m_NodeDistance, 1));
            drawDistanceNode();
            break;
            
        default:
            break;
    }
}


void TrackCircle::moveToShow(float dt)
{
    if(Utils::isEqual(m_Speed, MAX_SPEED))
    {
        m_Speed = MAX_SPEED;
    }
    else if(m_Speed < MAX_SPEED)
    {
        m_Speed += m_Acceleration * dt;
        if(m_Speed > MAX_SPEED)
        {
            m_Speed = MAX_SPEED;
        }
    }
    else if(m_Speed > MAX_SPEED)
    {
        m_Speed -= m_Acceleration * dt;
        if(m_Speed < MAX_SPEED)
        {
            m_Speed = MAX_SPEED;
        }
    }
    
    float setY = this->getPositionY() - m_Speed * dt;
    
    if(setY < Sh * LOWER_LIMMIT_POS_Y)
    {
        setY = Sh * LOWER_LIMMIT_POS_Y;
        m_State = State::idling;
        m_Speed = 0.0f;
        m_pMainLayer->setState(MainLayer::State::track_circle);
    }
    
    this->setPositionY(setY);
}


void TrackCircle::moveToHide(float dt)
{
    if(Utils::isEqual(m_Speed, MAX_SPEED))
    {
        m_Speed = MAX_SPEED;
    }
    else if(m_Speed < MAX_SPEED)
    {
        m_Speed += m_Acceleration * dt;
        if(m_Speed > MAX_SPEED)
        {
            m_Speed = MAX_SPEED;
        }
    }
    else if(m_Speed > MAX_SPEED)
    {
        m_Speed -= m_Acceleration * dt;
        if(m_Speed < MAX_SPEED)
        {
            m_Speed = MAX_SPEED;
        }
    }
    
    float setY = this->getPositionY() + m_Speed * dt;
    
    if(setY > Sh * UPPER_LIMMIT_POS_Y)
    {
        setY = Sh * UPPER_LIMMIT_POS_Y;
        m_State = State::idling;
        m_Speed = 0.0f;
        m_pMainLayer->setState(MainLayer::State::map_graph_generator);
    }
    
    this->setPositionY(setY);
}


bool TrackCircle::onTouchBegan(Touch *pTouch)
{
    switch(m_pMainLayer->getState())
    {
        case MainLayer::State::track_circle:
            if(checkTouchingCircle(pTouch))
            {
                m_State = State::using_circle;
                m_pMainLayer->setState(MainLayer::State::using_circle);
                changeCircleColor(Color4F(1.0f, 0.0f, 0.0f, 1.0f));
                auto center = m_pCircle->getPosition() + this->getPosition();
                m_FormerVecFromCenter = (pTouch->getLocation() - center);
                
            }
            break;
            
        case MainLayer::State::transition_to_track_circle:
        case MainLayer::State::transition_to_map_graph_generator:
            m_FormerTouchPointY = pTouch->getLocation().y;
            break;
            
        default:
            break;
    }
}


void TrackCircle::onTouchMoved(Touch *pTouch)
{
    switch(m_pMainLayer->getState())
    {
        case MainLayer::State::track_circle:
            
            break;
            
        case MainLayer::State::transition_to_track_circle:
        case MainLayer::State::transition_to_map_graph_generator:
        {
            auto touchPointY = pTouch->getLocation().y;
            auto offset = touchPointY - m_FormerTouchPointY;
            auto setPosY = this->getPositionY() + offset;
            
            Utils::clamp(setPosY,
                         Sh * LOWER_LIMMIT_POS_Y,
                         Sh * UPPER_LIMMIT_POS_Y);
            
            this->setPositionY(setPosY);
            m_FormerTouchPointY = touchPointY;
            m_FormerTouchTime = std::chrono::system_clock::now();
            break;
        }
            
        case MainLayer::State::using_circle:
            changeNodeDistance(pTouch);
            
            break;
            
        default:
            break;
    }
}


void TrackCircle::onTouchEnded(Touch *pTouch)
{
    switch(m_pMainLayer->getState())
    {
        case MainLayer::State::track_circle:
            
            break;
            
        case MainLayer::State::transition_to_track_circle:
        {
            m_Speed = getInitialSpeed(pTouch->getLocation().y);
            if(getPositionY() <= Sh * (UPPER_LIMMIT_POS_Y - SENSORD_POS_Y))
            {
                m_State = State::show;
            }
            else
            {
                m_State = State::hide;
            }
            break;
        }
            
        case MainLayer::State::transition_to_map_graph_generator:
        {
            m_Speed = getInitialSpeed(pTouch->getLocation().y);
            if(getPositionY() >= Sh * (LOWER_LIMMIT_POS_Y + SENSORD_POS_Y))
            {
                m_State = State::hide;
            }
            else
            {
                m_State = State::show;
            }
            break;
        }
            
        case MainLayer::State::using_circle:
            m_State = State::idling;
            m_pMainLayer->setState(MainLayer::State::track_circle);
            changeCircleColor(Color4F(1.0f, 1.0f, 1.0f, 1.0f));
            break;
            
        default:
            break;
    }
}


float TrackCircle::getInitialSpeed(float currentPosY)const
{
    using namespace std::chrono;
    auto currentTime = system_clock::now();
    duration<float, std::ratio<1, 1> > diff(duration_cast<microseconds>(currentTime - m_FormerTouchTime));
    if(Utils::isZero(diff.count())) return MAX_SPEED * 2;
    auto speed = (fabsf(currentPosY - m_FormerTouchPointY) / diff.count());
    return speed;
}


void TrackCircle::changeCircleColor(const Color4F &color)
{
    m_pCircle->clear();
    m_pCircle->drawCircle(Vec2::ZERO,
                          CIRCLE_RADIUS,
                          0.0f,
                          100,
                          false,
                          color);
}


bool TrackCircle::checkTouchingCircle(Touch *pTouch)const
{
    auto distFromCenter = (pTouch->getLocation() - (m_pCircle->getPosition() + this->getPosition())).length();
    if(distFromCenter <= CIRCLE_RADIUS * 1.2f)
    {
        return true;
    }
    return false;
}


void TrackCircle::changeNodeDistance(Touch *pTouch)
{
    auto center = m_pCircle->getPosition() + this->getPosition();
    auto pointFromCenter = (pTouch->getLocation() - center);
    
    m_NodeDistance += pointFromCenter.getAngle(m_FormerVecFromCenter) / Utils::TWO_PI * 10.0f;
    
    Utils::clamp(m_NodeDistance, MIN_NODE_DISTANCE, MAX_NODE_DISTANCE);
    
    m_FormerVecFromCenter = pointFromCenter;
}


void TrackCircle::drawDistanceNode()
{
    m_pLeftDot->clear();
    m_pLeftDot->drawDot(Vec2(Sw * 0.5f - m_NodeDistance * 0.5f, Sh * 0.4f),
                        3.0f,
                        Color4F(1.0f, 1.0f, 1.0f, 1.0f));
    
    m_pRightDot->clear();
    m_pRightDot->drawDot(Vec2(Sw * 0.5f + m_NodeDistance * 0.5f, Sh * 0.4f),
                         3.0f,
                         Color4F(1.0f, 1.0f, 1.0f, 1.0f));
    
    m_pLine->clear();
    m_pLine->drawLine(Vec2(Sw * 0.5f - m_NodeDistance * 0.5f, Sh * 0.4f),
                      Vec2(Sw * 0.5f + m_NodeDistance * 0.5f, Sh * 0.4f),
                      Color4F(1.0f, 1.0f, 1.0f, 1.0f));
}

