#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include "grid.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->grid->clear();

    // 设置默认可编辑状态
    ui->checkBox->setCheckState(Qt::Checked);

    // 设置默认速度
    ui->comboBox->setCurrentText("500");
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 窗口大小调整事件 重新划分内存大小
void MainWindow::resizeEvent(QResizeEvent *event) {
    ui->grid->move(200, 0);
    ui->grid->resize(width() - 200, height());
}

// clear
void MainWindow::on_pushButton_clicked()
{
    ui->grid->clear();
}

// random fill
void MainWindow::on_pushButton_2_clicked()
{
    ui->grid->random_fill();
}

// step
void MainWindow::on_pushButton_3_clicked()
{
    ui->grid->step();
}

// roll back
void MainWindow::on_pushButton_4_clicked()
{
    ui->grid->rollback();
}

// loop
void MainWindow::on_pushButton_5_clicked()
{
    // 若已经在循环中 则先停止循环
    if (_timer.isActive()) {
        _timer.stop();
    }

    // 设置循环进行的操作 即将网格进行一次迭代
    QWidget::connect(&_timer, &QTimer::timeout, this, [&]() {
        ui->grid->step();
    });

    // 获取速度
    int speed = ui->comboBox->currentText().toInt();

    // 启动循环
    _timer.start(speed);
}

// stop
void MainWindow::on_pushButton_6_clicked()
{
    // 若已停止 则不进行操作
    if (! _timer.isActive()) {
        return;
    }

    // 停止循环
    _timer.stop();
}

// speed changed
// 修改速度
void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    // 如果 计时器未启动 则不执行
    if (! _timer.isActive()) {
        return;
    }

    // 停止计时器
    _timer.stop();

    // 获取速度
    int speed = ui->comboBox->currentText().toInt();
    qDebug() << "loop speed" << speed;

    // 启动循环
    _timer.start(speed);
}

// edit mode
// 将用于修改鼠标是否可以编辑元素
void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if (arg1 == Qt::Unchecked) {
        ui->checkBox->setText("disabled");
        ui->grid->set_edit_mode(false);
    }

    else if (arg1 == Qt::Checked) {
        ui->checkBox->setText("enabled");
        ui->grid->set_edit_mode(true);
    }

    qDebug() << ui->checkBox->checkState() << ui->checkBox->text();
}
