#include "utils.h"

bool setVideoResolution(char* argvStandatdId, int& width, int& height)
{
    int standardId = 0;
    try
    {
        standardId = std::stoi(argvStandatdId);
    }
    catch (const std::invalid_argument&)
    {
        std::cerr << "������������ �������� ������ ��������� �����. ����������� �������� �� ������." << std::endl;
        return false;
    }
    catch (const std::out_of_range&)
    {
        std::cerr << "������������ �������� ������ ��������� �����. ����������� �������� �� ������." << std::endl;
        return false;
    }

    if (standardId < 0 || standardId > standardResolutions.size() - 1)
    {
        std::cerr << "������������ �������� ������ ��������� �����. ����������� �������� �� ������." << std::endl;
        return false;
    }

    width = standardResolutions[standardId].width;
    height = standardResolutions[standardId].height;
    std::cout << "������� ���������� �����: " << width << "x" << height << std::endl;

    return true;
};

void printHelp()
{
    std::cout << "��������� ��������� BMP (RGB 24 bit/px ��� �����, ������� � ����������) �� YUV420 �������� � ���������������." << std::endl
        << "�������������: ��� �������� .bmp, ��� �������� ����� .yuv, ����� ��������� �����,"
        << "��� ��������� ����� .yuv." << std::endl
        << "����� ������ ������������ ��������� � �����������." << std::endl
        << "�������������� ��������� �����:" << std::endl;
    int id = 0;
    for (auto it = standardResolutions.begin(); it != standardResolutions.end(); ++it)
    {
        std::cout << id++ << " - " << it->title << " " << it->width << "x" << it->height << "." << std::endl;
    }
};