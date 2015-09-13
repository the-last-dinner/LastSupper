//
//  DungeonMainMenuLayer.h
//  LastSupper
//
//  Created by Kohei on 2015/09/06.
//
//

#ifndef __LastSupper__DungeonMainMenuLayer__
#define __LastSupper__DungeonMainMenuLayer__

#include "Layers/baseMenuLayer.h"

class DungeonMainMenuLayer : public baseMenuLayer
{
	// 列挙型
public:
	enum struct Type
	{
		ITEM,
		SAVE,
		CHARA,
		TITLE,
		CLOSE,
		SIZE
	};
	
	// クラスメソッド
public:
	CREATE_FUNC(DungeonMainMenuLayer)
	
	// インスタンスメソッド
private:
	DungeonMainMenuLayer();
	~DungeonMainMenuLayer();
	virtual bool init();
	virtual void show();
	virtual void hide();
	virtual void onIndexChanged(int newIdx, bool sound = true);
	virtual void onSpacePressed(int idx);
	virtual void onMenuKeyPressed(){};
};

#endif /* defined(__LastSupper__DungeonMainMenuLayer__) */
