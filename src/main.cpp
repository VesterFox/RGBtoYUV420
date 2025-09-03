#include <string>
#include <iostream>
#include <fstream>
#include <thread>

#include "yuv.h"
#include "bmp.h"
#include "utils.h"

bool OverlayBMPonYUV(const std::string& inputYUVVideoFilename, const std::string& inputBMPFilename,
    const std::string& outputFilename, int videoWidth, int videoHeight, int xOffset, int yOffset, bool placeInCenter)
{
    // Чтение входного видео и сбор необходимой информации о нём
    std::ifstream inputYUVFile(inputYUVVideoFilename, std::ios::binary);
    if (!inputYUVFile) {
        std::cerr << "Ошибка чтения файла." << std::endl;
        return false;
    }

    inputYUVFile.seekg(0, std::ios::end);
    uint64_t yuvSize = inputYUVFile.tellg();
    inputYUVFile.seekg(0, std::ios::beg);

    int y_size = videoWidth * videoHeight;
    int uv_size = (videoWidth / 2) * (videoHeight / 2);
    int frame_size = y_size + (uv_size * 2);
    int frameAmount = yuvSize / frame_size;

    unsigned int nThreads = std::max(1u, std::thread::hardware_concurrency());

    // Чтение входного BMP и сбор необходимой информации о нём. Конвертация в YUV420
    BMPImage bmpImage(inputBMPFilename);

    if (!bmpImage.isValidForVideo(videoWidth, videoHeight)) return false;

    YUVFrame overlayYUV(bmpImage.getWidth(), bmpImage.getHeight());
    overlayYUV = bmpImage.toYUV(nThreads);

    // Наложение и запись результата через кольцевой буффер
    YUVRingBuffer videoFrameBuffer(nThreads, videoWidth, videoHeight);

    unsigned int remains = frameAmount;

    std::ofstream outStream(outputFilename, std::ios::binary | std::ios::trunc);
    if (!outStream)
    {
        std::cerr << "Ошибка записи yuv файла." << std::endl;
        return false;
    }

    bool firstPass = true;

    while (remains > 0)
    {
        int frameAmountInBuffer = std::min(remains, nThreads);
        for (int i = 0; i < frameAmountInBuffer; i++)
        {
            auto& frame = videoFrameBuffer.nextFrame();
            frame.readFromFile(inputYUVFile, y_size, uv_size);
        }

        // Вызов наложения картинки на видео (по центру или с сдвигом от левого верхнего угла)
        if (placeInCenter)
            overlayYUV.overlayOnVideo(videoFrameBuffer);
        else
            overlayYUV.overlayOnVideo(videoFrameBuffer, xOffset, yOffset);

        videoFrameBuffer.saveToFile(outStream);

        remains -= frameAmountInBuffer;

        // Для последующих батчей вызывается открытие ofstream с флагом app, а не trunc
        if(firstPass)
        {
            firstPass = false;
            if(!ReopenOfstreamForApp(outStream, outputFilename)) return false;
        }
    }

    std::cout << "Файл успешно сохранен." << std::endl;
    return true;
}

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");

    if ((argc < 5 || argc > 6) || (argc > 1 && std::string(argv[1]) == "--help"))
        printHelp();

    std::string inputBMPFilename = argv[1];
    std::string inputYUVVideoFilename = argv[2];
    std::string outputFilename = argv[3];

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

    if (!OverlayBMPonYUV(inputYUVVideoFilename, inputBMPFilename, outputFilename,
        videoWidth, videoHeight, xOffset, yOffset, placeInCenter)) 
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

