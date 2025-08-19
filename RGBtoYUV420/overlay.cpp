#include "overlay.h"

bool overlayFrame(YUVFrame& dstFrame, const YUVFrame& overlayFrame,
    int dstWidth, int dstHeight,
    int offsetX, int offsetY,
    int overlayWidth, int overlayHeight)
{   
    // Начало и конец наложения 
    int startX = std::max(0, offsetX);
    int startY = std::max(0, offsetY);

    int endX = std::min(dstWidth, offsetX + overlayWidth);
    int endY = std::min(dstHeight, offsetY + overlayHeight);

    if (startX >= endX || startY >= endY)
        return false;

    for (int y = startY; y < endY; ++y)
    {
        // Смещение в overlay с учетом сдвига
        int srcY = y - offsetY;
        int srcX = startX - offsetX;

        // Сдвиг указателя до строки, которая будет вставлена = 
        // номер строки * ширина строки с учетом возможно выхода за границы + сдвиг 
        const uint8_t* srcLine = overlayFrame.yPlane.data() + srcY * overlayWidth + srcX;
        // Сдвиг указателя до строки, вместо которой будет производиться вставка
        uint8_t* dstLine = dstFrame.yPlane.data() + y * dstWidth + startX;

        std::memcpy(dstLine, srcLine, endX - startX);
    }

    // Начало и конец наложения в UV (Chroma)
    int startXc = startX / 2;
    int startYc = startY / 2;
    int endXc = (endX + 1) / 2;
    int endYc = (endY + 1) / 2;

    // Горизонтальное смещение в overlay с учетом сдвига
    int overlayStartXc = (startX - offsetX) / 2;

    int overlayWidthC = overlayWidth / 2;

    for (int y = startYc; y < endYc; ++y)
    {
        int srcY = y - (offsetY / 2);
        const uint8_t* srcLine = overlayFrame.uPlane.data() + srcY * overlayWidthC + overlayStartXc;
        uint8_t* dstLine = dstFrame.uPlane.data() + y * (dstWidth / 2) + startXc;

        std::memcpy(dstLine, srcLine, endXc - startXc);
    }

    for (int y = startYc; y < endYc; ++y)
    {
        int srcY = y - (offsetY / 2);
        const uint8_t* srcLine = overlayFrame.vPlane.data() + srcY * overlayWidthC + overlayStartXc;
        uint8_t* dstLine = dstFrame.vPlane.data() + y * (dstWidth / 2) + startXc;

        std::memcpy(dstLine, srcLine, endXc - startXc);
    }

    return true;
}

void overlayOnVideo(const YUVFrame& image, YUVVideo& video, int imageWidth, int imageHeight, int xOffset, int yOffset)
{
    for (uint32_t i = 0; i < video.frameAmount; i++)
    {
        if (!overlayFrame(video.frames[i], image, video.width, video.height, xOffset, yOffset, imageWidth, imageHeight))
        {
            std::cout << "Площадь изображения полностью за границами видео." << std::endl 
                << " Нет информации для вставки. Отмена операции вставки." << std::endl;
            break;
        }
    }
}

void overlayOnVideo(const YUVFrame& image, YUVVideo& video, int imageWidth, int imageHeight)
{
    // Координаты наложения изображения по центру видео. 
    int xOffset = floor((video.width - imageWidth) / 2);
    int yOffset = floor((video.height - imageHeight) / 2);

    for (uint32_t i = 0; i < video.frameAmount; i++)
    {
        if (!overlayFrame(video.frames[i], image, video.width, video.height, xOffset, yOffset, imageWidth, imageHeight))
        {
            std::cout << "Площадь изображения полностью за границами видео." << std::endl
                << " Нет информации для вставки. Отмена операции вставки." << std::endl;
            break;
        }
    }
}