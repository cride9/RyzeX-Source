#pragma once
#include <format>
#include "../../SDK/DataTyes/Color.h"
#include <codecvt>

enum EFontDrawType : int
{
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2
};

enum EFontFlags
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};

struct Vertex_t
{
	Vertex_t() = default;

	Vertex_t(const Vector2D& vecPosition, const Vector2D& vecCoordinate = Vector2D(0, 0)) {

		this->vecPosition = vecPosition;
		this->vecCoordinate = vecCoordinate;
	}

	void Init(const Vector2D& vecPosition, const Vector2D& vecCoordinate = Vector2D(0, 0)) {

		this->vecPosition = vecPosition;
		this->vecCoordinate = vecCoordinate;
	}

	Vector2D vecPosition = { };
	Vector2D vecCoordinate = { };
};

typedef unsigned long HScheme, HPanel, HTexture, HCursor, HFont;
class ISurface {

public:

	void DrawT(int X, int Y, Color Color, HFont Font, bool Center, const char* _Input, ...) noexcept {

		int w, h;
		static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		static std::wstring text;
		try {
			text = converter.from_bytes(_Input);
		}
		catch (std::range_error){
			//util::LogConsole(std::format("Name: {} cannot be converted (not utf-8)\n", _Input).c_str());
			return;
		}

		GetTextSize(Font, text.c_str(), w, h);
		DrawSetTextFont(Font);
		DrawSetTextColor(Color);
		DrawSetTextPos(Center ? X - w / 2 : X, Y);
		DrawPrintText(text.c_str(), text.size());
	}

	void DrawT(int X, int Y, Color Color, HFont Font, bool Center, wchar_t* _Input, ...) noexcept {

		int w, h;

		GetTextSize(Font, _Input, w, h);
		DrawSetTextFont(Font);
		DrawSetTextColor(Color);
		DrawSetTextPos(Center ? X - w / 2 : X, Y);
		DrawPrintText(_Input, lstrlenW(_Input));
	}

	void DrawSetColor(Color colDraw)
	{
		util::CallVFunc<void>(this, 14U, colDraw);
	}

	void DrawSetColor(int r, int g, int b, int a)
	{
		util::CallVFunc<void>(this, 15U, r, g, b, a);
	}

	void DrawFilledRect(int x0, int y0, int x1, int y1)
	{
		util::CallVFunc<void>(this, 16U, x0, y0, x1, y1);
	}

	void DrawOutlinedRect(int x0, int y0, int x1, int y1)
	{
		util::CallVFunc<void>(this, 18U, x0, y0, x1, y1);
	}

	void DrawLine(int x0, int y0, int x1, int y1)
	{
		util::CallVFunc<void>(this, 19U, x0, y0, x1, y1);
	}

	void DrawPolyLine(int* x, int* y, int nPoints)
	{
		util::CallVFunc<void>(this, 20U, x, y, nPoints);
	}

	void DrawSetTextFont(HFont hFont)
	{
		util::CallVFunc<void>(this, 23U, hFont);
	}

	void DrawSetTextColor(Color color)
	{
		util::CallVFunc<void>(this, 24U, color);
	}

	void DrawSetTextColor(int r, int g, int b, int a)
	{
		util::CallVFunc<void>(this, 25U, r, g, b, a);
	}

	void DrawSetTextPos(int x, int y)
	{
		util::CallVFunc<void>(this, 26U, x, y);
	}

	void DrawPrintText(const wchar_t* wszText, int nTextLength, EFontDrawType drawType = FONT_DRAW_DEFAULT)
	{
		util::CallVFunc<void>(this, 28U, wszText, nTextLength, drawType);
	}

	void DrawSetTextureRGBA(int nIndex, const unsigned char* arrRGBA, int iWide, int iTall)
	{
		util::CallVFunc<void>(this, 37U, nIndex, arrRGBA, iWide, iTall);
	}

	void DrawSetTexture(int nIndex)
	{
		util::CallVFunc<void>(this, 38U, nIndex);
	}

	int CreateNewTextureID(bool bProcedural = false)
	{
		return util::CallVFunc<int>(this, 43U, bProcedural);
	}

	void UnLockCursor()
	{
		util::CallVFunc<void>(this, 66U);
	}

	void LockCursor()
	{
		util::CallVFunc<void>(this, 67U);
	}

	HFont CreateFontGame()
	{
		return util::CallVFunc<HFont>(this, 71U);
	}

	bool SetFontGlyphSet(HFont hFont, const char* szWindowsFontName, int iTall, int iWeight, int iBlur, int nScanLines, int iFlags, int nRangeMin = 0, int nRangeMax = 0)
	{
		return util::CallVFunc<bool>(this, 72U, hFont, szWindowsFontName, iTall, iWeight, iBlur, nScanLines, iFlags, nRangeMin, nRangeMax);
	}

	void GetTextSize(HFont hFont, const wchar_t* wszText, int& iWide, int& iTall)
	{
		util::CallVFunc<void>(this, 79U, hFont, wszText, &iWide, &iTall);
	}

	void PlaySoundSurface(const char* szFileName)
	{
		util::CallVFunc<void>(this, 82U, szFileName);
	}

	void DrawOutlinedCircle(int x, int y, int iRadius, int nSegments)
	{
		util::CallVFunc<void>(this, 103U, x, y, iRadius, nSegments);
	}

	void DrawTexturedPolygon(int nCount, Vertex_t* pVertices, bool bClipVertices = true)
	{
		util::CallVFunc<void>(this, 106U, nCount, pVertices, bClipVertices);
	}

	void DrawFilledRectFade(int x0, int y0, int x1, int y1, std::uint32_t uAlpha0, std::uint32_t uAlpha1, bool bHorizontal)
	{
		util::CallVFunc<void>(this, 123U, x0, y0, x1, y1, uAlpha0, uAlpha1, bHorizontal);
	}

	void GetClipRect(int& x0, int& y0, int& x1, int& y1)
	{
		util::CallVFunc<void>(this, 146U, &x0, &y0, &x1, &y1);
	}

	void SetClipRect(int x0, int y0, int x1, int y1)
	{
		util::CallVFunc<void>(this, 147U, x0, y0, x1, y1);
	}
};