#pragma once

#include <memory>
#include "D2DFramework.h"

const int BITMAP_WIDTH{ 1024 };
const int BITMAP_HEIGHT{ 768 };
const int BITMAP_BYTECOUNT{ 4 };

class BitmapExample :  public D2DFramework
{
private:
	Microsoft::WRL::ComPtr<ID2D1Bitmap> mspFramebitmap{};
	std::unique_ptr<UINT8[]> mspBackBuffer;	

public:
	virtual void Initialize(HINSTANCE hInstance,
		LPCWSTR title = L"Direct2D Example",
		UINT width = 1024,
		UINT height = 768) override;
	void Render() override;

	void ClearBuffer(D2D1::ColorF color);	// 배경색 지운다.
	void PresentBuffer();	// 백버퍼의 내용을 프레임버퍼로 복사한다.
	void DrawPixelToBuffer(int x, int y, D2D1::ColorF color); // 화면에 픽셀을 그린다.
	void FillRectToBuffer(
		int left, int top, int width, int height, D2D1::ColorF color
	);	// 사각형을 그린다
	void DrawCircleToBuffer(
		int x, int y, int radius, D2D1::ColorF color
	); // 원을 그린다.
	void DrawLineToBuffer(
		int x1, int y1, int x2, int y2, D2D1::ColorF color
	);	// 두 점을 잇는 직선을 그린다.
};

