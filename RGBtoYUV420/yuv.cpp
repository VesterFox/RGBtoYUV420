#include "yuv.h"
#include <iostream>

void readYUVVideo(std::ifstream& fp, YUVVideo& yuvVideo, uint64_t size)
{
    int y_size = yuvVideo.width * yuvVideo.height;
    int u_size = (yuvVideo.width / 2) * (yuvVideo.height / 2);
    int v_size = u_size;
    int frame_size = y_size + u_size + v_size;

    yuvVideo.frameAmount = size / frame_size;
    yuvVideo.frames.resize(yuvVideo.frameAmount);

    for (int i = 0; i < yuvVideo.frameAmount; i++)
    {
        yuvVideo.frames[i].yPlane.resize(y_size);
        yuvVideo.frames[i].uPlane.resize(u_size);
        yuvVideo.frames[i].vPlane.resize(v_size);

        fp.read(reinterpret_cast<char*>(yuvVideo.frames[i].yPlane.data()), y_size);
        fp.read(reinterpret_cast<char*>(yuvVideo.frames[i].uPlane.data()), u_size);
        fp.read(reinterpret_cast<char*>(yuvVideo.frames[i].vPlane.data()), v_size);
    }
}

bool prepareYUVVideo(std::string fileName, YUVVideo& inputVideo, int videoWidth, int videoHeight)
{
    std::ifstream inputYUVFile(fileName, std::ios::binary);
    if (!inputYUVFile) {
        std::cerr << "Ошибка чтения файла." << std::endl;
        return false;
    }

    inputVideo.width = videoWidth;
    inputVideo.height = videoHeight;

    inputYUVFile.seekg(0, std::ios::end);
    uint64_t yuvSize = inputYUVFile.tellg();
    inputYUVFile.seekg(0, std::ios::beg);

    readYUVVideo(inputYUVFile, inputVideo, yuvSize);

    return true;
}

bool saveYUVVideo(const std::string& outputFilename, YUVVideo& yuv)
{
    std::ofstream out(outputFilename, std::ios::binary);
    if (!out)
        return false;

    for (int i = 0; i < yuv.frameAmount; i++)
    {
        out.write(reinterpret_cast<const char*>(yuv.frames[i].yPlane.data()), yuv.frames[i].yPlane.size());
        out.write(reinterpret_cast<const char*>(yuv.frames[i].uPlane.data()), yuv.frames[i].uPlane.size());
        out.write(reinterpret_cast<const char*>(yuv.frames[i].vPlane.data()), yuv.frames[i].vPlane.size());
    }

    return true;
}