#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>

/// <summary>
/// ������ ��������� ���������� ���� �� ��������� ����� � ���������� ���������� ����.
/// </summary>
/// <typeparam name="Type"> ��� ����������, � ������� ����� ��������� ������. </typeparam>
/// <param name="fp"> ������� �������� �����, �������� � �������� ������. </param>
/// <param name="result"> ����������, � ������� ����� ��������� ������. </param>
/// <param name="size"> ���������� ���� ��� ������ �� �����. </param>
/// <returns> True � ������ ������, ����� False. </returns>
template <typename Type>
bool read(std::ifstream& fp, Type& result, std::size_t size)
{
    if (!fp.read(reinterpret_cast<char*>(&result), size))
    {
        std::cerr << "������ ������ �����." << std::endl;
        return false;
    }
    return true;
};

/// <summary>
/// ���� �������� ��������� ����� � ��� ����������.
/// </summary>
struct VideoResolution
{
    std::string title;
    int width;
    int height;
};

/// <summary>
/// ���� �������������� ���������� ����� � �������� �� ����������.
/// </summary>
const std::vector<VideoResolution> standardResolutions =
{
    {"CIF_PAL", 352, 288},
    {"CIF_NTSC", 352, 240},
    {"QCIF_PAL", 176, 144},
    {"QCIF_NTSC", 176, 120}
};

/// <summary>
/// ������ ��������� ��������� ����� �� ���������������� ���������� � ��������� ���������� ����� ���������. (������� ������ � cerr).
/// </summary>
/// <param name="standardId"> ���������� ����� ���������� ���������. </param>
/// <param name="width"> ������������� ������ ���������.</param>
/// <param name="height"> ������������ ������ ���������.</param>
/// <returns> True, � ������ ������, ����� False. </returns>
bool setVideoResolution(char* argvStandatdId, int& width, int& height);

/// <summary>
/// ����� --help.
/// </summary>
void printHelp();