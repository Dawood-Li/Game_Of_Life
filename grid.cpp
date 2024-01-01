#include "grid.h"
#include "./ui_grid.h"

Grid::Grid(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Grid)
    , _editable(false)
{
    ui->setupUi(this);

    history.resize(10);

    // 手动触发一次ResizeEvent复用其逻辑进行内存申请
    resizeEvent(nullptr);
}

Grid::~Grid()
{
    delete ui;
}

// 窗口大小调整事件 重新划分内存大小
void Grid::resizeEvent(QResizeEvent *event) {

    // 根据窗口大小和单元格大小 确定横纵列的单元格数量
    // 这里为了绘图美观 进行向上取整
    _grid_width = (width() + CELL_SIZE) / CELL_SIZE;
    _grid_height = (height() + CELL_SIZE) / CELL_SIZE;

    // 对队列中的所有帧进行resize
    for (auto & frame : history) {
        frame.resize(_grid_width + 2);
        for (auto & row : frame) {
            row.resize(_grid_height + 2);
        }

        qDebug() << "frame resize" << frame.size() << frame[0].size();
    }
}

// 绘制事件 将buffer1中的数据绘制到屏幕上 由Qt触发
void Grid::paintEvent(QPaintEvent *event) {
    
    // 准备QPainter用于绘图
    QPainter painter(this);
    painter.setPen(_gridcolor);

    // 方块上色
    // 为了简化在迭代过程中的越界判定 数据会比渲染的网格多一圈
    // 注意数据的地址 与 界面渲染的坐标 是偏移一位的
    // 新的修正： 绘制的是队首的一帧
    for (int w = 0; w < _grid_width; ++w) {
        for (int h = 0; h < _grid_height; ++h) {
            auto & color = history.front()[w+1][h+1] == 1 ? _cellcolor : _bgcolor;
            painter.fillRect(w * CELL_SIZE, h * CELL_SIZE, CELL_SIZE, CELL_SIZE, color);
        }
    }

    // 描边框 这里只针对渲染界面
    for (int w = 0; w < _grid_width; ++w) {
        for (int h = 0; h < _grid_height; ++h) {
            painter.drawLine(
                QPointF(w * CELL_SIZE + CELL_SIZE, h * CELL_SIZE),
                QPointF(w * CELL_SIZE + CELL_SIZE, (h + 1) * CELL_SIZE)
            );
            painter.drawLine(
                QPointF(w * CELL_SIZE, h * CELL_SIZE + CELL_SIZE),
                QPointF((w + 1) * CELL_SIZE, h * CELL_SIZE + CELL_SIZE)
            );
        }
    }
}

// 清空
void Grid::clear() {

    // 为了简化在迭代过程中的越界判定 数据会比渲染的网格多一圈
    // 最外圈的值被初始化为0 且在之后不会被更改
    // 注意计数器对边界的做法
    // 新的修正： 为了防止意外触发界面刷新 导致绘制未完成的一帧
    // 先在队尾的一帧进行绘制 后将队尾帧踢出 再插入到队首
    for (int w = 1; w <= _grid_width; ++w) {
        for (int h = 1; h <= _grid_height; ++h) {
            history.back()[w][h] = 0;
        }
    }

    history.push_front(std::move(history.back()));
    history.pop_back();

    // 刷新显示
    update();
}

// 随机填充
void Grid::random_fill() {

    // 新的修正： 为了防止意外触发界面刷新 导致绘制未完成的一帧
    // 先在队尾的一帧进行绘制 后将队尾帧踢出 再插入到队首
    for (int w = 1; w <= _grid_width; ++w) {
        for (int h = 1; h <= _grid_height; ++h) {
            history.back()[w][h] = rand() % 2;
        }
    }

    history.push_front(std::move(history.back()));
    history.pop_back();

    // 刷新显示
    update();
}

// 一次迭代
void Grid::step() {

    // 如果活细胞周围八个位置的活细胞数少于两个，则该位置活细胞死亡；
    // 如果活细胞周围八个位置有两个或三个活细胞，则该位置活细胞仍然存活；
    // 如果活细胞周围八个位置有超过三个活细胞，则该位置活细胞死亡；
    // 如果死细胞周围正好有三个活细胞，则该位置死细胞复活；

    // 为了简化判定 我给地图多加了一圈 不必再检查边界

    // 新的修正： 为了防止意外触发界面刷新 导致绘制未完成的一帧
    // 先在队尾的一帧进行绘制 后将队尾帧踢出 再插入到队首

    for (int w = 1; w <= _grid_width; ++w) {
        for (int h = 1; h <= _grid_height; ++h) {

            // 1 求和
            int x = 0;
            x += history.front()[w-1][h-1];
            x += history.front()[w-1][h  ];
            x += history.front()[w-1][h+1];
            x += history.front()[w  ][h-1];
            x += history.front()[w  ][h+1];
            x += history.front()[w+1][h-1];
            x += history.front()[w+1][h  ];
            x += history.front()[w+1][h+1];

            // 2 编程抽象判定逻辑
            history.back()[w][h] = x == 3 || history.front()[w][h] && x == 2;
        }
    }

    // 抽出队尾插入队头
    history.push_front(std::move(history.back()));
    history.pop_back();

    // 刷新显示
    update();
}

// 回滚到前一个状态
void Grid::rollback() {

    // 绘制的反向操作 将队首的一帧插回队尾 即可rollback
    // 考虑的更新： 加入可rollback次数，防止回滚超过10次 或读取到未被绘制的空的帧
    history.push_back(std::move(history.front()));
    history.pop_front();
    update();
}

// 设置可编辑状态
void Grid::set_edit_mode(bool mode) {
    _editable = mode;
}

// 获取可编辑状态
bool Grid::get_edit_mode() {
    return _editable;
}

void Grid::mousePressEvent(QMouseEvent *event) {

    // 如果不在编辑模式下 则拒绝执行
    if (! _editable) {
        return;
    }

    // 根据屏幕坐标计算方格坐标
    int x = event->pos().x() / CELL_SIZE;
    int y = event->pos().y() / CELL_SIZE;

    // 越界检查
    if (x < 0 || x > _grid_width || y < 0 || y > _grid_height) {
        return;
    }

    // 检查其是否是左键或右键
    // 因为若此event是move转发来的
    // 则event->button()将会变成Qt::NoButton
    // 需要使用存储的_currentButton来确认状态
    // 否则则视为点击了某键 记录鼠标状态
    if (event->button() != Qt::NoButton) {
        _currentButton = event->button();
    }

    // 使用记录的状态来操作数据
    if (_currentButton == Qt::LeftButton) {
        history.front()[x+1][y+1] = 1;
    } else if (_currentButton == Qt::RightButton) {
        history.front()[x+1][y+1] = 0;
    }

    // 更新显示
    update();
}

void Grid::mouseReleaseEvent(QMouseEvent *event) {
    // 其实没啥意义的一步操作
    // 只是为了顺手也写一下Release事件
    // 万一以后别的项目用得到呢
    _currentButton = Qt::NoButton;
}

void Grid::mouseMoveEvent(QMouseEvent *event) {
    // 转交给pressEvent处理
    // 但是此时event->button()将会变成Qt::NoButton
    // 所以需要一个临时变量_currentButton来存储鼠标的状态
    mousePressEvent(event);
}
