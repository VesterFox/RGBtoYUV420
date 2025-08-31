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

bool saveYUVFrames(const std::string& outputFilename, YUVRingBuffer& yuv)
{
    std::ofstream out(outputFilename, std::ios::binary | std::ios::app);
    if (!out)
        return false;

    for (int i = 0; i < yuv.size(); i++)
    {
        out.write(reinterpret_cast<const char*>(yuv.getFrame(i).yPlane.data()), yuv.getFrame(i).yPlane.size());
        out.write(reinterpret_cast<const char*>(yuv.getFrame(i).uPlane.data()), yuv.getFrame(i).uPlane.size());
        out.write(reinterpret_cast<const char*>(yuv.getFrame(i).vPlane.data()), yuv.getFrame(i).vPlane.size());
    }

    return true;
}