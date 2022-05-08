#include "receiver.h"

#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>

#include <QTime>

const size_t MTU = 1518;    // Maximum transmission unit

std::atomic<bool> closeApp;
std::mutex bitmapGuard;
std::vector<uint8_t> bitmap;
uint16_t bitmapWidth;
uint16_t bitmapHeight;


void Receiver::parseData(uint8_t* buffer, int dataSize) {
    if (dataSize == -1)
        return;
    Hdr* pHdr = (Hdr*)buffer;
    uint32_t offset = ((pHdr->offset_hi & 0x3fff) << 14) | (pHdr->offset_lo & 0x3fff);
    if ((pHdr->signature & 0x8000) || (pHdr->signature & 0x4000) ||
            offset + pHdr->size > pHdr->width * pHdr->height) {
        tBitmap.clear();
        return;
    }
    if ((pHdr->width != tBitmapWidth) ||
            (pHdr->height != tBitmapHeight)) {
        tBitmap.clear();
        tBitmapWidth = pHdr->width;
        tBitmapHeight = pHdr->height;
    }

    for (int i = 0; i < pHdr->size; ++i)
        tBitmap.push_back(*(buffer + sizeof(Hdr) + i));

    if (tBitmap.size() < tBitmapWidth * tBitmapHeight)
        return;

    std::lock_guard<std::mutex> guard(bitmapGuard);
    bitmapWidth = tBitmapWidth;
    bitmapHeight = tBitmapHeight;
    bitmap = tBitmap;
    emit bitmapChanged();
}

void Receiver::receiveData() {
    int sock = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    if (sock == -1) {
        std::cout << "Socket open error\n";
        return;
    }

    sockaddr_in sock_addr;
    inet_aton(options.addr, &sock_addr.sin_addr);
    sock_addr.sin_port = htons(options.port);
    sock_addr.sin_family = AF_INET;

    int res = bind(sock, (sockaddr*)&sock_addr, sizeof(sock_addr));
    if (res == -1) {
        std::cout << "Socket bind error\n";
        return;
    }

    uint8_t buffer[MTU];
    while (true) {
        int dataSize = recv(sock, buffer, MTU, 0);
        if ((dataSize == -1 && errno != EAGAIN) ||
                closeApp.load())
            break;
        parseData(buffer, dataSize);
    }

    close(sock);
}
