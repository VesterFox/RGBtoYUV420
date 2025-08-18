#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdexcept>

/// <summary>
/// Читает указанное количество байт из бинарного файла в переменную указанного типа.
/// </summary>
/// <typeparam name="Type"> Тип переменной, в которую будут загружены данные. </typeparam>
/// <param name="fp"> Входной файловый поток, открытый в бинарном режиме. </param>
/// <param name="result"> Переменная, в которую будут прочитаны данные. </param>
/// <param name="size"> Количество байт для чтения из файла. </param>
/// <returns> True в случае успеха, иначе False. </returns>
template <typename Type>
bool read(std::ifstream& fp, Type& result, std::size_t size)
{
    if (!fp.read(reinterpret_cast<char*>(&result), size))
    {
        std::cerr << "Ошибка чтения файла." << std::endl;
        return false;
    }
    return true;
};

/// <summary>
/// Пара названия стандарта видео и его разрешения.
/// </summary>
struct VideoResolution
{
    std::string title;
    int width;
    int height;
};

/// <summary>
/// Пары поддерживаемых стандартов видео и значений их разрешений.
/// </summary>
const std::vector<VideoResolution> standardResolutions =
{
    {"CIF_PAL", 352, 288},
    {"CIF_NTSC", 352, 240},
    {"QCIF_PAL", 176, 144},
    {"QCIF_NTSC", 176, 120}
};

/// <summary>
/// Чтение выбраного стандарта видео из пользовательских аргументов и получение разрешения этого стандарта. (Выводит ошибки в cerr).
/// </summary>
/// <param name="standardId"> Порядковый номер выбранного стандарта. </param>
/// <param name="width"> Возвращаяемая ширина стандарта.</param>
/// <param name="height"> Возвращаемая высота стандарта.</param>
/// <returns> True, в случае успеха, иначе False. </returns>
bool setVideoResolution(char* argvStandatdId, int& width, int& height);

/// <summary>
/// Вывод --help.
/// </summary>
void printHelp();