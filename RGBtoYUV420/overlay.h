#pragma once
#include "yuv.h"

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
void overlayFrame(YUVFrame& dstFrame, const YUVFrame& overlayFrame,
    int dstWidth, int dstHeight,
    int offsetX, int offsetYpos,
    int overlayWidth, int overlayHeight);

/// <summary>
/// Наложить YUV420 изображение поверх YUV420 видео.
/// </summary>
/// <param name="image"> Изображение. </param>
/// <param name="video"> Видео поверх которого будет накладываться изображение. </param>
/// <param name="imageWidth"> Ширина изображения. </param>
/// <param name="imageHeight"> Высота изображения. </param>
void overlayOnVideo(const YUVFrame& image, YUVVideo& video, int imageWidth, int imageHeight);