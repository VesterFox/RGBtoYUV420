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
/// ������������� ����������� .bmp � ������������ RGB � YUV4:2:0 (BT.601 YCbCr)
/// </summary>
/// <param name="file"> ����� � ������� �������� �����. </param>
/// <param name="header"> ��������� Bitmap ����� (<see cref="BMPHeader">). </param>
/// <param name="infoHeader"> BitmapInfoHeader (<see cref="BMPInfoHeader">). </param>
/// <param name="yuvResult"> ��������� �����������. </param>
/// <returns> True, � ������ ������, ����� False. </returns>
bool convertRGBtoYUV(std::ifstream& file, const BMPHeader& header, const BMPInfoHeader& infoHeader,
    YUVFrame& yuvResult);

/// <summary>
/// �������� ��������� .bmp ����� (����� ��������� ���������� � ����������  �� ���. ��������� �����������. (������� ������ ����� cerr).
/// </summary>
/// <param name="header"> ��������� Bitmap ����� (<see cref="BMPHeader">). </param>
/// <param name="infoHeader"> BitmapInfoHeader (<see cref="BMPInfoHeader">). </param>
/// <param name="inputVideo"> ����� .yuv, �� ������� ����� ������������� �����������. </param>
/// <returns> True, ���� BMP ����� ���������� ��������������, ����� false. </returns>
bool isCorrectInputFile(BMPHeader header, BMPInfoHeader infoHeader, YUVVideo inputVideo);

/// <summary>
/// ������ BMP �� ������ � �������� ������ � <see cref="BMPHeader"> � <see cref="BMPInfoHeader">.
/// </summary>
/// <param name="inputStream"> ����� � ������� �� �������� .bmp �����. </param>
/// <param name="header"> ��������� Bitmap ����� (<see cref="BMPHeader">). </param>
/// <param name="infoHeader"> BitmapInfoHeader (<see cref="BMPInfoHeader">). </param>
/// <returns> True, ���� �������� �������, ����� False. ������� ��������� � ������ � ���� ��������. </returns>
bool readBMP(std::ifstream& inputStream, BMPHeader& header, BMPInfoHeader& infoHeader);

/// <summary>
/// ������ �������� BMP � ���� ����������� ���������� � ��. ����������� � YUV420.
/// </summary>
/// <param name="fileName"> ��� (����) �� �����, ������� ����������. </param>
/// <param name="inputVideo"> <see cref="YUVVideo"> ��������� ����� YUV. </param>
/// <param name="yuvFrame"> ���� (�����������) YUV � ������� ��������� <see cref="YUVFrame">. ����� ���������� �����������. </param>
/// <param name="imageWidth"> ���������� ������ ���������� �����������.</param>
/// <param name="imageHeight"> ���������� ������ ���������� �����������. </param>
/// <returns> True, ���� �������� �������, ����� False. ������� ��������� � ������ � ���� ��������. </returns>
bool prepareBMP(std::string fileName, YUVVideo inputVideo, YUVFrame& yuvFrame, int& imageWidth, int& imageHeight);