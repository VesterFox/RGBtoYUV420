#pragma once

#include <vector>
#include <fstream>

struct YUVFrame
{
    std::vector<uint8_t> yPlane;
    std::vector<uint8_t> uPlane;
    std::vector<uint8_t> vPlane;

    YUVFrame() = default;

    YUVFrame(int width, int height)
    {
        yPlane.resize(width * height);
        uPlane.resize((width / 2) * (height / 2));
        vPlane.resize((width / 2) * (height / 2));
    }
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

private:
    int capacity_;
    int width_, height_;
    std::vector<YUVFrame> buffer_;
    int writeIndex_;
    int size_;
};

/// <summary>
/// Чтение YUV кадра из ifstream и запись в структуру YUVFrame.
/// </summary>
/// <param name="fp"> Поток с информацией из .yuv видео файла.</param>
/// <param name="YSize"> Размер вектора хранящего Y плоскость. </param>
/// <param name="UVSize"> Размер векторов хранящих UV плоскости. </param>
/// <returns> 1 кадр <see cref="YUVFrame"> из входного видео. </returns>
YUVFrame readYUVFrame(std::ifstream& fp, int YSize, int UVSize); 

/// <summary>
/// Сохранение набора кадров в .yuv формате (дописывает в конец файла).
/// </summary>
/// <param name="outputFilename"> Имя выходного файла (включая расширение).</param>
/// <param name="yuv"> Циклический буфер содержащий кадры для записи. </param>
/// <returns> True, если операция успешная, иначе False. </returns>
bool saveYUVFrames(const std::string& outputFilename, YUVRingBuffer& yuv);