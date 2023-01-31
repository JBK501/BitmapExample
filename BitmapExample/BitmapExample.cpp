#include "BitmapExample.h"
#include <cmath>

void BitmapExample::Initialize(HINSTANCE hInstance, LPCWSTR title, UINT width, UINT height)
{
	// 1. 기존 프레임워크를 초기화한다.
	D2DFramework::Initialize(hInstance, title, width, height);

	// 2. 백버퍼를 만든다.
	mspBackBuffer = std::make_unique<UINT8[]>(
		BITMAP_WIDTH * BITMAP_HEIGHT * BITMAP_BYTECOUNT
	);

	// 3. 비트맵 인터페이스를 만든다.
	HRESULT hr = mspRenderTarget->CreateBitmap(
		D2D1::SizeU(BITMAP_WIDTH, BITMAP_HEIGHT),
		D2D1::BitmapProperties(
			D2D1::PixelFormat(
				DXGI_FORMAT_R8G8B8A8_UNORM,
				D2D1_ALPHA_MODE_IGNORE
			)
		),
		mspFramebitmap.GetAddressOf()
	);

	ThrowIfFailed(hr);
}

void BitmapExample::Render()
{
	mspRenderTarget->BeginDraw();
	mspRenderTarget->Clear(D2D1::ColorF(0.0f, 0.2f, 0.4f, 1.0f));

	ClearBuffer(D2D1::ColorF(D2D1::ColorF::LightPink));
	//FillRectToBuffer(0, 0, 100, 100, D2D1::ColorF::Red);
	//FillRectToBuffer(50, 50, 100, 100, D2D1::ColorF(D2D1::ColorF::Green,0.5f));
	//DrawCircleToBuffer(200, 200, 50, D2D1::ColorF::DeepSkyBlue);
	DrawLineToBuffer(100, 200, 200, 400, D2D1::ColorF::Red);
	PresentBuffer();

	mspRenderTarget->DrawBitmap(mspFramebitmap.Get());

	mspRenderTarget->EndDraw();
}

void BitmapExample::ClearBuffer(D2D1::ColorF color)
{
	// 1. 백버퍼를 가져온다.
	UINT8* pCurrent = mspBackBuffer.get();

	// 2. 너비만큼 반복한다.
	for (int count = 0; count < BITMAP_WIDTH * BITMAP_HEIGHT; ++count)
	{
		// 2.1 픽셀에 색상을 채운다.
		*pCurrent = static_cast<UINT8>(color.r * 255);
		*(pCurrent + 1) = static_cast<UINT8>(color.g * 255);
		*(pCurrent + 2) = static_cast<UINT8>(color.b * 255);
		*(pCurrent + 3) = static_cast<UINT8>(color.a * 255);

		// 2.2 픽셀크키만큼 이동한다.
		pCurrent += BITMAP_BYTECOUNT;
	}
}

void BitmapExample::PresentBuffer()
{
	mspFramebitmap->CopyFromMemory(
		nullptr,
		mspBackBuffer.get(),
		BITMAP_WIDTH * BITMAP_BYTECOUNT
	);
}

// alpha blending (두 색상을 섞다)
// 기존 색상 * (1 - a) + 그릴색상 * a
void BitmapExample::DrawPixelToBuffer(int x, int y, D2D1::ColorF color)
{
	// 1. 피치를 구한다.
	int pitch = BITMAP_WIDTH * BITMAP_BYTECOUNT;	

	// 2. 인덱스를 구한다.
	int index = x * BITMAP_BYTECOUNT + pitch * y;

	// 3. 백버퍼의 해당 인덱스에 점을 찍는다.
	float inverse = 1.0f - color.a;

	mspBackBuffer[index] = static_cast<UINT8>(
		mspBackBuffer[index] * inverse + color.r * 255 * color.a
		);
	mspBackBuffer[index + 1] = static_cast<UINT8>(
		mspBackBuffer[index + 1] * inverse + color.g * 255 * color.a
		);
	mspBackBuffer[index + 2] = static_cast<UINT8>(
		mspBackBuffer[index + 2] * inverse + color.b * 255 * color.a
		);
	mspBackBuffer[index + 3] = static_cast<UINT8>(
		color.a * 255
		);
}

void BitmapExample::FillRectToBuffer(int left, int top, int width, int height, D2D1::ColorF color)
{
	// 1. 높이 만큼 반복한다.
	for (int h = 0; h < height; h++)
	{
		// 1.1. 너비 만큼 반복한다.
		for (int w = 0; w < width; w++)
		{
			// 1.1.1. 버퍼에 저장한다.
			DrawPixelToBuffer(left + w, top + h, color);
		}
	}
}

void BitmapExample::DrawCircleToBuffer(int x, int y, int radius, D2D1::ColorF color)
{
	const float pi{ 3.14f };
	const float piMultiplyTwo{ pi * 2 };
	float theta{};

	// 1. 세타가 2파이가 될때까지 반복한다.
	while(theta < piMultiplyTwo)
	{
		// 1.1. 점을 찍는다.
		DrawPixelToBuffer(
			static_cast<int>(x + cos(theta) * radius),
			static_cast<int>(y + sin(theta) * radius),
			color
		);
		// 1.2. 각도를 늘린다.
		theta += 0.02;
	}
}

void BitmapExample::DrawLineToBuffer(int x1, int y1, int x2, int y2, D2D1::ColorF color)
{
	// 1. 변화량을 구한다.
	int dx{ x2 - x1 };
	int dy{ y2 - y1 };

	int x;
	int y;
	int op;
	// 2. x변화량이 0이면
	if (dx == 0)
	{
		x = x1;	// x고정
		y = y1;
		op = (y2 > y1) ?  1 : -1;
		while (y != y2)
		{
			DrawPixelToBuffer(x, y, color);
			y += op;
		}
		return;
	}

	// 3. y변화량이 0이면
	if (dy == 0)
	{
		x = x1;
		y = y1; // y고정
		op = (x2 > x1) ? 1 : -1;
		while (x != x2)
		{
			DrawPixelToBuffer(x, y, color);
			x += op;
		}
		return;
	}

	// 4. 기울기를 구한다.
	float m = dy / (dx * 1.0f);

	// 5. 직선을 구한다.
	op = (x1 < x2) ? 1 : -1;
	x = x1;
	while (x != x2)
	{
		y = static_cast<int>(m * (x - x1) + y1);
		DrawPixelToBuffer(x, y, color);
		x += op;
	}
}
