//
//  GraphNodeTypes.h
//  MapEditorSample
//
//  Author: Mat Buckland 2002 (fup@ai-junkie.com)
//
//

#ifndef __MapEditorSample__GraphNodeTypes__
#define __MapEditorSample__GraphNodeTypes__

#include <fstream>
#include <cmath>

#include "cocos2d.h"
#include "Screen.h"
#include "NodeTypeEnum.h"


class GraphNode
{
protected:
    
    int m_Index;
    
public:
    
    GraphNode():m_Index(NodeTypeEnum::invalid_node_index){}
    GraphNode(int index):m_Index(index){}
    GraphNode(std::ifstream &ifs)
    {
        char buffer[50];
        ifs >> buffer >> m_Index;
    }
    
    virtual ~GraphNode(){}
    
    int getIndex()const{return m_Index;}
    void setIndex(int index){m_Index = index;}
};


template<class T_extra_info = void*>
class NavGraphNode : public GraphNode
{
protected:
    
    cocos2d::Vec2 m_Position;
    T_extra_info m_ExtraInfo;
    
public:
    
    NavGraphNode()
    :m_ExtraInfo(T_extra_info())
    {}
    
    NavGraphNode(int index,
                 cocos2d::Vec2 position)
    :GraphNode(index)
    ,m_Position(position)
    ,m_ExtraInfo(T_extra_info())
    {}
    
    NavGraphNode(std::ifstream &ifs, float offsetX = 0.0f, float offsetY = 0.0f)
    :m_ExtraInfo(T_extra_info())
    {
        float x, y;
        char buffer[50];
        ifs >> buffer >> m_Index >> buffer >> x >> buffer >> y;
        m_Position.x = offsetX + x;
        m_Position.y = offsetY + y;
    }
    
    virtual ~NavGraphNode(){}
    
    cocos2d::Vec2 getPosition()const{return m_Position;}
    void setPosition(const cocos2d::Vec2 &newPosition){m_Position = newPosition;}
    
    T_extra_info getExtraInfo()const{return m_ExtraInfo;}
    void setExtraInfo(T_extra_info info){m_ExtraInfo = info;}
};

#endif /* defined(__MapEditorSample__GraphNodeTypes__) */
