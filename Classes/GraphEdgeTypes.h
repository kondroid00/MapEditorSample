//
//  GraphEdgeType.h
//  MapEditorSample
//
//  Author: Mat Buckland 2002 (fup@ai-junkie.com)
//
//

#ifndef MapEditorSample_GraphEdgeTypes_h
#define MapEditorSample_GraphEdgeTypes_h

#include <fstream>
#include "Screen.h"
#include "NodeTypeEnum.h"

class GraphEdge
{
protected:
    
    int m_From;
    int m_To;
    float m_Cost;
    
public:
    
    GraphEdge(int from, int to, float cost)
    :m_Cost(cost)
    ,m_From(from)
    ,m_To(to)
    {}

    GraphEdge(int from, int to)
    :m_Cost(1.0f)
    ,m_From(from)
    ,m_To(to)
    {}
    
    GraphEdge()
    :m_Cost(1.0f)
    ,m_From(NodeTypeEnum::invalid_node_index)
    ,m_To(NodeTypeEnum::invalid_node_index)
    {}
    
    GraphEdge(std::ifstream &ifs)
    {
        float cost;
        char buffer[50];
        ifs >> buffer >> m_From >> buffer >> m_To >> buffer >> cost;
        m_Cost = cost * GameScreenSize::width;
    }
    
    virtual ~GraphEdge(){}
    
    int getFrom()const{return m_From;}
    void setFrom(int from){m_From = from;}
    
    int getTo()const{return m_To;}
    void setTo(int to){m_To = to;}
    
    float getCost()const{return m_Cost;}
    void setCost(float cost){m_Cost = cost;}
    
    bool operator==(const GraphEdge &rhs)
    {
        return rhs.m_From == this->m_From &&
               rhs.m_To   == this->m_To   &&
               rhs.m_Cost == this->m_Cost;
    }
    
    bool operator!=(const GraphEdge &rhs)
    {
        return !(*this == rhs);
    }
};


class NavGraphEdge : public GraphEdge
{
public:
    
    enum
    {
        normal            = 0,
        swim              = 1 << 0,
        crawl             = 1 << 1,
        creep             = 1 << 3,
        jump              = 1 << 3,
        fly               = 1 << 4,
        grapple           = 1 << 5,
        goes_through_door = 1 << 6
    };
    
protected:
    
    int m_Flags;
    int m_IntersectingEntityID;
    
public:
    
    NavGraphEdge(int from,
                 int to,
                 float cost,
                 int flags = 0,
                 int id = -1)
    :GraphEdge(from, to, cost)
    ,m_Flags(flags)
    ,m_IntersectingEntityID(id)
    {}
    
    NavGraphEdge(std::ifstream &ifs)
    {
        char buffer[50];
        ifs >> buffer >> m_From >> buffer >> m_To >> buffer >> m_Cost;
        ifs >> buffer >> m_Flags >> buffer >> m_IntersectingEntityID;
    }
    
    int getFlags()const{return m_Flags;}
    void setFlags(int flags){m_Flags = flags;}
    
    int getIntersectingEntityID()const{return m_IntersectingEntityID;}
    void setIntersectingEntityID(int id){m_IntersectingEntityID = id;}
};

#endif
