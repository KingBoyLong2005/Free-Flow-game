#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "axmol.h"
#include "BoardGame.hpp"
#include "base/EventListenerTouch.h"


class MainScene : public ax::Scene
{
    
    
public:
    bool init() override;
    void loadFromFile(const std::string& filename); 
    MainScene();
    ~MainScene();

    bool onTouchBegan(ax::Touch* touch, ax::Event* event);
    void onTouchMoved(ax::Touch* touch, ax::Event* event); 
    void onTouchEnded(ax::Touch* touch, ax::Event* event);

private:
    BoardGame* boardGame;
    ax::EventListenerTouchOneByOne* touchListener;

};

#endif  // __MAIN_SCENE_H__
