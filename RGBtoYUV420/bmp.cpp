#include "bmp.h"
#include "yuv.h"
#include "utils.h"
#include <immintrin.h>

inline void convertRGBtoYUVBlock(const std::vector<uint8_t>& bmpData, int startY, int endY,
    int width, int height, int row_size, YUVFrame& yuvResult)
{
    // BT.601
    const __m128 YR = _mm_set1_ps(0.299f);
    const __m128 YG = _mm_set1_ps(0.587f);
    const __m128 YB = _mm_set1_ps(0.114f);

    const __m128 UR = _mm_set1_ps(-0.169f);
    const __m128 UG = _mm_set1_ps(-0.331f);
    const __m128 UB = _mm_set1_ps(0.500f);

    const __m128 VR = _mm_set1_ps(0.500f);
    const __m128 VG = _mm_set1_ps(-0.419f);
    const __m128 VB = _mm_set1_ps(-0.081f);

    const __m128 add128 = _mm_set1_ps(128.0f);
    const __m128 zero = _mm_set1_ps(0.0f);
    const __m128 max255 = _mm_set1_ps(255.0f);

    // Y шаг 4
    for (int y = startY; y < endY; ++y)
    {
        const int rowNumberInBMP = height - 1 - y;
        const uint8_t* rowData = bmpData.data() + rowNumberInBMP * row_size;
        uint8_t* dstY = yuvResult.yPlane.data() + y * width;

        int x = 0;
        for (; x + 4 <= width; x += 4)
        {
            float Rf[4], Gf[4], Bf[4];
            // RGB24: B,G,R по 3 байта на пиксель
            Rf[0] = rowData[(x + 0) * 3 + 2]; Gf[0] = rowData[(x + 0) * 3 + 1]; Bf[0] = rowData[(x + 0) * 3 + 0];
            Rf[1] = rowData[(x + 1) * 3 + 2]; Gf[1] = rowData[(x + 1) * 3 + 1]; Bf[1] = rowData[(x + 1) * 3 + 0];
            Rf[2] = rowData[(x + 2) * 3 + 2]; Gf[2] = rowData[(x + 2) * 3 + 1]; Bf[2] = rowData[(x + 2) * 3 + 0];
            Rf[3] = rowData[(x + 3) * 3 + 2]; Gf[3] = rowData[(x + 3) * 3 + 1]; Bf[3] = rowData[(x + 3) * 3 + 0];

            __m128 R = _mm_loadu_ps(Rf);
            __m128 G = _mm_loadu_ps(Gf);
            __m128 B = _mm_loadu_ps(Bf);

            __m128 Yf = _mm_add_ps(_mm_add_ps(_mm_mul_ps(R, YR), _mm_mul_ps(G, YG)), _mm_mul_ps(B, YB));
            Yf = _mm_min_ps(_mm_max_ps(Yf, zero), max255);

            alignas(16) float Ytmp[4];
            _mm_store_ps(Ytmp, Yf);

            // ВАЖНО: пишем в текущую строку по смещению x
            dstY[x + 0] = static_cast<uint8_t>(Ytmp[0] + 0.5f);
            dstY[x + 1] = static_cast<uint8_t>(Ytmp[1] + 0.5f);
            dstY[x + 2] = static_cast<uint8_t>(Ytmp[2] + 0.5f);
            dstY[x + 3] = static_cast<uint8_t>(Ytmp[3] + 0.5f);
        }

        // Хвост
        for (; x < width; ++x)
        {
            const uint8_t B = rowData[x * 3 + 0];
            const uint8_t G = rowData[x * 3 + 1];
            const uint8_t R = rowData[x * 3 + 2];

            int Y = int(0.299f * R + 0.587f * G + 0.114f * B + 0.5f);
            if (Y < 0) 
                Y = 0; 
            else if (Y > 255) 
                Y = 255;
            dstY[x] = static_cast<uint8_t>(Y);
        }
    }

    // UV Начинаем с чётной строки, шаг 8
    int yUVStart = (startY + 1) & ~1; // Ближайшая чётная >= startY
    for (int y = yUVStart; y < endY; y += 2)
    {
        const int bmpRow = height - 1 - y;
        const uint8_t* row = bmpData.data() + bmpRow * row_size;

        int x = 0;
        // обрабатываем 8 «кандидатов» подряд, но записываем каждый второй (x, x+2, x+4, x+6)
        for (; x + 8 <= width; x += 8)
        {
            float Rf[8], Gf[8], Bf[8];
            #pragma unroll
            for (int i = 0; i < 8; ++i) 
            {
                Bf[i] = row[(x + i) * 3 + 0];
                Gf[i] = row[(x + i) * 3 + 1];
                Rf[i] = row[(x + i) * 3 + 2];
            }

            __m128 R1 = _mm_loadu_ps(Rf + 0);
            __m128 R2 = _mm_loadu_ps(Rf + 4);
            __m128 G1 = _mm_loadu_ps(Gf + 0);
            __m128 G2 = _mm_loadu_ps(Gf + 4);
            __m128 B1 = _mm_loadu_ps(Bf + 0);
            __m128 B2 = _mm_loadu_ps(Bf + 4);

            __m128 U1 = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(R1, UR), _mm_mul_ps(G1, UG)), _mm_mul_ps(B1, UB)), add128);
            __m128 V1 = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(R1, VR), _mm_mul_ps(G1, VG)), _mm_mul_ps(B1, VB)), add128);
            __m128 U2 = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(R2, UR), _mm_mul_ps(G2, UG)), _mm_mul_ps(B2, UB)), add128);
            __m128 V2 = _mm_add_ps(_mm_add_ps(_mm_add_ps(_mm_mul_ps(R2, VR), _mm_mul_ps(G2, VG)), _mm_mul_ps(B2, VB)), add128);

            U1 = _mm_min_ps(_mm_max_ps(U1, zero), max255);
            V1 = _mm_min_ps(_mm_max_ps(V1, zero), max255);
            U2 = _mm_min_ps(_mm_max_ps(U2, zero), max255);
            V2 = _mm_min_ps(_mm_max_ps(V2, zero), max255);

            alignas(16) float Utmp[8], Vtmp[8];
            _mm_store_ps(Utmp + 0, U1);
            _mm_store_ps(Utmp + 4, U2);
            _mm_store_ps(Vtmp + 0, V1);
            _mm_store_ps(Vtmp + 4, V2);

            // Индекс в UV (каждый второй пиксель)
            int uvIndex = (y / 2) * (width / 2) + (x / 2);

            // Пишем элементы 0,2,4,6 -> 4 значения U/V
            yuvResult.uPlane[uvIndex + 0] = static_cast<uint8_t>(Utmp[0] + 0.5f);
            yuvResult.uPlane[uvIndex + 1] = static_cast<uint8_t>(Utmp[2] + 0.5f);
            yuvResult.uPlane[uvIndex + 2] = static_cast<uint8_t>(Utmp[4] + 0.5f);
            yuvResult.uPlane[uvIndex + 3] = static_cast<uint8_t>(Utmp[6] + 0.5f);

            yuvResult.vPlane[uvIndex + 0] = static_cast<uint8_t>(Vtmp[0] + 0.5f);
            yuvResult.vPlane[uvIndex + 1] = static_cast<uint8_t>(Vtmp[2] + 0.5f);
            yuvResult.vPlane[uvIndex + 2] = static_cast<uint8_t>(Vtmp[4] + 0.5f);
            yuvResult.vPlane[uvIndex + 3] = static_cast<uint8_t>(Vtmp[6] + 0.5f);
        }

        // Хвост
        for (; x + 1 < width; x += 2)
        {
            const uint8_t B = row[x * 3 + 0];
            const uint8_t G = row[x * 3 + 1];
            const uint8_t R = row[x * 3 + 2];

            int U = int(-0.169f * R - 0.331f * G + 0.500f * B + 128.5f);
            int V = int(0.500f * R - 0.419f * G - 0.081f * B + 128.5f);

            if (U < 0) 
                U = 0; 
            else if (U > 255) 
                U = 255;

            if (V < 0) 
                V = 0; 
            else if (V > 255) 
                V = 255;

            const int uvIndex = (y / 2) * (width / 2) + (x / 2);
            yuvResult.uPlane[uvIndex] = static_cast<uint8_t>(U);
            yuvResult.vPlane[uvIndex] = static_cast<uint8_t>(V);
        }
    }
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

    std::vector<uint8_t> bmpData(row_size * height);
    file.read(reinterpret_cast<char*>(bmpData.data()), bmpData.size());

    unsigned int nThreads = std::max(1u, std::thread::hardware_concurrency());
    int blockSize = height / nThreads;

    std::vector<std::thread> threads;
    int startY = 0;

    for(unsigned int i = 0; i < nThreads; ++i)
    {
        int endY = (i == nThreads - 1) ? height : startY + blockSize;

        threads.emplace_back
        (
            convertRGBtoYUVBlock, std::cref(bmpData), startY, endY, width, height, row_size, std::ref(yuvResult)
        );

        startY = endY;
    }

    for(auto& t : threads)
    {
        t.join();
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

bool readBMP(std::ifstream& inputStream, BMPHeader& header, BMPInfoHeader& infoHeader)
{
    bool resultStatus = true;
    resultStatus = read(inputStream, header.fileType, sizeof(header.fileType));
    resultStatus = read(inputStream, header.fileSize, sizeof(header.fileSize));
    resultStatus = read(inputStream, header.reserved0, sizeof(header.reserved0));
    resultStatus = read(inputStream, header.reserved1, sizeof(header.reserved1));
    resultStatus = read(inputStream, header.offset, sizeof(header.offset));
    resultStatus = read(inputStream, infoHeader, sizeof(infoHeader));
    return resultStatus;
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