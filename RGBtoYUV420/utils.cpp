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
        std::cerr << "Некорректное значение номера стандарта видео. Используйте значение из списка." << std::endl;
        return false;
    }
    catch (const std::out_of_range&)
    {
        std::cerr << "Некорректное значение номера стандарта видео. Используйте значение из списка." << std::endl;
        return false;
    }

    if (standardId < 0 || standardId > standardResolutions.size() - 1)
    {
        std::cerr << "Некорректное значение номера стандарта видео. Используйте значение из списка." << std::endl;
        return false;
    }

    width = standardResolutions[standardId].width;
    height = standardResolutions[standardId].height;
    std::cout << "Выбрано разрешение видео: " << width << "x" << height << std::endl;

    return true;
};

void printHelp()
{
    std::cout << "Выполняет наложение BMP (RGB 24 bit/px без альфы, палитры и компрессии) на YUV420 видеоряд с преобразованием." << std::endl
        << "Использование: имя входного .bmp, имя входного видео .yuv, номер стандарта видео,"
        << "имя выходного файла .yuv." << std::endl
        << "Имена файлов необходимого указывать с расширением." << std::endl
        << "Поддерживаемые стандарты видео:" << std::endl;
    int id = 0;
    for (auto it = standardResolutions.begin(); it != standardResolutions.end(); ++it)
    {
        std::cout << id++ << " - " << it->title << " " << it->width << "x" << it->height << "." << std::endl;
    }
};