#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include "grid.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();                    // clear
    void on_pushButton_2_clicked();                  // random fill
    void on_pushButton_3_clicked();                  // step
    void on_pushButton_4_clicked();                  // roll back
    void on_pushButton_5_clicked();                  // loop
    void on_pushButton_6_clicked();                  // stop
    void on_comboBox_currentIndexChanged(int index); // speed changed
    void on_checkBox_stateChanged(int arg1);         // edit mode, enabled or disabled

protected:

    void resizeEvent(QResizeEvent *event) override;

private:
    Ui::MainWindow *ui;

    QTimer _timer; // 计时器
};
#endif // MAINWINDOW_H
