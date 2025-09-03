#include "yuv.h"

#include <cstring>
#include <iostream>
#include <cmath>
#include <cstring>
#include <thread>

void YUVFrame::readFromFile(std::ifstream& fp, int YSize, int UVSize)
{
    yPlane.resize(YSize);
    uPlane.resize(UVSize);
    vPlane.resize(UVSize);

    fp.read(reinterpret_cast<char*>(yPlane.data()), YSize);
    fp.read(reinterpret_cast<char*>(uPlane.data()), UVSize);
    fp.read(reinterpret_cast<char*>(vPlane.data()), UVSize);
}

void YUVRingBuffer::saveToFile(std::ofstream& outStream)
{
    for (int i = 0; i < size(); i++)
    {
        outStream.write(reinterpret_cast<const char*>(getFrame(i).yPlane.data()), getFrame(i).yPlane.size());
        outStream.write(reinterpret_cast<const char*>(getFrame(i).uPlane.data()), getFrame(i).uPlane.size());
        outStream.write(reinterpret_cast<const char*>(getFrame(i).vPlane.data()), getFrame(i).vPlane.size());
    }
}

void YUVFrame::overlayOnFrame(YUVFrame& dstFrame, int xOffset, int yOffset)
{
    // Начало и конец наложения
    int startX = std::max(0, xOffset);
    int startY = std::max(0, yOffset);

    int endX = std::min(dstFrame.getWidth(), xOffset + getWidth());
    int endY = std::min(dstFrame.getHeight(), yOffset + getHeight());


    for (int y = startY; y < endY; ++y)
    {
        // Смещение в overlay с учетом сдвига
        int srcY = y - yOffset;
        int srcX = startX - xOffset;

        // Сдвиг указателя до строки, которая будет вставлена =
        // номер строки * ширина строки с учетом возможно выхода за границы + сдвиг
        const uint8_t* srcLine = yPlane.data() + srcY * getWidth() + srcX;
        // Сдвиг указателя до строки, вместо которой будет производиться вставка
        uint8_t* dstLine = dstFrame.yPlane.data() + y * dstFrame.getWidth() + startX;

        std::memcpy(dstLine, srcLine, endX - startX);
    }

    // Начало и конец наложения в UV (Chroma)
    int startXc = startX / 2;
    int startYc = startY / 2;
    int endXc = (endX + 1) / 2;
    int endYc = (endY + 1) / 2;

    // Горизонтальное смещение в overlay с учетом сдвига
    int overlayStartXc = (startX - xOffset) / 2;

    int overlayWidthC = getWidth() / 2;

    for (int y = startYc; y < endYc; ++y)
    {
        int srcY = y - (yOffset / 2);
        const uint8_t* srcLine = uPlane.data() + srcY * overlayWidthC + overlayStartXc;
        uint8_t* dstLine = dstFrame.uPlane.data() + y * (dstFrame.getWidth() / 2) + startXc;

        std::memcpy(dstLine, srcLine, endXc - startXc);
    }

    for (int y = startYc; y < endYc; ++y)
    {
        int srcY = y - (yOffset / 2);
        const uint8_t* srcLine = vPlane.data() + srcY * overlayWidthC + overlayStartXc;
        uint8_t* dstLine = dstFrame.vPlane.data() + y * (dstFrame.getWidth() / 2) + startXc;

        std::memcpy(dstLine, srcLine, endXc - startXc);
    }
}

bool YUVFrame::isBeyondBorders(const YUVRingBuffer& video, const int xOffset, const int yOffset)
{
    int startX = std::max(0, xOffset);
    int startY = std::max(0, yOffset);

    int endX = std::min(video.getWidth(), xOffset + getWidth());
    int endY = std::min(video.getHeight(), yOffset + getHeight());

    if (startX >= endX || startY >= endY)
    {
        std::cout << "Площадь изображения полностью за границами видео." << std::endl
            << " Нет информации для вставки. Отмена операции вставки." << std::endl;
        return true;
    }
    return false;
}

void YUVFrame::overlayOnVideo(YUVRingBuffer& frames, int xOffset, int yOffset)
{
    if(isBeyondBorders(frames, xOffset, yOffset)) return;

    std::vector<std::thread> threads;
    threads.reserve(frames.size());

    for (int i = 0; i < frames.size(); i++)
    {
        threads.emplace_back
        (
            &YUVFrame::overlayOnFrame, this,
            std::ref(frames.getFrame(i)), xOffset, yOffset
        );
    }

    for (auto& t : threads)
    {
        t.join();
    }
}

void YUVFrame::overlayOnVideo(YUVRingBuffer& frames)
{
    int xOffset = static_cast<int>(std::floor((frames.getWidth() - getWidth()) / 2));
    int yOffset = static_cast<int>(std::floor((frames.getHeight() - getHeight()) / 2));

    if (isBeyondBorders(frames, xOffset, yOffset)) return;

    std::vector<std::thread> threads;
    threads.reserve(frames.size());

    for (int i = 0; i < frames.size(); i++)
    {
        threads.emplace_back
        (
            &YUVFrame::overlayOnFrame, this,
            std::ref(frames.getFrame(i)), xOffset, yOffset
        );
    }

    for (auto& t : threads)
    {
        t.join();
    }
}