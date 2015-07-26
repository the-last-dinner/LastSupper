//
//  baseMessageLayer.cpp
//  LastSupper
//
//  Created by Kohei on 2015/07/12.
//
//

#include "baseMessageLayer.h"

const map<baseMessageLayer::MessageType, float> baseMessageLayer::span =
{
	{baseMessageLayer::MessageType::NORMAL, 0.05f},
	{baseMessageLayer::MessageType::SLOW, 1.f},
};

// コンストラクタ
baseMessageLayer::baseMessageLayer():
eventListener(nullptr),
str(""),
base(nullptr),
message(nullptr),
letterActions{}
{
	FUNCLOG
	// イベントリスナ生成
	this->eventListener = EventListenerKeyboard::create();
	this->eventListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event){if(keyCode == EventKeyboard::KeyCode::KEY_SPACE) this->onSpacePressed();};
	
	// イベントリスナ登録
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(this->eventListener, this);
}

// デストラクタ
baseMessageLayer::~baseMessageLayer()
{FUNCLOG}

// spaceキーを押した時の処理
void baseMessageLayer::onSpacePressed()
{
	FUNCLOG
	bool _isVisible {false};
	for(int i = 0; i < this->message->getStringLength(); i++)
	{Sprite* letter = this->message->getLetter(i); if(letter && letter->isVisible()) _isVisible = true;}
	
	if(!_isVisible)
	{
		for(Action* letterAction : this->letterActions)
		{this->stopAction(letterAction);}
		for(int i = 0; i < this->message->getStringLength(); i++)
		{Sprite* letter = this->message->getLetter(i); if(letter) letter->setVisible(true);}
	}
	return;
}

// メッセージを表示
void baseMessageLayer::displayMessage(const string& str, int fontSize, MessageType type)
{
	FUNCLOG
	this->message = Label::createWithTTF(str, "fonts/cinecaption2.28.ttf", fontSize);
	this->message->setPosition(this->base->getContentSize() / 2);
	this->base->addChild(this->message);
	
	// アニメーション設定
	for(int i = 0; i < this->message->getStringLength() ; i++)
	{
		Sprite* letter = this->message->getLetter(i);
		// エスケープ文字をスルー
		if(letter)
		{
			letter->setVisible(false);
			Sequence* letterAction = Sequence::createWithTwoActions(TargetedAction::create(letter, DelayTime::create(this->span.at(type) * i)), TargetedAction::create(letter, Show::create()));
			this->letterActions.push_back(letterAction);
			this->runAction(letterAction);
		}
	}
	return;
}