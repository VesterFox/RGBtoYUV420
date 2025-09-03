#pragma once

#include <cstdint>
#include <string>

#include "yuv.h"

class BMPImage
{
public:
    BMPImage(const std::string& filename)
    {
        load(filename);
    };

    /// <summary>
    ///  Проверка входящего .bmp файла на удв. критериев конвертации.
    /// </summary>
    /// <param name="videoWidth"> Ширина кадра видео. </param>
    /// <param name="videoHeight"> Высота кадра видео. </param>
    /// <returns></returns>
    bool isValidForVideo(int videoWidth, int videoHeight) const;

    /// <summary>
    /// Чтение BMP из потока и передача данных в <see cref="BMPHeader"> и <see cref="BMPInfoHeader">.
    /// </summary>
    /// <param name="filename"> Адрес до .bmp файла. </param>
    void load(const std::string& filename);

    /// <summary>
    /// Многопоточная конвертация .bmp в пространстве RGB в YUV4:2:0 (BT.601 YCbCr). Используется SSE2.
    /// </summary>
    /// <param name="nThreads"> Доступное кол-во потоков.  </param>
    /// <returns></returns>
    YUVFrame toYUV(unsigned int nThreads) const;

    int getWidth() const { return width_; }
    int getHeight() const { return height_; }

    std::vector<uint8_t> imageData{};
private:
    #pragma pack(push, 1)
    struct BMPHeader
    {
        uint16_t fileType{};
        uint32_t fileSize{};
        uint16_t reserved0{};
        uint16_t reserved1{};
        uint32_t offset{};
    } header;
    #pragma pack(pop)

    #pragma pack(push, 1)
    struct BMPInfoHeader
    {
        uint32_t size{};
        int32_t width{};
        int32_t height{};
        uint16_t planes{};
        uint16_t bitCount{};
        uint32_t compression{};
        uint32_t imageSize{};
        int32_t xPixelsPerMeter{};
        int32_t yPixelsPerMeter{};
        uint32_t colorsUsed{};
        uint32_t colorsImportant{};
    } info;
    #pragma pack(pop)

    int width_ = 0;
    int height_ = 0;
    int rowSize_ = 0;

    /// <summary>
    /// Конвертация сегмента .bmp в пространстве RGB в YUV4:2:0 (BT.601 YCbCr). Используется SSE2.
    /// </summary>
    /// <param name="startY"> Строка с которой начинается обработка текущего блока. </param>
    /// <param name="endY"> Строка на которой заканчивается обработка текущего блока. </param>
    /// <param name="yuvResult"> <see cref="YUVFrame"> структура кадра, в которую записывается конвертированный блок. </param>
    inline void convertRGBtoYUVBlock(int startY, int endY, YUVFrame& yuvResult) const;
};