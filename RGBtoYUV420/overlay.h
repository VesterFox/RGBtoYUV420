#pragma once
#include "yuv.h"
#include <iostream>

/// <summary>
/// Наложение одного YUV4:2:0 кадра на другой.
/// </summary>
/// <param name="dstFrame"> Кадр на который будет накладываться изображение.</param>
/// <param name="overlayFrame"> Изображение, которое будет наложено поверх кадра. </param>
/// <param name="dstWidth"> Ширина кадра. </param>
/// <param name="dstHeight"> Высота кадра. </param>
/// <param name="offsetX"> Отступ наложения по горизонтали в px. </param>
/// <param name="offsetYpos"> Отступ наложения по вертикали в px. </param>
/// <param name="overlayWidth"> Ширина накладываемого изображения. </param>
/// <param name="overlayHeight"> Высота накладываемого изображения. </param>
/// <returns> True, в случае успеха, иначе False. </returns>
bool overlayFrame(YUVFrame& dstFrame, const YUVFrame& overlayFrame,
    int dstWidth, int dstHeight,
    int offsetX, int offsetYpos,
    int overlayWidth, int overlayHeight);

/// <summary>
/// Наложить YUV420 изображение поверх YUV420 видео с свдигом.
/// </summary>
/// <param name="image"> Изображение, которое будет накладываться на видео. </param>
/// <param name="video"> Видео, поверх которого будет накладываться изображение. </param>
/// <param name="imageWidth"> Ширина изображения в px. </param>
/// <param name="imageHeight"> Высота изображения в px. </param>
/// <param name="xOffset"> Сдвиг по горизонтали в px.</param>
/// <param name="yOffset"> Сдвиг по вертикали в px. </param>
void overlayOnVideo(const YUVFrame& image, YUVVideo& video, int imageWidth, int imageHeight, int xOffset, int yOffset);

/// <summary>
/// Наложить YUV420 изображение поверх YUV420 видео по центру кадра.
/// </summary>
/// <param name="image"> Изображение, которое будет накладываться на видео. </param>
/// <param name="video"> Видео, поверх которого будет накладываться изображение. </param>
/// <param name="imageWidth"> Ширина изображения в px. </param>
/// <param name="imageHeight"> Высота изображения в px. </param>
void overlayOnVideo(const YUVFrame& image, YUVVideo& video, int imageWidth, int imageHeight);