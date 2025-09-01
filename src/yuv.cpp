#include "yuv.h"
#include <iostream>

YUVFrame readYUVFrame(std::ifstream& fp, int YSize, int UVSize)
{
    YUVFrame result{};

    result.yPlane.resize(YSize);
    result.uPlane.resize(UVSize);
    result.vPlane.resize(UVSize);

    fp.read(reinterpret_cast<char*>(result.yPlane.data()), YSize);
    fp.read(reinterpret_cast<char*>(result.uPlane.data()), UVSize);
    fp.read(reinterpret_cast<char*>(result.vPlane.data()), UVSize);

    return result;
}

void saveYUVFrames(std::ofstream& outStream, YUVRingBuffer& yuv)
{
    for (int i = 0; i < yuv.size(); i++)
    {
        outStream.write(reinterpret_cast<const char*>(yuv.getFrame(i).yPlane.data()), yuv.getFrame(i).yPlane.size());
        outStream.write(reinterpret_cast<const char*>(yuv.getFrame(i).uPlane.data()), yuv.getFrame(i).uPlane.size());
        outStream.write(reinterpret_cast<const char*>(yuv.getFrame(i).vPlane.data()), yuv.getFrame(i).vPlane.size());
    }
}