#include "bmp.h"
#include "yuv.h"
#include "utils.h"

void readBMP(std::ifstream& inputStream, BMPHeader& header, BMPInfoHeader& infoHeader)
{
    read(inputStream, header.fileType, sizeof(header.fileType));
    read(inputStream, header.fileSize, sizeof(header.fileSize));
    read(inputStream, header.reserved0, sizeof(header.reserved0));
    read(inputStream, header.reserved1, sizeof(header.reserved1));
    read(inputStream, header.offset, sizeof(header.offset));
    read(inputStream, infoHeader, sizeof(infoHeader));
}

bool convertRGBtoYUV(std::ifstream& file, const BMPHeader& header, const BMPInfoHeader& infoHeader,
    YUVFrame& yuvResult)
{
    int width = infoHeader.width;
    int height = infoHeader.height;
    int row_size = ((width * 3 + 3) / 4) * 4;

    yuvResult.yPlane.resize(width * height);
    yuvResult.uPlane.resize((width / 2) * (height / 2));
    yuvResult.vPlane.resize((width / 2) * (height / 2));

    std::vector<uint8_t> row(row_size);

    for (int y = height - 1; y >= 0; y--)
    {
        file.read(reinterpret_cast<char*>(row.data()), row_size);

        for (int x = 0; x < width; ++x)
        {
            uint8_t B = row[x * 3 + 0];
            uint8_t G = row[x * 3 + 1];
            uint8_t R = row[x * 3 + 2];

            // Конвертация в YUV (BT.601 YCbCr)
            int Y = static_cast<int>(0.299 * R + 0.587 * G + 0.144 * B);
            int U = static_cast<int>(-0.169 * R - 0.331 * G + 0.500 * B + 128);
            int V = static_cast<int>(0.500 * R - 0.419 * G - 0.081 * B + 128);

            // Ограничение в диапазон 0-255
            Y = std::min(255, std::max(0, Y));
            U = std::min(255, std::max(0, U));
            V = std::min(255, std::max(0, V));

            // Полная Y
            yuvResult.yPlane[y * width + x] = static_cast<uint8_t>(Y);

            // для UV 1 пиксель на 2x2 блок
            if (y % 2 == 0 && x % 2 == 0)
            {
                int uvIndex = (y / 2) * (width / 2) + (x / 2);
                yuvResult.uPlane[uvIndex] = static_cast<uint8_t>(U);
                yuvResult.vPlane[uvIndex] = static_cast<uint8_t>(V);
            }
        }
    }

    return true;
}

bool isCorrectInputFile(BMPHeader header, BMPInfoHeader infoHeader, YUVVideo inputVideo)
{
    std::cout << "Входной BMP:" << std::endl;
    if (header.fileSize < 54)
    {
        std::cerr << "Некорректный размер: " + std::to_string(header.fileSize) + "< 54." << std::endl;
        return false;
    }

    if (header.fileType != 0x4D42)
    {
        std::cerr << "Некорретный тип в заголовке: " + std::to_string(header.fileType) << std::endl;
        return false;
    }

    if (infoHeader.bitCount != 24)
    {
        std::cerr << "Некорректный bpp: " + std::to_string(infoHeader.bitCount) << std::endl;
        return false;
    }

    if (infoHeader.compression != 0 && infoHeader.compression != 11)
    {
        std::cerr << "Некорректное сжатие: id = " + std::to_string(infoHeader.compression) << std::endl;
        return false;
    }

    if (infoHeader.colorsUsed != 0)
    {
        std::cerr << "Некорректная палитра: " + std::to_string(infoHeader.colorsUsed) + " цветов." << std::endl;
        return false;
    }

    if (infoHeader.width > inputVideo.width || infoHeader.height > inputVideo.height)
    {
        std::cerr << "Размер должен быть <= размера видео. \n BMP размер:"
            + std::to_string(infoHeader.width) + "x" + std::to_string(infoHeader.height) + " > видео "
            + std::to_string(inputVideo.width) + "x" + std::to_string(inputVideo.height) << std::endl;
        return false;
    }

    std::cout << std::to_string(infoHeader.bitCount) + " бит/пкс. "
        << "Сжатие: " + std::to_string(infoHeader.compression) << ". "
        << "Палитра: " + std::to_string(infoHeader.colorsUsed) << ". "
        << "Размер: " + std::to_string(infoHeader.width) + "x" + std::to_string(infoHeader.height) << std::endl;

    return true;
}

bool prepareBMP(std::string fileName, YUVVideo inputVideo, YUVFrame& yuvFrame, int& imageWidth, int& imageHeight)
{
    std::ifstream inputBMPFile(fileName, std::ios::binary);
    if (!inputBMPFile) {
        std::cerr << "Ошибка чтения файла." << std::endl;
        return false;
    }

    // Чтение BMP
    BMPHeader header;
    BMPInfoHeader infoHeader;
    readBMP(inputBMPFile, header, infoHeader);

    // Валидация BMP
    if (!isCorrectInputFile(header, infoHeader, inputVideo)) return false;

    // Конвертация BMP
    inputBMPFile.seekg(header.offset, std::ios::beg);

    if (!convertRGBtoYUV(inputBMPFile, header, infoHeader, yuvFrame))
    {
        std::cerr << "Ошибка конвертации BMP в YUV." << std::endl;
        return false;
    }
    else
    {
        std::cout << "Конвертация кадра завершена." << std::endl;
    }

    imageWidth = infoHeader.width;
    imageHeight = infoHeader.height;

    return true;
}