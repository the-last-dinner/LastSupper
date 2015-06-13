//
//  LoadScene.h
//  LastSupper
//
//  Created by Kohei on 2015/06/05.
//
//

#ifndef __LOAD_SCENE_H__
#define __LOAD_SCENE_H__

#include "Common.h"
#include "baseScene.h"
#include "Scenes.h"

class LoadScene : public baseScene
{
	// クラスメソッド
public:
	static Scene* createScene(const SceneType& sceneType);
	CREATE_FUNC_WITH_SCENE_TYPE(LoadScene);
	
	// インスタンスメソッド
private:
	virtual bool init(const SceneType& sceneType);
	void loadFinished();	// ロードが終了した時の処理
public:
	LoadScene();	// コンストラクタ
	~LoadScene();	// デストラクタ
	static const vector<Scene*> scenes;
private:
	mutex mtx;
};

#endif // __LOAD_SCENE_H__