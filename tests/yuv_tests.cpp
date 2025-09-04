#include <gtest/gtest.h>
#include <fstream>
#include <vector>
#include "../src/yuv.h"

YUVFrame createTestFrame(int w, int h, uint8_t yVal = 100, uint8_t uVal = 128, uint8_t vVal = 128)
{
    YUVFrame frame(w,h);

    frame.yPlane.assign(w * h, yVal);
    frame.uPlane.assign((w / 2) * (h / 2), uVal);
    frame.vPlane.assign((w / 2) * (h / 2), vVal);

    return frame;
}

TEST(YUVFrame, ReadFromFile_CorrectData)
{
    std::string filename = "test.yuv";

    std::ofstream out(filename, std::ios::binary);
    ASSERT_TRUE(out.is_open());

    std::vector<uint8_t> y(4, 10), u(1, 20), v(1, 30);
    out.write(reinterpret_cast<char*>(y.data()), y.size());
    out.write(reinterpret_cast<char*>(u.data()), u.size());
    out.write(reinterpret_cast<char*>(v.data()), v.size());
    out.close();

    std::ifstream in(filename, std::ios::binary);
    ASSERT_TRUE(in.is_open());

    YUVFrame frame(2,2);
    frame.readFromFile(in, 4, 1);

    EXPECT_EQ(frame.yPlane, y);
    EXPECT_EQ(frame.uPlane, u);
    EXPECT_EQ(frame.vPlane, v);

    std::remove(filename.c_str());
}

TEST(YUVFrame, OverlayOnFrame_CopiesData)
{
    YUVFrame dst = createTestFrame(4, 4, 0, 0, 0);
    YUVFrame overlay = createTestFrame(2, 2, 255, 128, 128);

    overlay.overlayOnFrame(dst, 1, 1);

    EXPECT_EQ(dst.yPlane[1 * 4 + 1], 255);
    EXPECT_EQ(dst.yPlane[2 * 4 + 2], 255);
}

TEST(YUVFrame, IsBeyondBorders_DetectsOutside)
{
    YUVRingBuffer buffer(1, 4, 4);

    YUVFrame firstFrame = buffer.nextFrame();
    firstFrame = createTestFrame(4, 4);

    YUVFrame overlay = createTestFrame(2, 2);

    // Полностью за пределами
    EXPECT_TRUE(overlay.isBeyondBorders(buffer, 5, 5));
    // Частично внутри
    EXPECT_FALSE(overlay.isBeyondBorders(buffer, 2, 2));
}

TEST(YUVFrame, OverlayOnVideo_AppliesToAllFrames)
{
    YUVRingBuffer buffer(2, 4, 4);

    YUVFrame firstFrame = buffer.nextFrame();
    firstFrame = createTestFrame(4, 4, 0);

    YUVFrame secondFrame = buffer.nextFrame();
    secondFrame = createTestFrame(4, 4, 0);

    YUVFrame overlay = createTestFrame(2, 2, 200);
    overlay.overlayOnVideo(buffer, 1, 1);

    for (int i = 0; i < buffer.size(); i++)
    {
        auto& frame = buffer.getFrame(i);
        EXPECT_EQ(frame.yPlane[5], 200);
    }
}

TEST(YUVRingBuffer, SaveToFile_CorrectData)
{
    YUVRingBuffer buffer(2, 2, 2);

    YUVFrame firstFrame = buffer.nextFrame();
    firstFrame = createTestFrame(2, 2, 50, 60, 70);

    YUVFrame secondFrame = buffer.nextFrame();
    secondFrame = createTestFrame(2, 2, 80, 90, 100);

    std::string filename = "buffer_out.yuv";
    std::ofstream out(filename, std::ios::binary);
    ASSERT_TRUE(out.is_open());

    buffer.saveToFile(out);
    out.close();

    std::ifstream in(filename, std::ios::binary | std::ios::ate);
    ASSERT_TRUE(in.is_open());

    // 2 кадра * (Y=4, U=1, V=1) = 12 байт
    EXPECT_EQ(in.tellg(), 12);

    std::remove(filename.c_str());
}