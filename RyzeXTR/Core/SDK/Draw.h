//#pragma once
//// used: std::function
//#include <functional>
//// used: std::deque
//#include <deque>
//// used: std::unique_lock
//#include <mutex>
//// used: std::shared_mutex
//#include <shared_mutex>
//// used: std::any
//#include <any>
//// used: color
//#include "DataTyes/color.h"
//// used: vector
//#include "DataTyes/vector.h"
//
//#define D3DX_PI    ((FLOAT)  3.141592654f)
//#define D3DX_1BYPI ((FLOAT)  0.318309886f)
//
//#define D3DXToRadian( degree ) ((degree) * (D3DX_PI / 180.0f))
//#define D3DXToDegree( radian ) ((radian) * (180.0f / D3DX_PI))
//
//
//// types of thread-safe render objects
//enum class EDrawType : int
//{
//	NONE = 0,
//	LINE,
//	RECT,
//	RECT_MULTICOLOR,
//	CIRCLE,
//	RING,
//	TRIANGLE,
//	POLYGON,
//	TEXT,
//	IMAGE,
//	ARC
//};
//
//struct DrawObject_t
//{
//	DrawObject_t( const EDrawType nType, std::any&& pObject ) :
//		nType( nType ), pObject( std::move( pObject ) ) { }
//
//	EDrawType nType = EDrawType::NONE;
//	std::any pObject = { };
//};
//
//#pragma region draw_objects_enumerations
//enum ERectRenderFlags : unsigned int
//{
//	DRAW_RECT_NONE = 0,
//	DRAW_RECT_OUTLINE = ( 1 << 0 ),
//	DRAW_RECT_BORDER = ( 1 << 1 ),
//	DRAW_RECT_FILLED = ( 1 << 2 ),
//	DRAW_RECT_ALIGNED = ( 1 << 3 ),
//};
//
//enum ECircleRenderFlags : unsigned int
//{
//	DRAW_CIRCLE_NONE = 0,
//	DRAW_CIRCLE_OUTLINE = ( 1 << 0 ),
//	DRAW_CIRCLE_FILLED = ( 1 << 1 )
//};
//
//enum ERingRenderFlags : unsigned int
//{
//	DRAW_RING_NONE = 0,
//	DRAW_RING_COLLISIONS = ( 1 << 1 )
//};
//
//enum ETriangleRenderFlags : unsigned int
//{
//	DRAW_TRIANGLE_NONE = 0,
//	DRAW_TRIANGLE_OUTLINE = ( 1 << 0 ),
//	DRAW_TRIANGLE_FILLED = ( 1 << 1 )
//};
//
//enum EPolygonRenderFlags : unsigned int
//{
//	DRAW_POLYGON_NONE = 0,
//	DRAW_POLYGON_OUTLINE = ( 1 << 0 ),
//	DRAW_POLYGON_FILLED = ( 1 << 1 )
//};
//
//enum ETextRenderFlags : unsigned int
//{
//	DRAW_TEXT_NONE = 0,
//	DRAW_TEXT_DROPSHADOW = ( 1 << 0 ),
//	DRAW_TEXT_OUTLINE = ( 1 << 1 )
//};
//#pragma endregion
//
//#pragma region draw_objects_structures
//struct LineObject_t
//{
//	ImVec2 vecStart = { };
//	ImVec2 vecEnd = { };
//	ImU32 colLine = 0x0;
//	float flThickness = 0.f;
//};
//
//struct RectObject_t
//{
//	ImVec2 vecMin = { };
//	ImVec2 vecMax = { };
//	ImU32 colRect = 0x0;
//	unsigned int uFlags = 0x0;
//	ImU32 colOutline = 0x0;
//	float flRounding = 0.f;
//	ImDrawCornerFlags roundingCorners = ImDrawCornerFlags_None;
//	float flThickness = 0.f;
//};
//
//struct RectMultiColorObject_t
//{
//	ImVec2 vecMin = { };
//	ImVec2 vecMax = { };
//	ImU32 colUpperLeft = 0x0;
//	ImU32 colUpperRight = 0x0;
//	ImU32 colBottomLeft = 0x0;
//	ImU32 colBottomRight = 0x0;
//	unsigned int uFlags = 0x0;
//	ImU32 colOutline = 0x0;
//};
//
//struct CircleObject_t
//{
//	ImVec2 vecCenter = { };
//	float flRadius = 0.f;
//	ImU32 colCircle = 0x0;
//	int nSegments = 0;
//	unsigned int uFlags = 0x0;
//	ImU32 colOutline = 0x0;
//	float flThickness = 0.f;
//};
//
//struct RingObject_t
//{
//	Vector vecCenter = { };
//	float flRadius = 0.f;
//	ImU32 colCircle = 0x0;
//	int nSegments = 0;
//	unsigned int uFlags = 0x0;
//	float flThickness = 0.f;
//};
//
//struct TriangleObject_t
//{
//	ImVec2 vecFirst = { };
//	ImVec2 vecSecond = { };
//	ImVec2 vecThird = { };
//	ImU32 colTriangle = 0x0;
//	unsigned int uFlags = 0x0;
//	ImU32 colOutline = 0x0;
//	float flThickness = 0.f;
//};
//
//struct PolygonObject_t
//{
//	std::vector<ImVec2> vecPoints = { };
//	ImU32 colPolygon = 0x0;
//	unsigned int uFlags = 0x0;
//	ImU32 colOutline = 0x0;
//	bool bClosed = false;
//	float flThickness = 0.f;
//};
//
//struct TextObject_t
//{
//	const ImFont* pFont = nullptr;
//	float flFontSize = 0.f;
//	ImVec2 vecPosition = { };
//	std::string szText = { };
//	ImU32 colText = 0x0;
//	unsigned int uFlags = 0x0;
//	ImU32 colOutline = 0x0;
//};
//
//struct ImageObject_t
//{
//	ImTextureID pTexture = nullptr;
//	ImVec2 vecMin = { };
//	ImVec2 vecMax = { };
//	ImU32 colImage = 0x0;
//	float flRounding = 0.f;
//	ImDrawCornerFlags roundingCorners = ImDrawCornerFlags_None;
//};
//
//struct ArcObject_t
//{
//	ImVec2 vecPosition = { };
//	float flRadius = 0.f;
//	float flMinAngle = 0.f;
//	float flMaxAngle = 0.f;
//	ImU32 colFill = 0x0;
//	float flThickness = 0.f;
//};
//
//namespace Fonts
//{
//	inline ImFont* Verdana = nullptr;
//}
//
//namespace D
//{
//	void SetupFonts( );
//	/* render primitives by stored & safe data */
//	void RenderDrawData( ImDrawList* pDrawList );
//	/* clear native data to draw */
//	void ClearDrawData( );
//	/* swap native draw data to safe */
//	void SwapDrawData( );
//	/* draw a circle like onetap */
//	void Render3DCircleOneteap( Vector pos, Color color, int point_count, float radius, bool fade = false, float rot_start = 0.f, float fade_start = 0.5f, float fade_length = 0.25f );
//	/* draw a circle as a dotted 3D object */
//	void Draw3dDottedRing( Vector pos, int16_t radius, uint16_t points, Color color1, float thickness );
//	/* draw a circle as a 3D object */
//	void Draw3dRing( Vector pos, int16_t radius, uint16_t points, Color color1, float thickness );
//	void Draw3DCircleFilled( Vector m_vecPosition, Color m_col, int m_iPointCount, float m_flRadius, float m_flThickness = 1.f );
//
//	// Render
//	void AddLine( const ImVec2& vecStart, const ImVec2& vecEnd, const Color& colLine, float flThickness = 1.0f );
//	void AddRect( const ImVec2& vecMin, const ImVec2& vecMax, const Color& colRect, unsigned int uFlags = DRAW_RECT_NONE, const Color& colOutline = Color( 0, 0, 0, 255 ), float flRounding = 0.f, ImDrawCornerFlags roundingCorners = ImDrawCornerFlags_All, float flThickness = 1.0f );
//	void AddRectMultiColor( const ImVec2& vecMin, const ImVec2& vecMax, const Color& colUpperLeft, const Color& colUpperRight, const Color& colBottomLeft, const Color& colBottomRight, unsigned int uFlags = DRAW_RECT_NONE, const Color& colOutline = Color( 0, 0, 0, 255 ) );
//	void AddCircle( const ImVec2& vecCenter, float flRadius, const Color& colCircle, int nSegments = 12, unsigned int uFlags = DRAW_CIRCLE_NONE, const Color& colOutline = Color( 0, 0, 0, 255 ), float flThickness = 1.0f );
//	void AddRing( const Vector& vecCenter, float flRadius, const Color& colCircle, int nSegments = 12, unsigned int uFlags = DRAW_RING_NONE, float flThickness = 1.0f );
//	void AddTriangle( const ImVec2& vecFirst, const ImVec2& vecSecond, const ImVec2& vecThird, const Color& colTriangle, unsigned int uFlags = DRAW_TRIANGLE_NONE, const Color& colOutline = Color( 0, 0, 0, 255 ), float flThickness = 1.0f );
//	void AddPolygon( std::vector<ImVec2>& vecPoints, const Color& colPolygon, unsigned int uFlags = DRAW_POLYGON_FILLED, const Color& colOutline = Color( 0, 0, 0, 255 ), bool bClosed = true, float flThickness = 1.0f );
//	void AddText( const ImFont* pFont, float flFontSize, const ImVec2& vecPosition, const std::string& szText, const Color& colText, unsigned int uFlags = DRAW_TEXT_NONE, const Color& colOutline = Color( 0, 0, 0, 255 ) );
//	void AddText( const ImVec2& vecPosition, const std::string& szText, const Color& colText, int iFlags = DRAW_TEXT_NONE, const Color& colOutline = Color( 0, 0, 0, 255 ) );
//	void AddImage( ImTextureID pTexture, const ImVec2& vecMin, const ImVec2& vecMax, const Color& colImage = Color( 255, 255, 255, 255 ), float flRounding = 0.f, ImDrawCornerFlags roundingCorners = ImDrawCornerFlags_All );
//	void AddArcFilled( const ImVec2& vecPosition, float flRadius, float flMinAngle, float flMaxAngle, const Color& colFill = Color( 255, 255, 255, 255 ), float flThickness = 1.f );
//
//	// Bindings
//	void AddDrawListRect( ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, ImU32 colRect, unsigned int uFlags = DRAW_RECT_NONE, ImU32 colOutline = IM_COL32( 0, 0, 0, 255 ), float flRounding = 0.f, ImDrawCornerFlags roundingCorners = ImDrawCornerFlags_All, float flThickness = 1.0f );
//	void AddDrawListRectMultiColor( ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, ImU32 colUpperLeft, ImU32 colUpperRight, ImU32 colBottomLeft, ImU32 colBottomRight, unsigned int uFlags = DRAW_RECT_NONE, ImU32 colOutline = IM_COL32( 0, 0, 0, 255 ) );
//	void AddDrawListText( ImDrawList* pDrawList, const ImFont* pFont, float flFontSize, const ImVec2& vecPosition, const std::string& szText, ImU32 colText, unsigned int uFlags = DRAW_TEXT_NONE, ImU32 colOutline = IM_COL32( 0, 0, 0, 255 ) );
//
//	// Extra
//	/* converts 3d game world space to screen space */
//	bool WorldToScreen( const Vector& vecOrigin, ImVec2& vecScreen );
//
//	// Values
//	/* directx init state */
//	inline bool	bInitialized = false;
//	/* saved data to draw */
//	inline std::deque<DrawObject_t> vecDrawData = { };
//	/* thread-safe data to draw */
//	inline std::deque<DrawObject_t> vecSafeDrawData = { };
//	/* thread-safe render mutex */
//	inline std::shared_mutex drawMutex = { };
//}