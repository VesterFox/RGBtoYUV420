#include "overlay.h"

void overlayFrame(YUVFrame& dstFrame, const YUVFrame& overlayFrame,
    int dstWidth, int dstHeight,
    int offsetX, int offsetYpos,
    int overlayWidth, int overlayHeight)
{
    // Y
    for (int row = 0; row < overlayHeight; ++row)
    {
        // Сдвиг указателя на начало подстроки, с которой начнется наложения
        // (Отступ в пикселях по вертикали + кол-во обработанных строк) * длину строки + отступ по горизонтали слева
        uint8_t* dstLine = dstFrame.yPlane.data() + (offsetYpos + row) * dstWidth + offsetX;

        // Сдвиг указателя на текущую строку изображения
        const uint8_t* srcLine = overlayFrame.yPlane.data() + row * overlayWidth;

        std::memcpy(dstLine, srcLine, overlayWidth);
    }

    int chromaWidth = dstWidth / 2;
    int overlayChromaWidth = overlayWidth / 2;
    int overlayChromaHeight = overlayHeight / 2;
    int overlayChromaX = offsetX / 2;
    int overlayChromaYpos = offsetYpos / 2;

    // U
    for (int row = 0; row < overlayChromaHeight; ++row)
    {
        uint8_t* dstLine = dstFrame.uPlane.data() + (overlayChromaYpos + row) * chromaWidth + overlayChromaX;
        const uint8_t* srcLine = overlayFrame.uPlane.data() + row * overlayChromaWidth;

        std::memcpy(dstLine, srcLine, overlayChromaWidth);
    }

    // V
    for (int row = 0; row < overlayChromaHeight; ++row)
    {
        uint8_t* dstLine = dstFrame.vPlane.data() + (overlayChromaYpos + row) * chromaWidth + overlayChromaX;
        const uint8_t* srcLine = overlayFrame.vPlane.data() + row * overlayChromaWidth;

        std::memcpy(dstLine, srcLine, overlayChromaWidth);
    }
}

void overlayOnVideo(const YUVFrame& image, YUVVideo& video, int imageWidth, int imageHeight)
{
    // Координаты наложения изображения по центру видео. 
    int yHorOffset = floor((video.width - imageWidth) / 2);
    int yVerOffset = floor((video.height - imageHeight) / 2);

    for (uint32_t i = 0; i < video.frameAmount; i++)
    {
        overlayFrame(video.frames[i], image, video.width, video.height, yHorOffset, yVerOffset, imageWidth, imageHeight);
    }
}