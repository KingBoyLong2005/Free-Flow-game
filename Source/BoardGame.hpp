#ifndef _BOARD_GAME_HPP_
#define _BOARD_GAME_HPP_

#include "axmol.h"
#include <vector>
#include <string>
#include <unordered_map>

class BoardGame {
private:
    static const int GRID_SIZE = 100;  // Kích thước ô vuông
    static const int DOT_RADIUS = 20;  // Bán kính của điểm tròn
    std::vector<std::vector<char>> currentLevel;  // Mảng lưu dữ liệu từ file .txt

public:
    BoardGame();
    ~BoardGame();

    ax::DrawNode* gridNode;
    ax::DrawNode* dotsNode;
    ax::DrawNode* pathsNode;

    struct Point {
        int x, y;
        std::string color;

        Point(int x, int y, const std::string& color) : x(x), y(y), color(color) {}
        bool operator==(const Point& other) const {
            return x == other.x && y == other.y;
        }
    };

    struct Path {
        std::vector<Point> points;
        std::string color;
    };

    std::unordered_map<std::string, Path> paths;
    Point* currentStart = nullptr;
    Point* lastGridPos = nullptr;
    std::string currentColor;

    void createGridAndDots(const std::vector<std::vector<char>>& levelData);  // Phương thức kết hợp tạo grid và vẽ điểm
    ax::Color4F getColorFromString(const std::string& colorStr);

    void drawPaths();
    bool isValidMove(const Point& from, const Point& to) const;
    bool canAddToPath(const Point& point) const;

    Point getGridPosition(const ax::Vec2& pos);

    ax::DrawNode* getGridNode() { return gridNode; }  // Trả về node của grid
    ax::DrawNode* getDotsNode() { return dotsNode; }  // Trả về node của dots
    ax::DrawNode* getPathsNode() { return pathsNode; }

    void handleTouchBegan(const ax::Vec2& touchPos);
    void handleTouchMoved(const ax::Vec2& touchPos);
    void handleTouchEnded();
};

#endif //_BOARD_GAME_HPP_
