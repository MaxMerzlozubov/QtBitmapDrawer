#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>
#include <QPainter>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent*) override;
    void closeEvent(QCloseEvent*) override;

private:
    std::unique_ptr<Ui::MainWindow> ui;

    QTime m_time;
    int m_frameCount;
    bool draw;

public slots:
    void repaintBitmap();

private slots:
    void startStop();
};

#endif // MAINWINDOW_H
