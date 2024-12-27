#include "BoardGame.hpp"
#include "axmol.h"
#include <fstream>
#include <sstream>

USING_NS_AX;

BoardGame::BoardGame()
    : gridNode(ax::DrawNode::create()), dotsNode(ax::DrawNode::create()), pathsNode(ax::DrawNode::create())
{
    // Thêm các đối tượng grid và dot vào scene
}

BoardGame::~BoardGame() {}

void BoardGame::createGridAndDots(const std::vector<std::vector<char>>& levelData) {
    currentLevel = levelData;
    int levelWidth = currentLevel[0].size();
    int levelHeight = currentLevel.size();

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    
    float startX = origin.x + (visibleSize.width - levelWidth * GRID_SIZE) / 2;
    float startY = origin.y + (visibleSize.height - levelHeight * GRID_SIZE) / 2;

    // Grid lines remain the same
    for (int i = 0; i <= levelWidth; i++) {
        float x = startX + i * GRID_SIZE;
        gridNode->drawLine(
            Vec2(x, startY),
            Vec2(x, startY + levelHeight * GRID_SIZE),
            Color4F(0.5f, 0.5f, 0.5f, 1.0f)
        );
    }

    for (int i = 0; i <= levelHeight; i++) {
        float y = startY + i * GRID_SIZE;
        gridNode->drawLine(
            Vec2(startX, y),
            Vec2(startX + levelWidth * GRID_SIZE, y),
            Color4F(0.5f, 0.5f, 0.5f, 1.0f)
        );
    }

    // Modified dot drawing
    for (int y = 0; y < levelHeight; y++) {
        for (int x = 0; x < levelWidth; x++) {
            char cell = currentLevel[y][x];
            if (cell != '0') {
                float centerX = startX + x * GRID_SIZE + GRID_SIZE / 2;
                float centerY = startY + (levelHeight - 1 - y) * GRID_SIZE + GRID_SIZE / 2;  // Removed the inversion

                dotsNode->drawDot(
                    Vec2(centerX, centerY),
                    DOT_RADIUS,
                    getColorFromString(std::string(1, cell))
                );
            }
        }
    }
}

ax::Color4F BoardGame::getColorFromString(const std::string& colorStr) {
    if (colorStr == "A") {
        return Color4F::RED;
    } else if (colorStr == "B") {
        return Color4F::GREEN;
    } else if (colorStr == "C") {
        return Color4F::BLUE;
    }
    return Color4F::WHITE;  // Mặc định là màu trắng
}

BoardGame::Point BoardGame::getGridPosition(const ax::Vec2& pos) {
    int levelWidth = currentLevel[0].size();  // Lấy số cột từ currentLevel
    int levelHeight = currentLevel.size();    // Lấy số hàng từ currentLevel

    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    
    float startX = origin.x + (visibleSize.width - levelWidth * GRID_SIZE) / 2;
    float startY = origin.y + (visibleSize.height - levelHeight * GRID_SIZE) / 2;
    
    int x = (pos.x - startX) / GRID_SIZE;
    int y = (pos.y - startY) / GRID_SIZE;
    
    y = levelHeight - 1 - y;  // Điều chỉnh lại y để phù hợp với lưới

    // Kiểm tra xem tọa độ có hợp lệ không
    if (x >= 0 && x < levelWidth && y >= 0 && y < levelHeight) {
        return Point(x, y, std::string(1, currentLevel[y][x]));
    }
    return Point(-1, -1, "");
}

void BoardGame::drawPaths() {
    pathsNode->clear();

    int levelWidth = currentLevel[0].size();  // Lấy số cột từ currentLevel
    int levelHeight = currentLevel.size();    // Lấy số hàng từ currentLevel
    
    auto visibleSize = ax::Director::getInstance()->getVisibleSize();
    auto origin = ax::Director::getInstance()->getVisibleOrigin();
    float startX = origin.x + (visibleSize.width - levelWidth * GRID_SIZE) / 2;
    float startY = origin.y + (visibleSize.height - levelHeight * GRID_SIZE) / 2;
    
    for (const auto& pair : paths) {
        const auto& path = pair.second;
        if (path.points.size() < 2) continue;
        
        for (size_t i = 1; i < path.points.size(); i++) {
            const auto& p1 = path.points[i-1];
            const auto& p2 = path.points[i];
            
            float x1 = startX + p1.x * GRID_SIZE + GRID_SIZE / 2;
            float y1 = startY + (levelHeight - 1 - p1.y) * GRID_SIZE + GRID_SIZE / 2;
            float x2 = startX + p2.x * GRID_SIZE + GRID_SIZE / 2;
            float y2 = startY + (levelHeight - 1 - p2.y) * GRID_SIZE + GRID_SIZE / 2;
            
            pathsNode->drawLine(
                ax::Vec2(x1, y1),
                ax::Vec2(x2, y2),
                getColorFromString(path.color)
            );
        }
    }
}

void BoardGame::handleTouchBegan(const ax::Vec2& touchPos) {
    Point gridPos = getGridPosition(touchPos);
    if (gridPos.x >= 0 && gridPos.x < currentLevel[0].size() &&
        gridPos.y >= 0 && gridPos.y < currentLevel.size()) {
        
        std::string color = std::string(1, currentLevel[gridPos.y][gridPos.x]);
        if (color != "0") {
            if (currentColor.empty()) {  // Nếu chưa có màu nào được chọn
                currentColor = color;
                currentStart = new Point(gridPos);  // Lưu điểm bắt đầu
                lastGridPos = new Point(gridPos);   // Cập nhật điểm cuối cùng

                paths[currentColor] = Path{{gridPos}, currentColor};  // Khởi tạo đường mới
                drawPaths();
        }
    }
}
}

void BoardGame::handleTouchMoved(const ax::Vec2& touchPos) {
    if (lastGridPos == nullptr || currentColor.empty()) return;
   
    Point gridPos = getGridPosition(touchPos);
    if (gridPos.x >= 0 && gridPos.x < currentLevel[0].size() &&
        gridPos.y >= 0 && gridPos.y < currentLevel.size()) {
        
        if (gridPos == *lastGridPos) return;  // Tránh vẽ lại cùng điểm

        auto& currentPath = paths[currentColor];
        // Kiểm tra xem có thể thêm điểm vào đường vẽ không
        // Kiểm tra xem điểm mới có nằm trong đường đi hiện tại không
        auto it = std::find_if(currentPath.points.begin(), currentPath.points.end(),
            [&gridPos](const Point& p) { return p == gridPos; });
            
        if (it != currentPath.points.end()) {
            // Nếu tìm thấy điểm, xóa tất cả các điểm sau điểm này
            currentPath.points.erase(it + 1, currentPath.points.end());
            *lastGridPos = gridPos;
            drawPaths();
            return;
        }
        
        // Kiểm tra xem có phải là điểm liền kề và hợp lệ không
        if (isValidMove(*lastGridPos, gridPos) && canAddToPath(gridPos)) {
            currentPath.points.push_back(gridPos);
            *lastGridPos = gridPos;
            drawPaths();
            
            // Nếu kết nối với điểm cùng màu khác
            std::string endColor = std::string(1, currentLevel[gridPos.y][gridPos.x]);
            if (endColor == currentColor && gridPos == *currentStart) {
                // Hoàn thành đường đi
                currentColor = "";
                delete lastGridPos;
                delete currentStart;
                lastGridPos = nullptr;
                currentStart = nullptr;
            }
        }
    }
}

void BoardGame::handleTouchEnded() {
    if (lastGridPos) {
        auto& currentPath = paths[currentColor];
        std::string endColor = std::string(1, currentLevel[lastGridPos->y][lastGridPos->x]);
        
        // Nếu không kết thúc ở điểm cùng màu, xóa đường đi
        if (endColor != currentColor) {
            paths.erase(currentColor);
            drawPaths();
        }
        
        delete lastGridPos;
        delete currentStart;
        lastGridPos = nullptr;
        currentStart = nullptr;
        currentColor = "";
    }
}

bool BoardGame::isValidMove(const Point& from, const Point& to) const {
    return std::abs(from.x - to.x) <= 1 && std::abs(from.y - to.y) <= 1;  // Di chuyển một ô
}

bool BoardGame::canAddToPath(const Point& point) const {
    // Kiểm tra xem điểm có trống không
    if (point.color != "0" && point.color != currentColor) {
        return false;
    }
    
    // Kiểm tra xem điểm có nằm trên đường đi khác không
    for (const auto& pair : paths) {
        if (pair.first != currentColor) {
            for (const auto& pathPoint : pair.second.points) {
                if (pathPoint == point) {
                    return false;
                }
            }
        }
    }
    return true;
}

