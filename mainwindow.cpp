#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <mutex>

extern std::atomic<bool> closeApp;
extern std::mutex bitmapGuard;
extern std::vector<uint8_t> bitmap;
extern uint16_t bitmapWidth;
extern uint16_t bitmapHeight;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(std::make_unique<Ui::MainWindow>())
    , m_frameCount(0)
    , draw(true)
{
    ui->setupUi(this);

    m_time.start();

    QPushButton* startButton = ui->pushButton;
    connect(startButton, SIGNAL(released()), this, SLOT(startStop()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::paintEvent(QPaintEvent*)
{
    // Draw FPS
    if (m_time.elapsed() > 2000) {
        m_time.restart();
        m_frameCount = 0;
    }
    else
        ui->lcdNumber->display(m_frameCount / (float(m_time.elapsed()) / 1000.0f));

    if (!draw ||
            bitmap.size() < bitmapHeight * bitmapWidth)
        return;

    static uint16_t prevbitmapWidth = bitmapWidth;
    static uint16_t prevbitmapHeight = bitmapHeight;
    if (prevbitmapWidth != bitmapWidth ||
            prevbitmapHeight != bitmapHeight) {
        resize(bitmapWidth, bitmapHeight);
        prevbitmapWidth = bitmapWidth;
        prevbitmapHeight = bitmapHeight;
    }
    QPainter painter;
    painter.begin(this);

    std::lock_guard<std::mutex> guard(bitmapGuard);
    QImage image = QImage(bitmapWidth, bitmapHeight, QImage::Format_Grayscale8);
    for (int line = 0; line < bitmapHeight; ++line)
        for (int column = 0; column < bitmapWidth; ++column)
            image.setPixel(column, line, QColor::fromHsv(0, 0, bitmap[line * bitmapWidth + bitmapHeight]).rgb()); // hsv, v - brightness

    painter.drawImage(0,0,image);

    painter.end();

    m_frameCount++;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    closeApp.store(true);
    QMainWindow::closeEvent(event);
}

void MainWindow::repaintBitmap() {
    repaint();
}

void MainWindow::startStop() {
    draw = !draw;
}
