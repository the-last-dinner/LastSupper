//
//  TiledMapManager.h
//  LastSupper
//
//  Created by Kohei on 2015/06/13.
//
//

#ifndef __TILED_MAP_MANAGER_H__
#define __TILED_MAP_MANAGER_H__

#include "define.h"

class TiledMapManager
{
	// クラスメソッド
public:
	static TiledMapManager* getInstance();
	static void destory();
	
	// シングルトンであるためにprivateに
private:
	TiledMapManager();												// コンストラクタ
	TiledMapManager(const TiledMapManager& other);					// デストラクタ
	TiledMapManager& operator = (const TiledMapManager& other);		// 代入演算子
	// インスタンスメソッド
public:
	~TiledMapManager();
	void setBasePath(string basePath);
	void setTiledMapWithFileName(string fileName);
	experimental::TMXTiledMap* getTiledMap();
	void removeTiledMap();
	
	// インスタンス変数
private:
	string basePath;
	experimental::TMXTiledMap* tiledMap;
};

#endif // __TILED_MAP_MANAGER_H__
