//
//  EventTask.cpp
//  LastSupper
//
//  Created by Kohei Asami on 2015/09/05.
//
//

#include "Tasks/EventTask.h"

#include "Event/GameEvent.h"
#include "Event/EventFactory.h"
#include "Event/EventScriptValidator.h"

#include "Event/EventScript.h"
#include "Managers/DungeonSceneManager.h"

// コンストラクタ
EventTask::EventTask(){FUNCLOG}

// デストラクタ
EventTask::~EventTask()
{
    FUNCLOG
    
    Director::getInstance()->getScheduler()->unscheduleUpdate(this);
    
    // イベントキューが空でなかったら全て削除
    if(!this->eventQueue.empty())
    {
        for(int i {0}; i < this->eventQueue.size(); i++)
        {
            CC_SAFE_RELEASE(this->getGameEvent(this->eventQueue.front()));
            this->eventQueue.pop_front();
        }
    }
}

// 初期化
bool EventTask::init()
{
    if(!GameTask::init()) return false;

	return true;
}

// イベントキューにあるイベントを実行開始
void EventTask::runEventQueue()
{
    if(!this->existsEvent()) return;
    this->run();
    // update開始
    Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
}

// イベントをIDから実行
void EventTask::runEvent(int eventId)
{
    this->pushEventBack(eventId);
    
    // 実行
    this->runEventQueue();
}

// イベントをIDベクタから実行、ベクタを全て実行するとコールバック呼び出し
void EventTask::runEvent(vector<int> eventIds, function<void()> callback)
{
    // ベクタが空ならコールバック
    if(eventIds.empty())
    {
        if(callback) callback();
        
        return;
    }
    
    int pushedNum { 0 };
    int lastEventId { 0 };
    
    for(int eventId : eventIds)
    {
        if(this->pushEventBack(eventId))
        {
            pushedNum++;
            lastEventId = eventId;
        }
    }
    
    // 一つも生成されていなければその場でコールバック
    if(pushedNum == 0)
    {
        if(callback) callback();
        
        return;
    }
    
    // キューにあるイベントを先頭から実行
    this->runEventQueue();
    
    this->callbackInfo = CallbackWithId({lastEventId, callback});
}

// イベントをIDベクタから実行
void EventTask::runEvent(vector<int> eventIds)
{
    if(eventIds.empty()) return;
    
    for(int eventId : eventIds)
    {
        this->runEvent(eventId);
    }
}

// キューに指定IDイベントを後ろから詰める
bool EventTask::pushEventBack(int eventId)
{
    GameEvent* event {this->createEventById(eventId)};
    if(!event) return false;
    this->eventQueue.push_back({eventId, event});
    
    return true;
}

// キューに指定IDイベントを前から詰める
bool EventTask::pushEventFront(int eventId)
{
    GameEvent* event {this->createEventById(eventId)};
    if(!event) return false;
    this->eventQueue.push_front({eventId, event});
    
    return true;
}

// キューにある先頭のイベントを実行
void EventTask::run()
{
    // 実行中のイベントがあればリターン
    if(this->isEventRunning()) return;
    
    // イベントキューが空ならリターン
    if(this->eventQueue.empty()) return;
    
    // なければ先頭を実行
    this->runningEvent = this->eventQueue.front();
    this->eventQueue.pop_front();
    this->getGameEvent(this->runningEvent)->run();
}

// IDからイベントを生成
GameEvent* EventTask::createEventById(int eventId)
{
    if(eventId == static_cast<int>(EventID::UNDIFINED) || PlayerDataManager::getInstance()->getEventFlag(PlayerDataManager::getInstance()->getLocation().map_id, eventId)) return nullptr;
    
    DungeonSceneManager* manager {DungeonSceneManager::getInstance()};
    
    GameEvent* event { manager->getEventFactory()->createGameEvent(manager->getEventScript()->getScriptJson(eventId))};
    
    CC_SAFE_RETAIN(event);
    
    // 生成されたらキューに詰められるので、この場で操作不可状態にしておく
    DungeonSceneManager::getInstance()->setEventListenerPaused(true);
    
    return event;
}

// イベントIDを取得
int EventTask::getEventId(const EventWithId& eventWithId) const
{
    return eventWithId.first;
}

// イベントポインタを取得
GameEvent* EventTask::getGameEvent(const EventWithId& eventWithId) const
{
    return eventWithId.second;
}

// 現在実行中のイベントがあるか
bool EventTask::isEventRunning()
{
    return  this->getGameEvent(this->runningEvent);
}

// キューにイベントが存在するか
bool EventTask::existsEvent()
{
    return !this->eventQueue.empty();
}

// update
void EventTask::update(float delta)
{
    // 実行中イベントを更新
    this->getGameEvent(this->runningEvent)->update(delta);
    
    // 実行中イベントが終了していたら解放
    if(this->getGameEvent(this->runningEvent)->isDone())
    {
        if(this->callbackInfo.second && this->callbackInfo.first == this->getEventId(this->runningEvent))
        {
            this->callbackInfo.second();
            this->callbackInfo = CallbackWithId({static_cast<int>(EventID::UNDIFINED), nullptr});
        }
        CC_SAFE_RELEASE(this->getGameEvent(this->runningEvent));
        this->runningEvent = EventWithId({static_cast<int>(EventID::UNDIFINED), nullptr});
    }
        
    // キューが空になったらupdate停止
    if(this->eventQueue.empty() && !this->getGameEvent(this->runningEvent))
    {
        Director::getInstance()->getScheduler()->unscheduleUpdate(this);
 
        // 操作可能状態に
        DungeonSceneManager::getInstance()->setEventListenerPaused(false);
    }
    
    // イベントを実行
    this->run();
}

// 実行中のイベントIDを取得
int EventTask::getRunningEventId() const
{
    return this->getEventId(this->runningEvent);
}

// キューにあるイベントを全て取得
deque<EventTask::EventWithId> EventTask::getEvents() const
{
    return this->eventQueue;
}