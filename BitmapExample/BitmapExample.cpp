#include "BitmapExample.h"

void BitmapExample::Initialize(HINSTANCE hInstance, LPCWSTR title, UINT width, UINT height)
{
	// 1. ���� �����ӿ�ũ�� �ʱ�ȭ�Ѵ�.
	D2DFramework::Initialize(hInstance, title, width, height);

	// 2. ����۸� �����.
	mspBackBuffer = std::make_unique<UINT8[]>(
		BITMAP_WIDTH * BITMAP_HEIGHT * BITMAP_BYTECOUNT
	);

	// 3. ��Ʈ�� �������̽��� �����.
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
	FillRectToBuffer(0, 0, 100, 100, D2D1::ColorF::Red);
	FillRectToBuffer(50, 50, 100, 100, D2D1::ColorF(D2D1::ColorF::Green,0.5f));
	PresentBuffer();

	mspRenderTarget->DrawBitmap(mspFramebitmap.Get());

	mspRenderTarget->EndDraw();
}

void BitmapExample::ClearBuffer(D2D1::ColorF color)
{
	// 1. ����۸� �����´�.
	UINT8* pCurrent = mspBackBuffer.get();

	// 2. �ʺ�ŭ �ݺ��Ѵ�.
	for (int count = 0; count < BITMAP_WIDTH * BITMAP_HEIGHT; ++count)
	{
		// 2.1 �ȼ��� ������ ä���.
		*pCurrent = static_cast<UINT8>(color.r * 255);
		*(pCurrent + 1) = static_cast<UINT8>(color.g * 255);
		*(pCurrent + 2) = static_cast<UINT8>(color.b * 255);
		*(pCurrent + 3) = static_cast<UINT8>(color.a * 255);

		// 2.2 �ȼ�ũŰ��ŭ �̵��Ѵ�.
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

// alpha blending (�� ������ ����)
// ���� ���� * (1 - a) + �׸����� * a
void BitmapExample::DrawPixelToBuffer(int x, int y, D2D1::ColorF color)
{
	// 1. ��ġ�� ���Ѵ�.
	int pitch = BITMAP_WIDTH * BITMAP_BYTECOUNT;	

	// 2. �ε����� ���Ѵ�.
	int index = x * BITMAP_BYTECOUNT + pitch * y;

	// 3. ������� �ش� �ε����� ���� ��´�.
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
	// 1. ���� ��ŭ �ݺ��Ѵ�.
	for (int h = 0; h < height; h++)
	{
		// 1.1. �ʺ� ��ŭ �ݺ��Ѵ�.
		for (int w = 0; w < width; w++)
		{
			// 1.1.1. ���ۿ� �����Ѵ�.
			DrawPixelToBuffer(left + w, top + h, color);
		}
	}
}
