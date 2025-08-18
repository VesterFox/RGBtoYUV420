#pragma once

#include <cstdint>
#include <fstream>
#include <string>
#include <iostream>
#include <thread>
#include <algorithm>

#include "yuv.h"

struct BMPHeader
{
    uint16_t fileType{};
    uint32_t fileSize{};
    uint16_t reserved0{};
    uint16_t reserved1{};
    uint32_t offset{};
};

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
};

/// <summary>
/// Многопоточная конвертация .bmp в пространстве RGB в YUV4:2:0 (BT.601 YCbCr)
/// </summary>
/// <param name="file"> Поток с данными входного файла. </param>
/// <param name="header"> Заголовок Bitmap файла (<see cref="BMPHeader">). </param>
/// <param name="infoHeader"> BitmapInfoHeader (<see cref="BMPInfoHeader">). </param>
/// <param name="yuvResult"> Результат конвертации. </param>
/// <returns> True, в случае успеха, иначе False. </returns>
bool convertRGBtoYUV(std::ifstream& file, const BMPHeader& header, const BMPInfoHeader& infoHeader,
    YUVFrame& yuvResult);

/// <summary>
/// Проверка входящего .bmp файла (через структуры полученные в результате  на удв. критериев конвертации. (Выводит ошибки через cerr).
/// </summary>
/// <param name="header"> Заголовок Bitmap файла (<see cref="BMPHeader">). </param>
/// <param name="infoHeader"> BitmapInfoHeader (<see cref="BMPInfoHeader">). </param>
/// <param name="inputVideo"> Видео .yuv, на которое будет накладываться изображение. </param>
/// <returns> True, если BMP имеет корректные характеристики, иначе false. </returns>
bool isCorrectInputFile(BMPHeader header, BMPInfoHeader infoHeader, YUVVideo inputVideo);

/// <summary>
/// Чтение BMP из потока и передача данных в <see cref="BMPHeader"> и <see cref="BMPInfoHeader">.
/// </summary>
/// <param name="inputStream"> Поток с данными из входного .bmp файла. </param>
/// <param name="header"> Заголовок Bitmap файла (<see cref="BMPHeader">). </param>
/// <param name="infoHeader"> BitmapInfoHeader (<see cref="BMPInfoHeader">). </param>
/// <returns> True, если операция успешна, иначе False. Выводит сообщения о ошибке и ходе операции. </returns>
bool readBMP(std::ifstream& inputStream, BMPHeader& header, BMPInfoHeader& infoHeader);

/// <summary>
/// Чтение входного BMP и сбор необходимой информации о нём. Конвертация в YUV420.
/// </summary>
/// <param name="fileName"> Имя (путь) до файла, включая расширение. </param>
/// <param name="inputVideo"> <see cref="YUVVideo"> структура видео YUV. </param>
/// <param name="yuvFrame"> Кадр (изображение) YUV в формате структуры <see cref="YUVFrame">. Вывод результата конвертации. </param>
/// <param name="imageWidth"> Полученная ширина разрешения изображения.</param>
/// <param name="imageHeight"> Полученная высота разрешения изображения. </param>
/// <returns> True, если операция успешна, иначе False. Выводит сообщения о ошибке и ходе операции. </returns>
bool prepareBMP(std::string fileName, YUVVideo inputVideo, YUVFrame& yuvFrame, int& imageWidth, int& imageHeight);