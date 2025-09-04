#include "gtest/gtest.h"
#include "../src/bmp.h"

class BMPFixture : public ::testing::Test
{
protected:
    std::string testFile = "test_2x2.bmp";

    void SetUp() override
    {
        createTestBMP(testFile);
    }

    void TearDown() override
    {
        std::remove(testFile.c_str());
    }

    void createTestBMP(const std::string& filename,
        char typeFrstChar = 'B', short bitCount = 24, int compMethod = 0, int clrUsed = 0)
    {
        // BMP 24bpp, 2x2
        int width = 2;
        int height = 2;
        // Выравнивание на 4 байта
        int rowSize = (width * 3 + 3) & ~3;
        int imageSize = rowSize * height;
        int fileSize = 54 + imageSize;

        std::ofstream out(filename, std::ios::binary);
        ASSERT_TRUE(out.is_open());

        // BMP Header
        // Signature
        out.put(typeFrstChar).put('M');
        // File size
        out.write(reinterpret_cast<char*>(&fileSize), 4);

        int reserved = 0;
        out.write(reinterpret_cast<char*>(&reserved), 4);

        int dataOffset = 54;
        out.write(reinterpret_cast<char*>(&dataOffset), 4);

        // Info Header
        int infoSize = 40;
        out.write(reinterpret_cast<char*>(&infoSize), 4);
        out.write(reinterpret_cast<char*>(&width), 4);
        out.write(reinterpret_cast<char*>(&height), 4);

        short planes = 1;
        out.write(reinterpret_cast<char*>(&planes), 2);

        short bpp = bitCount;
        out.write(reinterpret_cast<char*>(&bpp), 2);

        int compression = compMethod;
        out.write(reinterpret_cast<char*>(&compression), 4);
        out.write(reinterpret_cast<char*>(&imageSize), 4);

        int ppm = 2835; // 72 DPI
        out.write(reinterpret_cast<char*>(&ppm), 4);
        out.write(reinterpret_cast<char*>(&ppm), 4);

        int colorsUsed = clrUsed;
        out.write(reinterpret_cast<char*>(&colorsUsed), 4);

        int colorsImportant = 0;
        out.write(reinterpret_cast<char*>(&colorsImportant), 4);

        // Pixel Data (BGR, СНИЗУ ВВЕРХ)
        // (Нижняя строка) - синий (0 0 255), белый (255 255 255)
        uint8_t row0[8] = { 255,0,0,   255,255,255,   0,0 };
        out.write(reinterpret_cast<char*>(row0), rowSize);

        // (Верхняя строка) красный (255 0 0), зелёный (0 255 0)
        uint8_t row1[8] = { 0,0,255,   0,255,0,   0,0 };
        out.write(reinterpret_cast<char*>(row1), rowSize);

        out.close();
    }
};

TEST(BMP, Load_NonExistentFileThrows)
{
    EXPECT_THROW(BMPImage bmp("no_such_file.bmp"), std::runtime_error);
}

TEST_F(BMPFixture, Load_Existing)
{
    BMPImage bmp(testFile);
}

TEST_F(BMPFixture, Load_Valid)
{
    BMPImage bmp(testFile);
    EXPECT_TRUE(bmp.isValidForVideo(3,3));
}

TEST_F(BMPFixture, Load_Invalid)
{
    BMPImage bmp_large(testFile);
    EXPECT_FALSE(bmp_large.isValidForVideo(1,1));

    createTestBMP(testFile, 'Z');
    BMPImage bmp_unknownType(testFile);
    EXPECT_FALSE(bmp_unknownType.isValidForVideo(5,5));

    createTestBMP(testFile, 'B', 16);
    BMPImage bmp_wrongBBS(testFile);
    EXPECT_FALSE(bmp_wrongBBS.isValidForVideo(5,5));

    createTestBMP(testFile, 'B', 24, 1);
    BMPImage bmp_wrongCompression(testFile);
    EXPECT_FALSE(bmp_wrongCompression.isValidForVideo(5,5));

    createTestBMP(testFile, 'B', 24, 0, 1);
    BMPImage bmp_wrongColor(testFile);
    EXPECT_FALSE(bmp_wrongColor.isValidForVideo(5,5));
}

static inline int RGBtoY(uint8_t r, uint8_t g, uint8_t b)
{
    return static_cast<int>(0.299 * r + 0.587 * g + 0.114 * b);
}

TEST_F(BMPFixture, ToYUV)
{
    BMPImage bmp(testFile);

    YUVFrame yuv = bmp.toYUV(1);

    EXPECT_NEAR(yuv.yPlane[0], RGBtoY(255,0,0), 2);
    EXPECT_NEAR(yuv.yPlane[1], RGBtoY(0,255,0), 2);
    EXPECT_NEAR(yuv.yPlane[2], RGBtoY(0,0,255), 2);
    EXPECT_NEAR(yuv.yPlane[3], RGBtoY(255,255,255), 2);

    for (auto u : yuv.uPlane)
    {
        EXPECT_GE(u, 0);
        EXPECT_LE(u, 255);
    }
    for (auto v : yuv.vPlane)
    {
        EXPECT_GE(v, 0);
        EXPECT_LE(v, 255);
    }
}