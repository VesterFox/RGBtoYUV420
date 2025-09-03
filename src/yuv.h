#pragma once

#include <cstdint>
#include <vector>
#include <fstream>

class YUVRingBuffer;

class YUVFrame
{
public:
    YUVFrame(int width, int height): width_(width), height_(height)
    {
        yPlane.resize(width * height);
        uPlane.resize((width / 2) * (height / 2));
        vPlane.resize((width / 2) * (height / 2));
    }

    std::vector<uint8_t> yPlane;
    std::vector<uint8_t> uPlane;
    std::vector<uint8_t> vPlane;

    int getWidth() const { return width_; }

    int getHeight() const { return height_; }

    /// <summary>
    /// Чтение YUV кадра из ifstream и запись в структуру YUVFrame.
    /// </summary>
    /// <param name="fp"> Поток с информацией из .yuv видео файла.</param>
    /// <param name="YSize"> Размер вектора хранящего Y плоскость. </param>
    /// <param name="UVSize"> Размер векторов хранящих UV плоскости. </param>
    /// <returns> 1 кадр <see cref="YUVFrame"> из входного видео. </returns>
    void readFromFile(std::ifstream& fp, int YSize, int UVSize);

    /// <summary>
    /// Наложить YUV420 изображение поверх YUV420 видео с сдвигом.
    /// </summary>
    /// <param name="frames"> Кольцевой буфер содержащий кадры, поверх которых будет накладываться изображение. </param>
    /// <param name="xOffset"> Сдвиг по горизонтали в px.</param>
    /// <param name="yOffset"> Сдвиг по вертикали в px. </param>
    void overlayOnVideo(YUVRingBuffer& frames, int xOffset, int yOffset);

    /// <summary>
    /// Наложить YUV420 изображение поверх YUV420 видео с сдвигом.
    /// </summary>
    /// <param name="frames"> Кольцевой буфер содержащий кадры, поверх которых будет накладываться изображение. </param>
    void overlayOnVideo(YUVRingBuffer& frames);

    /// <summary>
    /// Наложение этого YUV4:2:0 изображения на другое.
    /// </summary>
    /// <param name="dstFrame"> Кадр на который будет накладываться изображение.</param>
    /// <param name="xOffset"> Отступ наложения по горизонтали в px. </param>
    /// <param name="yOffset"> Отступ наложения по вертикали в px. </param>
    void overlayOnFrame(YUVFrame& dstFrame, int xOffset, int yOffset);

    /// <summary>
    /// Проверяет, находится ли это изображение за пределами <see cref="YUVRingBuffer"/>, учитывая offset.
    /// </summary>
    /// <param name="video"> Видео, границы которого учитываются при проверке. </param>
    /// <param name="xOffset"> Отступ наложения по горизонтали в px. </param>
    /// <param name="yOffset"> Отступ наложения по вертикали в px. </param>
    /// <returns> True, если данное изображение полностью за пределами видео. </returns>
    bool isBeyondBorders(const YUVRingBuffer& video, int xOffset, int yOffset);

private:
    int width_, height_;
};

class YUVRingBuffer
{
public:
    YUVRingBuffer(int capacity, int width, int height)
        : capacity_(capacity), width_(width), height_(height), buffer_(), writeIndex_(0), size_(0)
    {
        buffer_.reserve(capacity);
        for (int i = 0; i < capacity; i++)
        {
            buffer_.emplace_back(width, height);
        }
    }

    /// <summary>
    /// Получение ссылки на свободный кадр для записи.
    /// </summary>
    /// <returns> Ссылка на <see cref="YUVFrame"> доступный для записи. </returns>
    YUVFrame& nextFrame()
    {
        YUVFrame& frame = buffer_[writeIndex_];
        writeIndex_ = (writeIndex_ + 1) % capacity_;
        if (size_ < capacity_)
            size_++;
        return frame;
    }

    /// <summary>
    /// Получение кадра по индексу.
    /// </summary>
    /// <param name="idx"> Индекс. </param>
    /// <returns> ССылка на кадр. </returns>
    YUVFrame& getFrame(int idx)
    {
        if (idx >= size_) 
            throw std::out_of_range("Индекс кадра в кольцевом буффере вышел за его границы.");
        int realIndex = (writeIndex_ + capacity_ - size_ + idx) % capacity_;
        return buffer_[realIndex];
    }

    int getWidth() const { return width_; }

    int getHeight() const { return height_; }

    /// <summary>
    /// Возвращает объем буффера в кадрах.
    /// </summary>
    /// <returns> Объем буффера в кадрах </returns>
    int size() const { return size_; }

    /// <summary>
    /// Возвращает текущее кол-во доступных для записи кадров.
    /// </summary>
    /// <returns> Текущее кол-во доступных для записи кадров. </returns>
    int capacity() const { return capacity_; }

    /// <summary>
    /// Сохранение набора кадров в .yuv формате (дописывает в конец файла).
    /// </summary>
    /// <param name="outStream"> Поток для записи файла с результатом работы. </param>
    void saveToFile(std::ofstream& outStream);

private:
    int capacity_;
    int width_, height_;
    std::vector<YUVFrame> buffer_;
    int writeIndex_;
    int size_;
};