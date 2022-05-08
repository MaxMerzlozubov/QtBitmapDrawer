#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>
#include <atomic>
#include <vector>
#include <mutex>

struct Hdr {
    uint16_t signature;  //Тег: 0x3fff
    uint16_t width;      //Ширина (в пикселях)
    uint16_t height;     //Высота (в пикселях)
    uint16_t offset_lo;  //Младшие 14 бит смещения (в пикселях)
    uint16_t offset_hi;  //Старшие 14 бит смещения (в пикселях)
    uint16_t size;       //Размер блока (в пикселях)
    uint16_t res1;       //Резерв
    uint16_t res2;       //Резерв
};

struct ReceiverOptions {
    const char* addr;
    uint16_t port;
};

extern std::atomic<bool> closeApp;
extern std::mutex bitmapGuard;
extern std::vector<uint8_t> bitmap;
extern uint16_t bitmapWidth;
extern uint16_t bitmapHeight;

class Receiver : public QObject
{
    Q_OBJECT

public:
    Receiver(ReceiverOptions opt, QObject* parent = nullptr) :
        QObject(parent), options(opt) {
        closeApp.store(false);
    }
    virtual ~Receiver() {}
    void parseData(uint8_t* buffer, int dataSize);
    void receiveData();

private:
    ReceiverOptions options;
    std::vector<uint8_t> tBitmap;
    uint16_t tBitmapWidth;
    uint16_t tBitmapHeight;

signals:
    void bitmapChanged();
};

#endif // RECEIVER_H
