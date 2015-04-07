//
//  MapGraphGenerator.h
//  MapEditorSample
//
//  Created by kondo on 2015/04/03.
//
//

#ifndef __MapEditorSample__MapGraphGenerator__
#define __MapEditorSample__MapGraphGenerator__

#include "cocos2d.h"
#include "ObjectEnumerations.h"
#include "GraphNodeTypes.h"
#include "GraphEdgeTypes.h"

#include <vector>

class Wall;
class MainLayer;


struct MapCell
{
    int num;
    cocos2d::Vec2 position;
    int x;
    int y;
    
    //探索済みならtrue
    bool checked;

    //有効なセルならtrue
    bool affective;
    
    //隣接する有効なセル
    std::vector<MapCell*> m_CellList;
    
public:
    
    MapCell(){}
    
    MapCell(const cocos2d::Vec2 &position,
         int x,
         int y)
    :num(0)
    ,position(position)
    ,x(x)
    ,y(y)
    ,checked(false)
    ,affective(false)
    {
        m_CellList.reserve(8);
    }
    
    void check(){checked = true;}
    void resetCheck(){checked = false;}
    bool isChecked()const{return checked;}
    
    void setAffective(){affective = true;}
    void resetAffective(){affective = false;}
    bool isAffective()const{return affective;}
};


class MapGraphGenerator
{
private:
    
    using MapCellMap = std::vector<std::vector<MapCell*> >;
    
private:
    
    
    const std::vector<Wall*> &m_Walls;
    
    float m_NodeDistance;
    
    MapCellMap m_MapCells;
    int m_MapCellMapNumWidth;
    int m_MapCellMapNumHeight;
    
    std::vector<NavGraphNode<> > &m_Nodes;
    std::vector<NavGraphEdge> &m_Edges;
    
    MapCell* createMapCells(const cocos2d::Vec2 &seed);
    
    void checkCellOnWalls();
    
    void calculate(MapCell *pSeed);
    
    void createNodeAndEdge();
    
    void setMapCellsNum();
    
    bool isExistMapCell(int x, int y)const;
    
public:
    
    MapGraphGenerator(const std::vector<Wall*> &walls,
              std::vector<NavGraphNode<> > &nodes,
              std::vector<NavGraphEdge> &edges)
    :m_Walls(walls)
    ,m_Nodes(nodes)
    ,m_Edges(edges)
    {}
    
    ~MapGraphGenerator(){clear();}
    
    void createGraph(const cocos2d::Vec2 &seed,
                     float nodeDistance);
    
    void clear();
    
    
//------------------------------------------------
//      アニメーション付き
//------------------------------------------------
    
private:
    
    enum class State
    {
        idling,
        creating_node,
        creating_edge
    };
    
public:
    
    inline std::string showState()
    {
        switch (m_State)
        {
            case State::idling:
                return "MapGraphGenerator : State idling";
                break;
                
            case State::creating_node:
                return "MapGraphGenerator : State creating_node";
                break;
                
            case State::creating_edge:
                return "MapGraphGenerator : State creating_edge";
                break;
                
            default:
                break;
        }
    }
    
private:
    
    State m_State;
    
    std::deque<MapCell*> m_CellQueue;
    
    MapCell *m_pSeedCell;
    
    //有効化されたセルを格納
    std::vector<MapCell*> m_AffectiveCells;
    //上のコンテナの現在の位置を指す
    std::vector<MapCell*>::iterator m_AffectiveCellsIterator;
    
    MainLayer *m_pLayer;
    
    cocos2d::DrawNode *m_pDrawDotObject;
    
    void drawSeed();
    
    void drawNode(MapCell *pCell);
    void drawEdge(MapCell *pCurrentCell,
                  MapCell *pNextCell);
    
    void createNodeCycleOnce();
    void createEdgeCycleOnce();
    
public:
    
    MapGraphGenerator(const std::vector<Wall*> &walls,
                      std::vector<NavGraphNode<> > &nodes,
                      std::vector<NavGraphEdge> &edges,
                      MainLayer *layer)
    :m_Walls(walls)
    ,m_Nodes(nodes)
    ,m_Edges(edges)
    ,m_pLayer(layer)
    ,m_pSeedCell(nullptr)
    ,m_pDrawDotObject(nullptr)
    ,m_State(State::idling)
    {}
    
    void update();
    
    void setSeed(const cocos2d::Vec2 &seed,
                 float nodeDistance);
    
    bool isIdling()const{return m_State == State::idling;}
    bool isCreatingNode()const{return m_State == State::creating_node;}
    bool isCreatingEdge()const{return m_State == State::creating_edge;}
    
};

#endif /* defined(__MapEditorSample__MapGraphGenerator__) */
