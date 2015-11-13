//
//  PathFinder.cpp
//  LastSupper
//
//  Created by Kohei Asami on 2015/11/07.
//
//

#include "PathFinder.h"

// コンストラクタ
PathFinder::PathFinder() { FUNCLOG };

// デストラクタ
PathFinder::~PathFinder() { FUNCLOG };

// 初期化
bool PathFinder::init(const Size& mapSize)
{
    this->mapSize = mapSize;
    
    // マップのマス数
    this->gridWidth = static_cast<int>(MapUtils::getGridNum(mapSize.width));
    this->gridHeight = static_cast<int>(MapUtils::getGridNum(mapSize.height));
    
    return true;
}

// A-starのアルゴリズムで経路を探索
stack<Direction> PathFinder::getPath(const Rect& chaserGridRect, const vector<Rect>& collisionGridRects, const Point& destGridPosition)
{
    map<Point, PathNode*> nodeMap {};
    
    // 追跡者の幅を考慮して進入不可座標をセットする
    if(!collisionGridRects.empty())
    {
        for(Rect collisionGridRect : collisionGridRects)
        {
            for(Point gridPosition : this->splitByGrid(collisionGridRect))
            {
                int chaserGridWidth {static_cast<int>(chaserGridRect.size.width)};
                
                // 追跡者のマス幅 - 1だけ左側のマスをCANT状態にする
                for(int i {0}; i < chaserGridWidth - 1 ; i++)
                {
                    Point position {Point(gridPosition.x - i, gridPosition.y)};
                    
                    // もうすでにマス座標に登録されているノードがあれば無視
                    if(nodeMap.count(position) != 0) continue;
                    
                    PathNode* node { this->createNode(position) };
                    node->setState(PathNode::State::CANT);
                    
                    nodeMap.insert({position, PathNode::create(position)});
                }
            }
        }
    }
    
    // 始点ノードを生成、探索開始
    PathNode* startNode { this->createNode(chaserGridRect.origin, destGridPosition) };
    
    PathNode* destNode { this->find(startNode, destGridPosition, nodeMap)};
    
    stack<Direction> directions;
    
    if(!destNode) return directions;
    
    // どの方向に進むのかを、目的地ノードから探索開始ノードまで取り出す
    PathNode* node {destNode};
    while (node->getParent())
    {
        // 親との差ベクトルを方向に変換
        Direction direction {MapUtils::vecToMapDirection(node->getParent()->getGridPoint() - node->getGridPoint())};
        if(direction != Direction::SIZE) directions.push(direction);
    }
    
    return directions;
}

// 基準ノードを中心に周りをOPEN
PathFinder::PathNode* PathFinder::find(PathFinder::PathNode* referenceNode, const Point& destGridPosition, map<Point, PathFinder::PathNode*> nodeMap)
{
    // 基準ノード座標 = 目的地座標ならばリターン
    if(referenceNode->getGridPoint() == destGridPosition) return referenceNode;
    
    // 基準ノードをCLOSE
    referenceNode->setState(PathNode::State::CLOSED);
    
    // 方向毎にPointを用意
    Point referencePosition { referenceNode->getGridPoint() };
    vector<Point> positions
    {
        Point(referencePosition.x, referencePosition.y - 1), // 上
        Point(referencePosition.x, referencePosition.y + 1), // 下
        Point(referencePosition.x + 1, referencePosition.y), // 右
        Point(referencePosition.x - 1, referencePosition.y), // 左
    };
    
    // 周りの四方向についてノードを生成
    for(Point position : positions)
    {
        // あたり判定がないか確認、あれば無視
        if(nodeMap.count(position) != 0 && nodeMap.at(position)->getState() == PathNode::State::CANT) continue;
        
        PathNode* node {this->createNode(position, destGridPosition, referenceNode)};
        
        // マス座標にノードがないとき
        if(nodeMap.count(position) == 0)
        {
            // OPEN状態で登録
            nodeMap.insert({position, node});
            
            continue;
        }

        // マス座標にOPEN・CLOSED状態のノードが存在する時
        if(nodeMap.at(position)->getState() == PathNode::State::OPEN || nodeMap.at(position)->getState() == PathNode::State::CLOSED)
        {
            // 生成したノードのスコア < 登録されているノードのスコア の場合入れ替える
            if(node->getScore() < nodeMap.at(position)->getScore())
            {
                CC_SAFE_RELEASE(nodeMap.at(position));
                nodeMap.erase(position);
                nodeMap.insert({position, node});
                
                continue;
            }
            
            // 入れ替えが発生しなかったら、生成したノードをリリース
            CC_SAFE_RELEASE(node);
        }
    }
    
    // スコア最小ノード
    PathNode* minScoreNode { nullptr };

    // OPEN状態のノードの中から最小スコアのノードを探す
    for(pair<Point, PathNode*> nodeWithPos : nodeMap)
    {
        // OPEN状態以外のノードは無視
        if(nodeWithPos.second->getState() != PathNode::State::OPEN) continue;
        
        if (!minScoreNode) minScoreNode = nodeWithPos.second;
        
        // スコアで比較
        if(nodeWithPos.second->getScore() < minScoreNode->getScore())
        {
            minScoreNode = nodeWithPos.second;
            
            continue;
        }
        
        // スコアが同じならば、実コストで比較
        if(nodeWithPos.second->getScore() == minScoreNode->getScore() && nodeWithPos.second->getActualCost() < minScoreNode->getActualCost())
        {
            minScoreNode = nodeWithPos.second;
        }
    }
    
    // OPEN状態のノードが一個もなければ(minScoreNodeがnullptrのとき)、探索失敗として終了(nullptrを返す)
    if(!minScoreNode) return nullptr;
    
    // 最小ノードを新たな基準とし探索
    return this->find(minScoreNode, destGridPosition, nodeMap);
}

// マスRectをマス座標に分割
vector<Point> PathFinder::splitByGrid(const Rect& gridRect)
{
    vector<Point> points {};
    
    int gridWidth {static_cast<int>(gridRect.size.width)};
    int gridHeight {static_cast<int>(gridRect.size.height)};
    
    for(int w {0}; w < gridWidth; w++)
    {
        for(int h {0}; h < gridHeight; h++)
        {
            points.push_back(Point(gridRect.origin.x + w, gridRect.origin.y - h));
        }
    }
    
    return points;
}

// ノードを生成
PathFinder::PathNode* PathFinder::createNode(const Point& gridPosition)
{
    PathNode* node { PathNode::create(gridPosition) };
    CC_SAFE_RETAIN(node);
    
    return node;
}

// ノードを生成、実、推定コストも一緒にセット
PathFinder::PathNode* PathFinder::createNode(const Point& gridPosition, const Point& destGridPosition, PathNode* parent)
{
    PathNode* node { PathNode::create(gridPosition) };
    CC_SAFE_RETAIN(node);
    
    // OPEN状態にする
    node->setState(PathNode::State::OPEN);
    
    // 推定コスト(目的地x, y座標との差の和)
    node->setEstimatedCost(abs(destGridPosition.x - gridPosition.x) + abs(destGridPosition.y - gridPosition.y));
    
    if(!parent) return node;
    
    // 親ノードをセット
    node->setParent(parent);
    
    // 実コスト(親ノードの実コスト＋１)
    node->setActualCost(parent->getActualCost() + 1);
    
    return node;
}