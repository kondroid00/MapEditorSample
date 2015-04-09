//
//  MapGraphGenerator.cpp
//  MapEditorSample
//
//  Created by kondo on 2015/04/03.
//
//

#include "MapGraphGenerator.h"
#include "Wall.h"
#include "WallIntersection.h"
#include "Screen.h"
#include "Utils.h"
#include "MainLayer.h"

#include <fstream>
#include <math.h>
#include <deque>

using namespace cocos2d;


void MapGraphGenerator::clear()
{
    for(auto &cells : m_MapCells)
    {
        for(auto pMapCell : cells)
        {
            if(pMapCell->isAffective()) pMapCell->m_CellList.clear();
            delete pMapCell;
        }
    }
    m_MapCells.clear();
    
    m_CellQueue.clear();
    
    m_AffectiveCells.clear();
    
    m_pSeedCell = nullptr;
    
    if(m_pDrawDotObject)
    {
        m_pLayer->removeChild(m_pDrawDotObject);
        m_pDrawDotObject = nullptr;
    }
    
    m_State = State::idling;
}


void MapGraphGenerator::createGraph(const Vec2 &seed,
                                    float nodeDistance)
{
    //ノード間の距離を指定
    m_NodeDistance = nodeDistance;
    
    //セルの作成してシードのセルを返す
    auto pSeedCell = createMapCells(seed);
    
    //シードセルに探索済みチェックを入れる
    pSeedCell->check();
    
    //Wall上にあるセル全てに探索済みチェックを入れる（無効化）
    checkCellOnWalls();
    
    //シードフィルアルゴリズムでセルを有効化する
    calculate(pSeedCell);
    
    //有効なセルに番号を打つ
    setMapCellsNum();
    
    //ノードとエッジを作成
    createNodeAndEdge();
}


MapCell* MapGraphGenerator::createMapCells(const Vec2 &seed)
{
    //画面をm_NodeDistanceの間隔ごとに縦横を分割し、その交点にセルを設置する。シードのセルを戻り値にする。
    
    //シードの行と列をあらかじめ測っておく
    int seedX = 0;
    int seedY = 0;
    
    m_MapCellMapNumWidth = 0;
    m_MapCellMapNumHeight = 0;
    
    auto currentPosition = seed;
    
    //左下方向に移動し、そこから右方向および上方向に移動してセルの縦横の数を数える
    //左方向の探索
    while(currentPosition.x - m_NodeDistance > 0)
    {
        currentPosition -= Vec2(m_NodeDistance, 0.0f);
        ++seedX;
    }
    //下方向の探索
    while(currentPosition.y - m_NodeDistance > 0)
    {
        currentPosition -= Vec2(0.0f, m_NodeDistance);
        ++seedY;
    }
    //右方向への探索
    while(currentPosition.x + m_NodeDistance * m_MapCellMapNumWidth < Sw)
    {
        ++m_MapCellMapNumWidth;
    }
    //上方向への探索
    while(currentPosition.y + m_NodeDistance * m_MapCellMapNumHeight < Sh)
    {
        ++m_MapCellMapNumHeight;
    }
    
    //コンテナに全て格納する
    float posX = 0.0f;
    float posY = 0.0f;
    MapGraphGenerator::MapCellMap map(m_MapCellMapNumWidth, std::vector<MapCell*>(m_MapCellMapNumHeight, nullptr));
    for(int y = 0; y < m_MapCellMapNumHeight; ++y)
    {
        posY = m_NodeDistance * y + currentPosition.y;
        for(int x = 0; x < m_MapCellMapNumWidth; ++x)
        {
            posX = m_NodeDistance * x + currentPosition.x;
            map[x][y] = new MapCell(Vec2(posX, posY), x, y);
        }
    }
    m_MapCells = std::move(map);
    
    //シードのセルを戻す
    return m_MapCells[seedX][seedY];
}


void MapGraphGenerator::checkCellOnWalls()
{
    for(auto &cells : m_MapCells)
    {
        for(auto pMapCell : cells)
        {
            if(WallIntersection::pointLocatedOnWalls(pMapCell->position,
                                                     m_Walls))
            {
                pMapCell->check();
            }
        }
    }
}


void MapGraphGenerator::calculate(MapCell *pOriginCell)
{
    //次に探索するセルを入れておくためのコンテナ
    std::vector<MapCell*> container;

    //起点のセルを有効化
    pOriginCell->setAffective();
    
    //現在のセルのXYの値を入れる
    int currentX = pOriginCell->x;
    int currentY = pOriginCell->y;
    
    //壁からの距離
    float distance;
    //ダミー（関数の実装上）
    Vec2 dummy;
    
    int offsets[4][2] = {{ 0,  1},   //上
                         { 1,  0},   //右
                         { 0, -1},   //下
                         {-1,  0}};  //左
    
    //上下左右のセルを順番に調べる
    for(auto offset : offsets)
    {
        //オフセットで指定した方向にセルがあれば処理
        if(isExistMapCell(currentX + offset[0], currentY + offset[1]))
        {
            //オフセットの方向にあるセル（以下ネクストセル）
            auto pNextMapCell = m_MapCells[currentX + offset[0]][currentY + offset[1]];
            //ネクストセルが探索済みでなければ処理
            if(!pNextMapCell->isChecked())
            {
                //現在のセルとネクストセルの間に壁がなければ処理
                if(!WallIntersection::findClosestPointOfIntersectionWithWalls(pOriginCell->position,
                                                                              pNextMapCell->position,
                                                                              distance,
                                                                              dummy,
                                                                              m_Walls))
                {
                    //ネクストセルから一番近い壁を取得
                    WallIntersection::getClosestWall(pNextMapCell->position,
                                                                  distance,
                                                                  m_Walls);
                    //ネクストセルと壁の間の距離がある程度あればネクストセルを次に探索するセルのコンテナに入れる
                    if(distance > m_NodeDistance * 0.5f)
                    {
                        container.push_back(pNextMapCell);
                    }
                }
                //ネクストセルに探索済みのチェックを入れる
                pNextMapCell->check();
            }
        }
    }
    
    //コンテナの中のセルをシードにして探索する（再帰呼び出し）
    for(auto pMapCell : container)
    {
        calculate(pMapCell);
    }
}


bool MapGraphGenerator::isExistMapCell(int x, int y)const
{
    if(x >= 0 && x < m_MapCellMapNumWidth &&
       y >= 0 && y < m_MapCellMapNumHeight)
    {
        return true;
    }
    return false;
}


void MapGraphGenerator::setMapCellsNum()
{
    int i = 0;
    
    for(auto &cells : m_MapCells)
    {
        for(auto pMapCell : cells)
        {
            pMapCell->resetCheck();
            if(pMapCell->isAffective())
            {
                pMapCell->num = i;
                ++i;
            }
        }
    }
}


void MapGraphGenerator::createNodeAndEdge()
{
    int offsets[8][2] = {{ 0,  1},   //上
                         { 1,  1},   //右上
                         { 1,  0},   //右
                         { 1, -1},   //右下
                         { 0, -1},   //下
                         {-1, -1},   //左下
                         {-1,  0},   //左
                         {-1,  1}};  //左上
    
    for(std::size_t x = 0; x < m_MapCells.size(); ++x)
    {
        for(std::size_t y = 0; y < m_MapCells[x].size(); ++y)
        {
            //現在のセル
            auto pMapCell = m_MapCells[x][y];
            
            //有効なセルなら処理をする
            if(pMapCell->isAffective())
            {
                //セルの番号とポジションからノードを作成してコンテナに格納
                m_Nodes.push_back(NavGraphNode<>(pMapCell->num, pMapCell->position));
                
                //オフセット方向のノードを探索しエッジを作成
                for(auto offset : offsets)
                {
                    //オフセットの方向にセルがあれば処理を行う
                    if(isExistMapCell(x + offset[0], y + offset[1]))
                    {
                        //オフセット方向にあるセル（以下、ネクストセル）
                        auto pNextMapCell = m_MapCells[x + offset[0]][y + offset[1]];
                        
                        //現在のノードから壁までの距離
                        float distanceToWall;
                        
                        //ダミー（関数の実装上、実際には使わない）
                        Vec2 dummy;
                        
                        //ネクストセルが有効(affective == true)かつ、現在のセルとネクストセルの間に壁がなければ処理
                        if(pNextMapCell->isAffective() &&
                           !WallIntersection::findClosestPointOfIntersectionWithWalls(pMapCell->position,
                                                                                      pNextMapCell->position,
                                                                                      distanceToWall,
                                                                                      dummy, //関数の実装上
                                                                                      m_Walls))
                        {
                            //現在のセルから最も近い壁を返す
                            auto pClosestWallFromCell = WallIntersection::getClosestWall(pMapCell->position,
                                                                                         distanceToWall,
                                                                                         m_Walls);
                            //ネクストセルから最も近い壁を返す
                            auto pClosestWallFromNext = WallIntersection::getClosestWall(pNextMapCell->position,
                                                                                         distanceToWall,
                                                                                         m_Walls);
                            float closestDist = Utils::MAX_FLOAT;
                            
                            //以下は壁の角とエッジが近すぎる場合にはエッジを作成しないための処理
                            //上で求めた二つの壁の両端をコンテナに格納
                            std::vector<Vec2> points;
                            points.push_back(pClosestWallFromCell->getFrom());
                            points.push_back(pClosestWallFromCell->getTo());
                            points.push_back(pClosestWallFromNext->getFrom());
                            points.push_back(pClosestWallFromNext->getTo());
                            
                            //壁の端点とエッジの距離を求め、いちばん近い距離を求める
                            for(auto point : points)
                            {
                                distanceToWall = Geometry::distToLineSegment(pMapCell->position,
                                                                             pNextMapCell->position,
                                                                             point);
                                if(distanceToWall < closestDist)
                                {
                                    closestDist = distanceToWall;
                                }
                            }
                            
                            //いちばん近い距離がある程度あればエッジを作成してコンテナに格納
                            if(closestDist > m_NodeDistance * 0.4f)
                            {
                                m_Edges.push_back(NavGraphEdge(pMapCell->num,
                                                               pNextMapCell->num,
                                                               pMapCell->position.getDistance(pNextMapCell->position)));
                            }
                        }
                    }
                }
            }
        }
    }
}




//------------------------------------------------
//      アニメーション用
//------------------------------------------------

void MapGraphGenerator::update()
{
    switch(m_State)
    {
        case State::idling:
            return;
            
        case State::creating_node:
            createNodeCycleOnce();
            break;
            
        case State::creating_edge:
            createEdgeCycleOnce();
            break;
            
        default:
            break;
    }
}


void MapGraphGenerator::createNodeCycleOnce()
{
    //現在のセル
    MapCell *pCurrentCell;
    
    //キューが空、すなわち探索開始時の時はシードを現在のセルに格納し、
    //キューにセルがある場合は前から取り出したものを現在のセルに格納
    if(m_CellQueue.empty())
    {
        pCurrentCell = m_pSeedCell;
    }
    else
    {
        pCurrentCell = m_CellQueue.front();
        m_CellQueue.pop_front();
    }
    
    //起点のセルを有効化し、有効化されたセルをm_AffectiveCellsに格納する
    pCurrentCell->setAffective();
    m_AffectiveCells.push_back(pCurrentCell);
    
    //現在のセルのXYの値を入れる
    int currentX = pCurrentCell->x;
    int currentY = pCurrentCell->y;
    
    //壁からの距離
    float distance;
    //ダミー（関数の実装上）
    Vec2 dummy;
    
    int offsets[8][2] = {{ 1,  1},   //右上
                         { 1,  0},   //右
                         { 1, -1},   //右下
                         { 0, -1},   //下
                         {-1, -1},   //左下
                         {-1,  0},   //左
                         {-1,  1},   //左上
                         { 0,  1}};  //上
    
    //上下左右のセルを順番に調べる
    for(auto offset : offsets)
    {
        //オフセットで指定した方向にセルがあれば処理
        if(isExistMapCell(currentX + offset[0], currentY + offset[1]))
        {
            //オフセットの方向にあるセル（以下ネクストセル）
            auto pNextMapCell = m_MapCells[currentX + offset[0]][currentY + offset[1]];
            
            //ネクストセルが探索済みでなければ処理
            if(!pNextMapCell->isChecked())
            {
                //現在のセルとネクストセルの間に壁がなければ処理
                if(!WallIntersection::findClosestPointOfIntersectionWithWalls(pCurrentCell->position,
                                                                              pNextMapCell->position,
                                                                              distance,
                                                                              dummy,
                                                                              m_Walls))
                {
                    //ネクストセルから一番近い壁を取得
                    WallIntersection::getClosestWall(pNextMapCell->position,
                                                                  distance,
                                                                  m_Walls);
                    
                    //セルと壁の間が一定以上離れていれば処理
                    if(distance > m_NodeDistance * 0.5f)
                    {
                        //ネクストセルと壁の間の距離がある程度あればネクストセルを次に探索するセルのコンテナに入れる
                        m_CellQueue.push_back(pNextMapCell);
                        
                        //ネクストセルを現在のセルの隣接リストに登録する
                        pCurrentCell->m_CellList.push_back(pNextMapCell);
                        
                        //ノードを描画
                        drawNode(pNextMapCell);
                    }
                }
                //ネクストセルに探索済みのチェックを入れる
                pNextMapCell->check();
            }
            //チェック済みでも有効化されていればネクストセルを現在のセルの隣接リストに追加
            else if(pNextMapCell->isAffective())
            {
                pCurrentCell->m_CellList.push_back(pNextMapCell);
            }
        }
    }
    
    //キューが空なら探索終了
    if(m_CellQueue.empty())
    {
        //次の準備
        m_State = State::creating_edge;
        m_AffectiveCellsIterator = m_AffectiveCells.begin();
    }
}


void MapGraphGenerator::setSeed(const cocos2d::Vec2 &seed,
                                float nodeDistance)
{
    clear();
    
    m_State = State::creating_node;
    
    //ノード間の距離を指定
    m_NodeDistance = nodeDistance;
    
    //セルの作成してシードのセルを返す
    m_pSeedCell = createMapCells(seed);
    
    //シードセルに探索済みチェックを入れる
    m_pSeedCell->check();
    
    //Wall上にあるセル全てに探索済みチェックを入れる（無効化）
    checkCellOnWalls();
    
    drawSeed();
}


void MapGraphGenerator::createEdgeCycleOnce()
{
    //現在のセル
    auto pMapCell = *m_AffectiveCellsIterator;
    
    //現在のセルの隣接リストから一つずつ取り出して処理
    for(auto pNextMapCell : pMapCell->m_CellList)
    {
        //現在のノードから壁までの距離
        float distanceToWall;
        
        //ダミー（関数の実装上、実際には使わない）
        Vec2 dummy;
        
        //現在のセルとネクストセルの間に壁がなければ処理
        if(!WallIntersection::findClosestPointOfIntersectionWithWalls(pMapCell->position,
                                                                      pNextMapCell->position,
                                                                      distanceToWall,
                                                                      dummy, //関数の実装上
                                                                      m_Walls))
        {
            //現在のセルから最も近い壁を返す
            auto pClosestWallFromCell = WallIntersection::getClosestWall(pMapCell->position,
                                                                         distanceToWall,
                                                                         m_Walls);
            //ネクストセルから最も近い壁を返す
            auto pClosestWallFromNext = WallIntersection::getClosestWall(pNextMapCell->position,
                                                                         distanceToWall,
                                                                         m_Walls);
            float closestDist = Utils::MAX_FLOAT;
            
            //以下は壁の角とエッジが近すぎる場合にはエッジを作成しないための処理
            //上で求めた二つの壁の両端をコンテナに格納
            std::vector<Vec2> points;
            points.reserve(4);
            points.push_back(pClosestWallFromCell->getFrom());
            points.push_back(pClosestWallFromCell->getTo());
            points.push_back(pClosestWallFromNext->getFrom());
            points.push_back(pClosestWallFromNext->getTo());
            
            //壁の端点とエッジの距離を求め、いちばん近い距離を求める
            for(auto point : points)
            {
                distanceToWall = Geometry::distToLineSegment(pMapCell->position,
                                                             pNextMapCell->position,
                                                             point);
                if(distanceToWall < closestDist)
                {
                    closestDist = distanceToWall;
                }
            }
            
            //いちばん近い距離がある程度あればエッジを作成
            if(closestDist > m_NodeDistance * 0.5f)
            {
                drawEdge(pMapCell, pNextMapCell);
            }
        }
    }
    
    //有効化されたセルの探索が全て終われば終了
    if(++m_AffectiveCellsIterator == m_AffectiveCells.end())
    {
        //次の処理へ
        m_State = State::idling;
    }
}


void MapGraphGenerator::drawSeed()
{
    m_pDrawDotObject = DrawNode::create();
    m_pDrawDotObject->drawDot(m_pSeedCell->position,
                            3.0f,
                            Color4F(1.0f, 0.0f, 0.0f, 1.0f));
    m_pDrawDotObject->visit();
    
    m_pLayer->addChild(m_pDrawDotObject, Screen::Back_Fore);
    
}


void MapGraphGenerator::drawNode(MapCell *pCell)
{
    m_pLayer->getRenderTexture()->begin();
    
    auto pDrawDotObject = DrawNode::create();
    pDrawDotObject->drawDot(pCell->position,
                            3.0f,
                            Color4F(1.0f, 1.0f, 1.0f, 1.0f));
    pDrawDotObject->visit();
    
    m_pLayer->getRenderTexture()->end();
}


void MapGraphGenerator::drawEdge(MapCell *pCurrentCell, MapCell *pNextCell)
{
    m_pLayer->getRenderTexture()->begin();
    
    auto pDrawLineObject = DrawNode::create();
    pDrawLineObject->drawLine(pCurrentCell->position,
                              pNextCell->position,
                              Color4F(1.0f, 1.0f, 1.0f, 1.0f));
    pDrawLineObject->visit();
    
    m_pLayer->getRenderTexture()->end();
}

