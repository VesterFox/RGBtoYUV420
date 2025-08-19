#include <string>
#include <iostream>
#include <fstream>  

#include "yuv.h"
#include "bmp.h"
#include "overlay.h"
#include "utils.h"

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");

    if ((argc < 5 && argc > 6) || argv[1] == "--help")
        printHelp();

    std::string inputBMPFilename = argv[1];
    std::string inputYUVVideoFilename = argv[2];
    std::string outputFilename = argv[3];

    // Получение разрешения видео из аргументов ввода
    int videoWidth, videoHeight = 0;
    if (!setVideoResolution(argv[4], videoWidth, videoHeight)) 
    {
        printHelp();
        return EXIT_FAILURE;
    }

    int xOffset, yOffset = 0;
    bool placeInCenter = false;
    if(!setOffset(argv[5], argv[6], xOffset, yOffset, placeInCenter))
    {
        printHelp();
        return EXIT_FAILURE;
    }

    // Чтение входного видео и сбор необходимой информации о нём
    YUVVideo inputVideo;
    if (!prepareYUVVideo(inputYUVVideoFilename, inputVideo, videoWidth, videoHeight)) return EXIT_FAILURE;

    // Чтение входного BMP и сбор необходимой информации о нём. Конвертация в YUV420
    YUVFrame yuvFrame;
    int imageWidth, imageHeight;
    if (!prepareBMP(inputBMPFilename, inputVideo, yuvFrame, imageWidth, imageHeight)) return EXIT_FAILURE;

    // Вызов наложения картинки на видео (по центру или с сдвигом от левого верхнего угла)
    if (placeInCenter)
        overlayOnVideo(yuvFrame, inputVideo, imageWidth, imageHeight);
    else
        overlayOnVideo(yuvFrame, inputVideo, imageWidth, imageHeight, xOffset, yOffset);

    // Сохранение результата
    if(!saveYUVVideo(outputFilename, inputVideo))
    {
        std::cerr << "Ошибка записи yuv файла." << std::endl;
        return EXIT_FAILURE;
    }
    else
    {
        std::cout << "Файл успешно сохранен." << std::endl;
    }

    return EXIT_SUCCESS;
}

