#include "MainScene.h"
#include "BoardGame.hpp"
#include <iostream>
#include <fstream>

using namespace ax;

MainScene::MainScene() {
    boardGame = new BoardGame();  // Khởi tạo đối tượng BoardGame
    this->addChild(boardGame->getGridNode());  // Thêm gridNode vào scene
    this->addChild(boardGame->getDotsNode());  // Thêm dotsNode vào scene
    this->addChild(boardGame->getPathsNode());
}

MainScene::~MainScene() {
    if (boardGame) {
        delete boardGame;  // Hủy đối tượng BoardGame khi scene bị xóa
        boardGame = nullptr;  // Đảm bảo trỏ về nullptr sau khi xóa
    }
}

// on "init" you need to initialize your instance
bool MainScene::init() {
    //////////////////////////////
    // 1. super init first
    if (!Scene::init()) {
        return false;
    }

    auto visibleSize = _director->getVisibleSize();
    auto origin = _director->getVisibleOrigin();
    auto safeArea = _director->getSafeAreaRect();
    auto safeOrigin = safeArea.origin;

    // Thêm BoardGame vào scene
    this->addChild(boardGame->getGridNode());  // Thêm gridNode vào scene
    this->addChild(boardGame->getDotsNode());  // Thêm dotsNode vào scene
    this->addChild(boardGame->getPathsNode());

    // Có thể gọi thêm các phương thức khởi tạo khác như load level từ file nếu cần
    loadFromFile("grid.txt");

    // Khởi tạo listener
    touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = AX_CALLBACK_2(MainScene::onTouchBegan, this);
    touchListener->onTouchMoved = AX_CALLBACK_2(MainScene::onTouchMoved, this); 
    touchListener->onTouchEnded = AX_CALLBACK_2(MainScene::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    return true;
}

void MainScene::loadFromFile(const std::string& filename) {
    std::string filePath = ax::FileUtils::getInstance()->getWritablePath() + filename;
    printf("%s\n", filePath.c_str());

    std::ifstream file(filePath);
    std::vector<std::vector<char>> levelData;
    if (!file.is_open()) {
        printf("Không thể mở file %s", filePath.c_str());
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<char> row;
        for (char c : line) {
            if (c != ' ') {  // Bỏ qua kí tự dấu cách
                row.push_back(c);
            }
        }
        levelData.push_back(row);
    }

    // Cập nhật levelData vào boardGame và vẽ lại
    boardGame->createGridAndDots(levelData);  // Tạo grid và dots dựa trên levelData
    file.close();
}

// Cập nhật các phương thức xử lý sự kiện cảm ứng
bool MainScene::onTouchBegan(ax::Touch* touch, ax::Event* event) {
    boardGame->handleTouchBegan(touch->getLocation());
    return true;  // Trả về true để chỉ ra sự kiện đã được xử lý
}

void MainScene::onTouchMoved(ax::Touch* touch, ax::Event* event) {
    boardGame->handleTouchMoved(touch->getLocation());
}

void MainScene::onTouchEnded(ax::Touch* touch, ax::Event* event) {
    boardGame->handleTouchEnded();
}
