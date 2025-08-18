#pragma once

#include <vector>
#include <fstream>

struct YUVFrame
{
    std::vector<uint8_t> yPlane;
    std::vector<uint8_t> uPlane;
    std::vector<uint8_t> vPlane;
};

struct YUVVideo
{
    int width = 0;
    int height = 0;
    uint32_t frameAmount = 0;
    std::vector<YUVFrame> frames;
};

/// <summary>
/// Чтение видео из ifstream и запись в структуру YUVVideo.
/// </summary>
/// <param name="fp"> Поток с информацией из .yuv видео файла.</param>
/// <param name="yuvVideo"> Ссылка на <see cref="YUVVideo">. </param>
/// <param name="size"> Размер входных данных в ifstream. </param>
void readYUVVideo(std::ifstream& fp, YUVVideo& yuvVideo, uint64_t size);

/// <summary>
/// Сохранение видео в .yuv формате.
/// </summary>
/// <param name="outputFilename"> Имя выходного файла (включая расширение).</param>
/// <param name="yuv"> <see cref=YUVVideo> структура видео в формате YUV</param>
/// <returns> True, если операция успешная, иначе False. </returns>
bool saveYUVVideo(const std::string& outputFilename, YUVVideo& yuv);

/// <summary>
/// Чтение входного видео и сбор необходимой информации о нём
/// </summary>
/// <param name="fileName"> Имя (путь) до файла, включая расширение. </param>
/// <param name="inputVideo"> <see cref="YUVVideo"> структура видео YUV. Вывод результата чтения файла. </param>
/// <param name="videoWidth"> Ширина разрешения видео.</param>
/// <param name="videoHeight"> Высота разрешения видео. </param>
/// <returns> True, если операция успешна, иначе False. Выводит сообщения о ошибке и ходе операции. </returns>
bool prepareYUVVideo(std::string fileName, YUVVideo& inputVideo, int videoWidth, int videoHeight);