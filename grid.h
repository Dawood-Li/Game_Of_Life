#ifndef GRID_H
#define GRID_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>

#include <vector>
#include <deque>

QT_BEGIN_NAMESPACE
namespace Ui {
class Grid;
}
QT_END_NAMESPACE

class Grid : public QWidget
{
    Q_OBJECT

public:
    
    Grid(QWidget *parent = nullptr);
    ~Grid();

    void random_fill();            // 随机填充
    void clear();                  // 清空
    void step();                   // 进行一次迭代
    void rollback();               // 进行一次回滚
    void set_edit_mode(bool mode); // 设置可编辑状态
    bool get_edit_mode();          // 获取可编辑状态

private:
    Ui::Grid *ui;

    const QColor _gridcolor = QColor(0x44, 0x44, 0x44); // 网格色
    const QColor _bgcolor   = QColor(0x22, 0x22, 0x22); // 0
    const QColor _cellcolor = QColor(0x14, 0x30, 0x50); // 1
    const int MAX_HISTORY_SIZE = 15; // 最大历史记录长度
    const int CELL_SIZE = 10;        // 每个cell的边长

    // 用于存储历史状态的deque
    std::deque<std::vector<std::vector<uint8_t>>> history;

    int _grid_width, _grid_height;   // 网格宽高 被resizeEvent赋值 由paintEvent使用
    bool _editable = false;          // 网格是否可编辑

    Qt::MouseButton _currentButton = Qt::NoButton; // 用于拖动鼠标时记录鼠标点击的键位

protected:

    void paintEvent(QPaintEvent *event) override;        // 绘制事件
    void resizeEvent(QResizeEvent *event) override;      // 窗口大小重设事件
    void mousePressEvent(QMouseEvent *event) override;   // 鼠标按下事件
    void mouseReleaseEvent(QMouseEvent *event) override; // 鼠标抬起事件
    void mouseMoveEvent(QMouseEvent *event) override;    // 鼠标拖动事件
};
#endif // GRID_H
