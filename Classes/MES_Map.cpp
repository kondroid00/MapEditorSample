//
//  MES_Map.cpp
//  MapEditorSample
//
//  Created by kondo on 2015/04/02.
//
//

#include <vector>

#include "MES_Map.h"
#include "Wall.h"
#include "ObjectEnumerations.h"


void MES_Map::clear()
{
    for(auto pWall : m_Walls)
    {
        delete pWall;
    }
    m_Walls.clear();
}


void MES_Map::clearGraph()
{
    m_Nodes.clear();
    m_Edges.clear();
}


void MES_Map::loadWallFile(const std::string &wallFullPath,
                             float offsetX,
                             float offsetY)
{
    std::ifstream ifs(wallFullPath.c_str());
    assert(ifs && "<MES_MapEditor::loadWallFile>: File doesn't exist.");
    
    m_OffsetX = offsetX;
    m_OffsetY = offsetY;
    
    while(!ifs.eof())
    {
        int objectType;
        
        ifs >> objectType;
        
        switch(objectType)
        {
            case ObjectType::type_wall:
                addWall(ifs);
                break;
                
            default:
                break;
        }
    }
}


void MES_Map::addWall(std::ifstream &ifs)
{
    m_Walls.push_back(new Wall(ifs, m_OffsetX, m_OffsetY));
}


void MES_Map::writeMapFile(const std::string &mapFullPath)
{
    std::ofstream ofs(mapFullPath.c_str());
    
    assert(ofs && "<MapEditor::createFile>: File can't open.");
    
    ofs << m_OffsetX << " " << m_OffsetY <<std::endl;
    
    ofs << m_Nodes.size() << std::endl;
    for(auto node : m_Nodes)
    {
        auto posX = node.getPosition().x - m_OffsetX;
        auto posY = node.getPosition().y - m_OffsetY;
        ofs << "Index: " << node.getIndex() << " PosX: " << posX << " PosY: " << posY << std::endl;
    }
    
    ofs << m_Edges.size() << std::endl;
    for(auto edge : m_Edges)
    {
        ofs << "From: " << edge.getFrom() << " To: " << edge.getTo() << " Cost: " << edge.getCost() << " Flags: " << edge.getFlags() << " ID: " << edge.getIntersectingEntityID() << std::endl;
    }
    
    ofs.close();
}

