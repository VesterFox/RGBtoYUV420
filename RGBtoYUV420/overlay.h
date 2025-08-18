#pragma once
#include "yuv.h"

/// <summary>
/// ��������� ������ YUV4:2:0 ����� �� ������.
/// </summary>
/// <param name="dstFrame"> ���� �� ������� ����� ������������� �����������.</param>
/// <param name="overlayFrame"> �����������, ������� ����� �������� ������ �����. </param>
/// <param name="dstWidth"> ������ �����. </param>
/// <param name="dstHeight"> ������ �����. </param>
/// <param name="offsetX"> ������ ��������� �� ����������� � px. </param>
/// <param name="offsetYpos"> ������ ��������� �� ��������� � px. </param>
/// <param name="overlayWidth"> ������ �������������� �����������. </param>
/// <param name="overlayHeight"> ������ �������������� �����������. </param>
void overlayFrame(YUVFrame& dstFrame, const YUVFrame& overlayFrame,
    int dstWidth, int dstHeight,
    int offsetX, int offsetYpos,
    int overlayWidth, int overlayHeight);

/// <summary>
/// �������� YUV420 ����������� ������ YUV420 �����.
/// </summary>
/// <param name="image"> �����������. </param>
/// <param name="video"> ����� ������ �������� ����� ������������� �����������. </param>
/// <param name="imageWidth"> ������ �����������. </param>
/// <param name="imageHeight"> ������ �����������. </param>
void overlayOnVideo(const YUVFrame& image, YUVVideo& video, int imageWidth, int imageHeight);