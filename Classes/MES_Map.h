//
//  MES_Map.h
//  MapEditorSample
//
//  Created by kondo on 2015/04/02.
//
//

#ifndef __MapEditorSample__MES_Map__
#define __MapEditorSample__MES_Map__

#include "cocos2d.h"
#include "GraphEdgeTypes.h"
#include "GraphNodeTypes.h"


class Wall;

class MES_Map
{
private:
    
    std::vector<Wall*> m_Walls;
    
    std::vector<NavGraphNode<> > m_Nodes;
    std::vector<NavGraphEdge> m_Edges;
    
    //マップデータを画面左下から離して表示するときのオフセット値
    float m_OffsetX;
    float m_OffsetY;
    
    void clear();
    
public:
    
    MES_Map()
    :m_OffsetX(0.0f)
    ,m_OffsetY(0.0f)
    {}
    
    ~MES_Map()
    {
        clear();
    }
    
    void loadWallFile(const std::string &wallFullPath,
                      float offsetX,
                      float offsetY);
    
    void writeMapFile(const std::string &mapFullPath);
    
    void addWall(std::ifstream &ifs);
    
    void clearGraph();
    
    const std::vector<Wall*>& getWalls()const{return m_Walls;}
    std::vector<NavGraphNode<> >& getNodes(){return m_Nodes;}
    std::vector<NavGraphEdge>& getEdges(){return m_Edges;}
    
    std::size_t getNumNode(){return m_Nodes.size();}
    
};

#endif /* defined(__MapEditorSample__MES_Map__) */
