#pragma once

class IVDebugOverlay
{
public:
	virtual void AddEntityTextOverlay(int entityIndex, int lineOffset, float duration, int r, int g, int b, int a, const char* fmt, ...) = 0;
	virtual void AddBoxOverlay(const Vector& origin, const Vector& min, const Vector& max, const Vector& orientation, int r, int g, int b, int a, float duration) = 0;
	virtual void AddSphereOverlay(const Vector& origin, float flRadius, int nTheta, int nPhi, int r, int g, int b, int a, float duration) = 0;
	virtual void AddTriangleOverlay(const Vector& p1, const Vector& p2, const Vector& p3, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void AddLineOverlay(const Vector& origin, const Vector& dest, int r, int g, int b, bool noDepthTest, float duration) = 0;
	virtual void AddTextOverlay(const Vector& origin, float duration, const char* fmt, ...) = 0;
	virtual void AddTextOverlay(const Vector& origin, int lineOffset, float duration, const char* fmt, ...) = 0;
	virtual void AddScreenTextOverlay(float x, float y, float duration, int r, int g, int b, int a, const char* text) = 0;
	virtual void AddSweptBoxOverlay(const Vector& start, const Vector& end, const Vector& min, const Vector& max, const Vector& angles, int r, int g, int b, int a, float duration) = 0;
	virtual void AddGridOverlay(const Vector& origin) = 0;
	virtual void AddCoordFrameOverlay(const matrix3x4_t& frame, float scale, int colorTable[3][3] = nullptr) = 0;
	virtual int ScreenPosition(const Vector& worldPosition, Vector& screen) = 0;
	virtual int ScreenPosition(float flXPos, float flYPos, Vector& vecScreen) = 0;
	virtual void* GetFirst() = 0;
	virtual void* GetNext(void* pCurrentRecord) = 0;
	virtual void ClearDeadOverlays() = 0;
	virtual void ClearAllOverlays() = 0;
	virtual void AddTextOverlayRGB(const Vector& vecOrigin, int iLineOffset, float flDuration, float r, float g, float b, float a, const char* fmt, ...) = 0;
	virtual void AddTextOverlayRGB(const Vector& vecOrigin, int iLineOffset, float flDuration, int r, int g, int b, int a, const char* fmt, ...) = 0;
	virtual void AddLineOverlayAlpha(const Vector& vecOrigin, const Vector& dest, int r, int g, int b, int a, bool bNoDepthTest, float flDuration) = 0;
	virtual void AddBoxOverlay2(const Vector& vecOrigin, const Vector& vecAbsMin, const Vector& vecAbsMax, const Vector& angOrientation, const Color& faceColor, const Color& edgeColor, float flDuration) = 0;
	virtual void AddLineOverlay(const Vector& vecOrigin, const Vector& vecDest, int r, int g, int b, int a, float flThickness, float flDuration) = 0;
	virtual void PurgeTextOverlays() = 0;
	virtual void AddCapsuleOverlay(const Vector& vecAbsMin, const Vector& vecAbsMax, const float& flRadius, int r, int g, int b, int a, float flDuration) = 0;
	virtual void DrawPill(Vector& vecAbsMin, Vector& vecAbsMax, float flRadius, int r, int g, int b, int a, float flDuration) = 0;

	void AddCapsuleOverlay(Vector& vecAbsMin, Vector& vecAbsMax, float flRadius, Color arrColor, float flDuration)
	{
		using fnOriginal = void(__thiscall*)(void*, Vector&, Vector&, float&, int, int, int, int, float);
		util::GetVFunc<fnOriginal>(this, 24)(this, vecAbsMin, vecAbsMax, flRadius, arrColor[0], arrColor[1], arrColor[2], arrColor[3], flDuration);
	};
};