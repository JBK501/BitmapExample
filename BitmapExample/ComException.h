#pragma once
#include <exception>
#include <sstream>

// COM 예외 헬퍼클래스
class ComException : public std::exception
{
	HRESULT mResult;

public:
	ComException(HRESULT hr) : mResult(hr) {}

	virtual const char* what() const override
	{
		static char buf[512]{};

		sprintf_s(buf, 512, "Failure with HRESULT : %X", mResult);
		return buf;
	}
};


//// COM 실패 헬퍼 유틸리티
//inline void ThrowIfFailed(HRESULT hr)
//{
//	if (FAILED(hr))
//	{
//		throw ComException(hr);
//	}
//}

