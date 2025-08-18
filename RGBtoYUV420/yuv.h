#pragma once

#include <vector>
#include <fstream>

struct YUVFrame
{
    std::vector<uint8_t> yPlane;
    std::vector<uint8_t> uPlane;
    std::vector<uint8_t> vPlane;
};

struct YUVVideo
{
    int width = 0;
    int height = 0;
    uint32_t frameAmount = 0;
    std::vector<YUVFrame> frames;
};

/// <summary>
/// ������ ����� �� ifstream � ������ � ��������� YUVVideo.
/// </summary>
/// <param name="fp"> ����� � ����������� �� .yuv ����� �����.</param>
/// <param name="yuvVideo"> ������ �� <see cref="YUVVideo">. </param>
/// <param name="size"> ������ ������� ������ � ifstream. </param>
void readYUVVideo(std::ifstream& fp, YUVVideo& yuvVideo, uint64_t size);

/// <summary>
/// ���������� ����� � .yuv �������.
/// </summary>
/// <param name="outputFilename"> ��� ��������� ����� (������� ����������).</param>
/// <param name="yuv"> <see cref=YUVVideo> ��������� ����� � ������� YUV</param>
/// <returns> True, ���� �������� ��������, ����� False. </returns>
bool saveYUVVideo(const std::string& outputFilename, YUVVideo& yuv);

/// <summary>
/// ������ �������� ����� � ���� ����������� ���������� � ��
/// </summary>
/// <param name="fileName"> ��� (����) �� �����, ������� ����������. </param>
/// <param name="inputVideo"> <see cref="YUVVideo"> ��������� ����� YUV. ����� ���������� ������ �����. </param>
/// <param name="videoWidth"> ������ ���������� �����.</param>
/// <param name="videoHeight"> ������ ���������� �����. </param>
/// <returns> True, ���� �������� �������, ����� False. ������� ��������� � ������ � ���� ��������. </returns>
bool prepareYUVVideo(std::string fileName, YUVVideo& inputVideo, int videoWidth, int videoHeight);