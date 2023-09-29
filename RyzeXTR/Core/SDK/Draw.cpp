//#include "Draw.h"
//#include "../Interface/interfaces.h"
//#include "math.h"
//#include "../globals.h"
//#include "../../Dependecies/ImGui/imgui_internal.h"
//#include "../../Dependecies/ImGui/imgui_freetype.h"
//
//void D::SetupFonts( )
//{
//    ImGuiIO& io = ImGui::GetIO( );
//    ImFontConfig imVerdanaConfig = {};
//    imVerdanaConfig.RasterizerMultiply = ImGuiFreeType::MonoHinting;
//
//    Fonts::Verdana = io.Fonts->AddFontFromFileTTF( XorStr( "C:\\Windows\\Fonts\\Verdana.ttf" ), 50.0f, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic( ) );
//}
//
//void D::RenderDrawData( ImDrawList* pDrawList )
//{
//    std::unique_lock<std::shared_mutex> lock( drawMutex );
//
//    // prevent render in main menu
//    if ( !i::EngineClient->IsInGame( ) )
//        return;
//
//    if ( vecSafeDrawData.empty( ) )
//        return;
//
//    for ( const auto& data : vecSafeDrawData )
//    {
//        if ( !data.pObject.has_value( ) )
//            continue;
//
//        switch ( data.nType )
//        {
//        case EDrawType::LINE:
//        {
//            const auto& pObject = std::any_cast< LineObject_t >( data.pObject );
//            pDrawList->AddLine( pObject.vecStart, pObject.vecEnd, pObject.colLine, pObject.flThickness );
//            break;
//        }
//        case EDrawType::RECT:
//        {
//            const auto& pObject = std::any_cast< RectObject_t >( data.pObject );
//
//            if ( pObject.uFlags & DRAW_RECT_ALIGNED )
//                AddDrawListRect( pDrawList,
//                    { floor( pObject.vecMin.x ), floor( pObject.vecMin.y ) },
//                    { floor( pObject.vecMax.x ), floor( pObject.vecMax.y ) },
//                    pObject.colRect, pObject.uFlags, pObject.colOutline, pObject.flRounding, pObject.roundingCorners, pObject.flThickness );
//            else
//                AddDrawListRect( pDrawList, pObject.vecMin, pObject.vecMax, pObject.colRect, pObject.uFlags, pObject.colOutline, pObject.flRounding, pObject.roundingCorners, pObject.flThickness );
//
//            break;
//        }
//        case EDrawType::RECT_MULTICOLOR:
//        {
//            const auto& pObject = std::any_cast< RectMultiColorObject_t >( data.pObject );
//
//            if ( pObject.uFlags & DRAW_RECT_ALIGNED )
//                AddDrawListRectMultiColor( pDrawList,
//                    { floor( pObject.vecMin.x ), floor( pObject.vecMin.y ) },
//                    { floor( pObject.vecMax.x ), floor( pObject.vecMax.y ) },
//                    pObject.colUpperLeft, pObject.colUpperRight,
//                    pObject.colBottomLeft, pObject.colBottomRight,
//                    pObject.uFlags, pObject.colOutline );
//            else
//                AddDrawListRectMultiColor( pDrawList,
//                    pObject.vecMin,
//                    pObject.vecMax,
//                    pObject.colUpperLeft, pObject.colUpperRight,
//                    pObject.colBottomLeft, pObject.colBottomRight,
//                    pObject.uFlags, pObject.colOutline );
//
//            break;
//        }
//        case EDrawType::CIRCLE:
//        {
//            const auto& pObject = std::any_cast< CircleObject_t >( data.pObject );
//
//            if ( pObject.uFlags & DRAW_CIRCLE_FILLED )
//                pDrawList->AddCircleFilled( pObject.vecCenter, pObject.flRadius, pObject.colCircle, pObject.nSegments );
//            else
//                pDrawList->AddCircle( pObject.vecCenter, pObject.flRadius, pObject.colCircle, pObject.nSegments, pObject.flThickness );
//
//            if ( pObject.uFlags & DRAW_CIRCLE_OUTLINE )
//                pDrawList->AddCircle( pObject.vecCenter, pObject.flRadius + 1.0f, pObject.colOutline, pObject.nSegments, pObject.flThickness + 1.0f );
//
//            break;
//        }
//        case EDrawType::TRIANGLE:
//        {
//            const auto& pObject = std::any_cast< TriangleObject_t >( data.pObject );
//
//            if ( pObject.uFlags & DRAW_TRIANGLE_FILLED )
//                pDrawList->AddTriangleFilled( pObject.vecFirst, pObject.vecSecond, pObject.vecThird, pObject.colTriangle );
//            else
//                pDrawList->AddTriangle( pObject.vecFirst, pObject.vecSecond, pObject.vecThird, pObject.colTriangle, pObject.flThickness );
//
//            if ( pObject.uFlags & DRAW_TRIANGLE_OUTLINE )
//                pDrawList->AddTriangle( pObject.vecFirst, pObject.vecSecond, pObject.vecThird, pObject.colOutline, pObject.flThickness + 1.0f );
//
//            break;
//        }
//        case EDrawType::POLYGON:
//        {
//            const auto& pObject = std::any_cast< PolygonObject_t >( data.pObject );
//
//            if ( pObject.uFlags & DRAW_POLYGON_FILLED )
//                pDrawList->AddConvexPolyFilled( pObject.vecPoints.data( ), pObject.vecPoints.size( ), pObject.colPolygon );
//            else
//                pDrawList->AddPolyline( pObject.vecPoints.data( ), pObject.vecPoints.size( ), pObject.colPolygon, pObject.bClosed, pObject.flThickness );
//
//            if ( pObject.uFlags & DRAW_POLYGON_OUTLINE )
//                pDrawList->AddPolyline( pObject.vecPoints.data( ), pObject.vecPoints.size( ), pObject.colOutline, pObject.bClosed, pObject.flThickness + 1.0f );
//
//            break;
//        }
//        case EDrawType::TEXT:
//        {
//            const auto& pObject = std::any_cast< TextObject_t >( data.pObject );
//            AddDrawListText( pDrawList, pObject.pFont, pObject.flFontSize, pObject.vecPosition, pObject.szText, pObject.colText, pObject.uFlags, pObject.colOutline );
//            break;
//        }
//        case EDrawType::IMAGE:
//        {
//            const auto& pObject = std::any_cast< ImageObject_t >( data.pObject );
//            pDrawList->AddImageRounded( pObject.pTexture, pObject.vecMin, pObject.vecMax, ImVec2( 0, 0 ), ImVec2( 1, 1 ), pObject.colImage, pObject.flRounding, pObject.roundingCorners );
//            break;
//        }
//        case EDrawType::ARC:
//        {
//            const auto& pObject = std::any_cast< ArcObject_t >( data.pObject );
//            pDrawList->PathArcTo( pObject.vecPosition, pObject.flRadius, M_DEG2RAD( pObject.flMinAngle ), M_DEG2RAD( pObject.flMaxAngle ), 32 );
//            pDrawList->PathStroke( pObject.colFill, false, pObject.flThickness );
//            break;
//        }
//        case EDrawType::RING:
//        {
//            Vector      prevPos;
//            ImVec2      pervScreenPos, screenPos;
//            const auto& pObject = std::any_cast< RingObject_t >( data.pObject );
//            float       step = ( float )D3DX_PI * 2.0f / pObject.nSegments;
//
//            for ( float a = 0; a < ( D3DX_PI * 2.0f ); a += step )
//            {
//                if ( ( pObject.uFlags & DRAW_RING_COLLISIONS ) && g::pLocal )
//                {
//                    Vector position( pObject.flRadius * cos( a ) + pObject.vecCenter.x, pObject.flRadius * sin( a ) + pObject.vecCenter.y, pObject.vecCenter.z + 5.f );
//
//                    Trace_t      trace;
//                    CTraceFilter filter( g::pLocal );
//                    i::EngineTrace->TraceRay( Ray_t( pObject.vecCenter, position ), MASK_SHOT_BRUSHONLY, &filter, &trace );
//
//                    if ( WorldToScreen( trace.vecEnd, screenPos ) )
//                    {
//                        if ( pervScreenPos.x != 0.f && pervScreenPos.y != 0.f && screenPos.x != 0 && screenPos.y != 0 )
//                        {
//                            pDrawList->AddLine( ImVec2( pervScreenPos.x, pervScreenPos.y ), ImVec2( screenPos.x, screenPos.y ), pObject.colCircle, pObject.flThickness );
//                        }
//                        pervScreenPos = screenPos;
//                        prevPos = trace.vecEnd;
//                    }
//                }
//                else
//                {
//                    Vector start( pObject.flRadius * cosf( a ) + pObject.vecCenter.x, pObject.flRadius * sinf( a ) + pObject.vecCenter.y, pObject.vecCenter.z );
//                    Vector end( pObject.flRadius * cosf( a + step ) + pObject.vecCenter.x, pObject.flRadius * sinf( a + step ) + pObject.vecCenter.y, pObject.vecCenter.z );
//
//                    ImVec2 start2d;
//                    ImVec2 end2d;
//
//                    ImVec2 start22d( start2d.x, start2d.y );
//                    ImVec2 end22d( end2d.x, end2d.y );
//                    if ( WorldToScreen( start, start2d ) &&
//                        WorldToScreen( end, end2d ) )
//                    {
//                        start22d.x = start2d.x;
//                        start22d.y = start2d.y;
//
//                        end22d.x = end2d.x;
//                        end22d.y = end2d.y;
//
//                        pDrawList->AddLine( ImVec2( start22d.x, start22d.y ), ImVec2( end22d.x, end22d.y ), pObject.colCircle, pObject.flThickness );
//                    }
//                }
//            }
//            break;
//        }
//        default:
//            break;
//        }
//    }
//}
//
//void D::ClearDrawData( )
//{
//    if ( !vecDrawData.empty( ) )
//        vecDrawData.clear( );
//}
//
//void D::SwapDrawData( )
//{
//    std::unique_lock<std::shared_mutex> lock( drawMutex );
//    vecDrawData.swap( vecSafeDrawData );
//}
//
//#pragma region draw_render
//
//void D::AddLine( const ImVec2& vecStart, const ImVec2& vecEnd, const Color& colLine, float flThickness )
//{
//    vecDrawData.emplace_back( EDrawType::LINE, std::make_any<LineObject_t>( LineObject_t{ vecStart, vecEnd, colLine.GetU32( ), flThickness } ) );
//}
//
//void D::AddRect( const ImVec2& vecMin, const ImVec2& vecMax, const Color& colRect, unsigned int uFlags, const Color& colOutline, float flRounding, ImDrawCornerFlags roundingCorners, float flThickness )
//{
//    vecDrawData.emplace_back( EDrawType::RECT, std::make_any<RectObject_t>( RectObject_t{ vecMin, vecMax, colRect.GetU32( ), uFlags, colOutline.GetU32( ), flRounding, roundingCorners, flThickness } ) );
//}
//
//void D::AddRectMultiColor( const ImVec2& vecMin, const ImVec2& vecMax, const Color& colUpperLeft, const Color& colUpperRight, const Color& colBottomLeft, const Color& colBottomRight, unsigned int uFlags, const Color& colOutline )
//{
//    vecDrawData.emplace_back( EDrawType::RECT_MULTICOLOR, std::make_any<RectMultiColorObject_t>( RectMultiColorObject_t{ vecMin, vecMax, colUpperLeft.GetU32( ), colUpperRight.GetU32( ), colBottomLeft.GetU32( ), colBottomRight.GetU32( ), uFlags, colOutline.GetU32( ) } ) );
//}
//
//void D::AddCircle( const ImVec2& vecCenter, float flRadius, const Color& colCircle, int nSegments, unsigned int uFlags, const Color& colOutline, float flThickness )
//{
//    vecDrawData.emplace_back( EDrawType::CIRCLE, std::make_any<CircleObject_t>( CircleObject_t{ vecCenter, flRadius, colCircle.GetU32( ), nSegments, uFlags, colOutline.GetU32( ), flThickness } ) );
//}
//
//void D::AddRing( const Vector& vecCenter, float flRadius, const Color& colCircle, int nSegments, unsigned int uFlags, float flThickness )
//{
//    vecDrawData.emplace_back( EDrawType::RING, std::make_any<RingObject_t>( RingObject_t{ vecCenter, flRadius, colCircle.GetU32( ), nSegments, uFlags, flThickness } ) );
//}
//
//void D::AddTriangle( const ImVec2& vecFirst, const ImVec2& vecSecond, const ImVec2& vecThird, const Color& colTriangle, unsigned int uFlags, const Color& colOutline, float flThickness )
//{
//    vecDrawData.emplace_back( EDrawType::TRIANGLE, std::make_any<TriangleObject_t>( TriangleObject_t{ vecFirst, vecSecond, vecThird, colTriangle.GetU32( ), uFlags, colOutline.GetU32( ), flThickness } ) );
//}
//
//void D::AddPolygon( std::vector<ImVec2>& vecPoints, const Color& colPolygon, unsigned int uFlags, const Color& colOutline, bool bClosed, float flThickness )
//{
//    vecDrawData.emplace_back( EDrawType::POLYGON, std::make_any<PolygonObject_t>( PolygonObject_t{ std::move( vecPoints ), colPolygon.GetU32( ), uFlags, colOutline.GetU32( ), bClosed, flThickness } ) );
//}
//
//void D::AddText( const ImFont* pFont, float flFontSize, const ImVec2& vecPosition, const std::string& szText, const Color& colText, unsigned int uFlags, const Color& colOutline )
//{
//    if ( pFont->ContainerAtlas == nullptr )
//        return;
//
//    // check is only one flag selected
//    IM_ASSERT( ImIsPowerOfTwo( uFlags == DRAW_TEXT_NONE || uFlags & ( DRAW_TEXT_DROPSHADOW | DRAW_TEXT_OUTLINE ) ) );
//
//    vecDrawData.emplace_back( EDrawType::TEXT, std::make_any<TextObject_t>( TextObject_t{ pFont, flFontSize, vecPosition, szText, colText.GetU32( ), uFlags, colOutline.GetU32( ) } ) );
//}
//
//void D::AddText( const ImVec2& vecPosition, const std::string& szText, const Color& colText, int iFlags, const Color& colOutline )
//{
//    AddText( nullptr, 0.f, vecPosition, szText, colText, iFlags, colOutline );
//}
//
//void D::AddImage( ImTextureID pTexture, const ImVec2& vecMin, const ImVec2& vecMax, const Color& colImage, float flRounding, ImDrawCornerFlags roundingCorners )
//{
//    vecDrawData.emplace_back( EDrawType::IMAGE, std::make_any<ImageObject_t>( ImageObject_t{ pTexture, vecMin, vecMax, colImage.GetU32( ), flRounding, roundingCorners } ) );
//}
//
//void D::AddArcFilled( const ImVec2& vecPosition, float flRadius, float flMinAngle, float flMaxAngle, const Color& colFill, float flThickness )
//{
//    vecDrawData.emplace_back( EDrawType::ARC, std::make_any<ArcObject_t>( ArcObject_t{ vecPosition, flRadius, flMinAngle, flMaxAngle, colFill.GetU32( ), flThickness } ) );
//}
//
//#pragma endregion
//
//#pragma region draw_bindings
//void D::AddDrawListRect( ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, ImU32 colRect, unsigned int uFlags, ImU32 colOutline, float flRounding, ImDrawCornerFlags roundingCorners, float flThickness )
//{
//    if ( uFlags & DRAW_RECT_FILLED )
//        pDrawList->AddRectFilled( vecMin, vecMax, colRect, flRounding, roundingCorners );
//    else
//        pDrawList->AddRect( vecMin, vecMax, colRect, flRounding, roundingCorners, flThickness );
//
//    if ( uFlags & DRAW_RECT_BORDER )
//        pDrawList->AddRect( vecMin + ImVec2( 1.0f, 1.0f ), vecMax - ImVec2( 1.0f, 1.0f ), colOutline, flRounding, roundingCorners, 1.0f );
//
//    if ( uFlags & DRAW_RECT_OUTLINE )
//        pDrawList->AddRect( vecMin - ImVec2( 1.0f, 1.0f ), vecMax + ImVec2( 1.0f, 1.0f ), colOutline, flRounding, roundingCorners, 1.0f );
//}
//
//void D::AddDrawListRectMultiColor( ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, ImU32 colUpperLeft, ImU32 colUpperRight, ImU32 colBottomLeft, ImU32 colBottomRight, unsigned int uFlags, ImU32 colOutline )
//{
//    // @todo:
//    // implement thickness
//
//    if ( uFlags & DRAW_RECT_FILLED )
//        pDrawList->AddRectFilledMultiColor( vecMin, vecMax, colUpperLeft, colUpperRight, colBottomRight, colBottomLeft );
//    else
//    {
//        pDrawList->AddRectFilledMultiColor( vecMin, ImVec2( vecMax.x, vecMin.y + 1.f ), colUpperLeft, colUpperRight, colUpperRight, colUpperLeft );                // top
//        pDrawList->AddRectFilledMultiColor( ImVec2( vecMin.x, vecMax.y ), vecMax - ImVec2( 0, 1.f ), colBottomLeft, colBottomRight, colBottomRight, colBottomLeft ); // bottom
//
//        pDrawList->AddRectFilledMultiColor( vecMin, ImVec2( vecMin.x + 1.f, vecMax.y ), colUpperLeft, colUpperLeft, colBottomLeft, colBottomLeft );                // left
//        pDrawList->AddRectFilledMultiColor( ImVec2( vecMax.x, vecMin.y ), vecMax - ImVec2( 1.f, 0 ), colUpperRight, colUpperRight, colBottomRight, colBottomRight ); // right
//    }
//
//    if ( uFlags & DRAW_RECT_BORDER )
//        pDrawList->AddRect( vecMin + ImVec2( 1.0f, 1.0f ), vecMax - ImVec2( 1.0f, 1.0f ), colOutline, 0, ImDrawCornerFlags_All, 1.0f );
//
//    if ( uFlags & DRAW_RECT_OUTLINE )
//        pDrawList->AddRect( vecMin - ImVec2( 1.0f, 1.0f ), vecMax + ImVec2( 1.0f, 1.0f ), colOutline, 0, ImDrawCornerFlags_All, 1.0f );
//}
//
//void D::AddDrawListText( ImDrawList* pDrawList, const ImFont* pFont, float flFontSize, const ImVec2& vecPosition, const std::string& szText, ImU32 colText, unsigned int uFlags, ImU32 colOutline )
//{
//    // set font texture
//    pDrawList->PushTextureID( pFont->ContainerAtlas->TexID );
//
//    if ( uFlags & DRAW_TEXT_DROPSHADOW )
//        pDrawList->AddText( pFont, flFontSize, vecPosition + ImVec2( 1.0f, 1.0f ), colOutline, szText.c_str( ) );
//    else if ( uFlags & DRAW_TEXT_OUTLINE )
//    {
//        pDrawList->AddText( pFont, flFontSize, vecPosition + ImVec2( 0.0f, -1.0f ), colOutline, szText.c_str( ) );
//        pDrawList->AddText( pFont, flFontSize, vecPosition + ImVec2( 0.0f, 1.0f ), colOutline, szText.c_str( ) );
//        pDrawList->AddText( pFont, flFontSize, vecPosition + ImVec2( -1.0f, 0.0f ), colOutline, szText.c_str( ) );
//        pDrawList->AddText( pFont, flFontSize, vecPosition + ImVec2( 1.0f, 0.0f ), colOutline, szText.c_str( ) );
//    }
//
//    pDrawList->AddText( pFont, flFontSize, vecPosition, colText, szText.data( ) );
//    pDrawList->PopTextureID( );
//}
//#pragma endregion
//
//#pragma region draw_extra
//bool D::WorldToScreen( const Vector& vecOrigin, ImVec2& vecScreen )
//{
//    const ViewMatrix_t& matWorldToScreen = i::EngineClient->WorldToScreenMatrix( );
//    const float         flWidth = matWorldToScreen[ 3 ][ 0 ] * vecOrigin.x + matWorldToScreen[ 3 ][ 1 ] * vecOrigin.y + matWorldToScreen[ 3 ][ 2 ] * vecOrigin.z + matWorldToScreen[ 3 ][ 3 ];
//
//    if ( flWidth < 0.001f )
//        return false;
//
//    // compute the scene coordinates of a point in 3d
//    const float flInverse = 1.0f / flWidth;
//    vecScreen.x = ( matWorldToScreen[ 0 ][ 0 ] * vecOrigin.x + matWorldToScreen[ 0 ][ 1 ] * vecOrigin.y + matWorldToScreen[ 0 ][ 2 ] * vecOrigin.z + matWorldToScreen[ 0 ][ 3 ] ) * flInverse;
//    vecScreen.y = ( matWorldToScreen[ 1 ][ 0 ] * vecOrigin.x + matWorldToScreen[ 1 ][ 1 ] * vecOrigin.y + matWorldToScreen[ 1 ][ 2 ] * vecOrigin.z + matWorldToScreen[ 1 ][ 3 ] ) * flInverse;
//
//    // screen transform
//    // get the screen position in pixels of given point
//    const ImVec2 vecDisplaySize = ImGui::GetIO( ).DisplaySize;
//    vecScreen.x = ( vecDisplaySize.x * 0.5f ) + ( vecScreen.x * vecDisplaySize.x ) * 0.5f;
//    vecScreen.y = ( vecDisplaySize.y * 0.5f ) - ( vecScreen.y * vecDisplaySize.y ) * 0.5f;
//    return true;
//}
//
//void D::Render3DCircleOneteap( Vector m_vecPosition, Color m_col, int m_iPointCount, float m_flRadius, bool m_bFade, float m_flRotationStart, float m_flFadeStart, float m_flFadeLength )
//{
//    float               m_flStep = static_cast< float >( D3DX_PI ) * 2.0f / m_iPointCount;
//    std::vector<Vector> points3d;
//
//    int m_iAlpha = 255;
//    int m_iFadeStartPoint = 0;
//    int m_iFadeEndPoint = 0;
//    int m_iFadeStep = 0;
//    if ( m_bFade )
//    {
//        m_iFadeStartPoint = ( int )( m_iPointCount * m_flFadeStart );
//        m_iFadeEndPoint = m_iFadeStartPoint + ( int )( m_iPointCount * m_flFadeLength );
//        m_iFadeStep = 255 / ( m_iFadeEndPoint - m_iFadeStartPoint );
//    }
//
//    auto  m_colOuter = Color( 0, 0, 0, 255 );
//    Color m_colInner = m_col;
//
//    for ( int i = 0; i < m_iPointCount; i++ )
//    {
//        if ( m_bFade && i > m_iFadeEndPoint )
//            break;
//
//        float m_flTheta = ( i * m_flStep ) - ( static_cast< float >( D3DX_PI ) * 2.f * m_flRotationStart );
//
//        Vector m_vecWorldStart( m_flRadius * cosf( m_flTheta ) + m_vecPosition.x, m_flRadius * sinf( m_flTheta ) + m_vecPosition.y, m_vecPosition.z );
//        Vector m_vecWorldEnd( m_flRadius * cosf( m_flTheta + m_flStep ) + m_vecPosition.x, m_flRadius * sinf( m_flTheta + m_flStep ) + m_vecPosition.y, m_vecPosition.z );
//
//        ImVec2 m_vecStart, m_vecEnd;
//        if ( !D::WorldToScreen( m_vecWorldStart, m_vecStart ) || !D::WorldToScreen( m_vecWorldEnd, m_vecEnd ) )
//            return;
//
//        if ( m_bFade && i >= m_iFadeStartPoint )
//        {
//            m_iAlpha -= m_iFadeStep;
//
//            if ( m_iAlpha < 0 )
//                m_iAlpha = 0;
//        }
//
//        D::AddLine( ImVec2( m_vecStart.x, m_vecStart.y + 1 ), ImVec2( m_vecEnd.x + 1, m_vecEnd.y + 1 ), Color( m_colOuter.Get<COLOR_R>( ), m_colOuter.Get<COLOR_G>( ), m_colOuter.Get<COLOR_B>( ), static_cast< uint8_t >( m_iAlpha ) ), 1.f );
//        D::AddLine( ImVec2( m_vecStart.x, m_vecStart.y - 1 ), ImVec2( m_vecEnd.x - 1, m_vecEnd.y - 1 ), Color( m_colOuter.Get<COLOR_R>( ), m_colOuter.Get<COLOR_G>( ), m_colOuter.Get<COLOR_B>( ), static_cast< uint8_t >( m_iAlpha ) ), 1.f );
//        D::AddLine( ImVec2( m_vecStart.x, m_vecStart.y ), ImVec2( m_vecEnd.x, m_vecEnd.y ), Color( m_colInner.Get<COLOR_R>( ), m_colInner.Get<COLOR_G>( ), m_colInner.Get<COLOR_B>( ), static_cast< uint8_t >( m_iAlpha ) ), 1.f );;
//    }
//}
//
//void D::Draw3dDottedRing( Vector m_vecPosition, int16_t m_iRadius, uint16_t m_iPointCount, Color m_col, float m_flThickness )
//{
//    ImVec2 m_vecStart2D;
//    float  m_flStep = ( float )D3DX_PI * 2.0f / m_iPointCount;
//
//    for ( float a = 0; a < ( D3DX_PI * 2.0f ); a += m_flStep )
//    {
//        Vector m_vecStart( m_iRadius * cosf( a ) + m_vecPosition.x, m_iRadius * sinf( a ) + m_vecPosition.y, m_vecPosition.z );
//
//        if ( D::WorldToScreen( m_vecStart, m_vecStart2D ) )
//        {
//            AddLine( ImVec2( m_vecStart2D.x, m_vecStart2D.y ), ImVec2( m_vecStart2D.x + 1, m_vecStart2D.y + 1 ), m_col, m_flThickness );
//        }
//    }
//}
//
//void D::Draw3dRing( Vector m_vecPosition, int16_t m_iRadius, uint16_t m_iPointCount, Color m_col, float m_flThickness )
//{
//    ImVec2 m_vecStart2D;
//    ImVec2 m_vecEnd2D;
//    float  m_flStep = ( float )D3DX_PI * 2.0f / m_iPointCount;
//
//    for ( float a = 0; a < ( D3DX_PI * 2.0f ); a += m_flStep )
//    {
//        Vector m_vecStart( m_iRadius * cosf( a ) + m_vecPosition.x, m_iRadius * sinf( a ) + m_vecPosition.y, m_vecPosition.z );
//        Vector m_vecEnd( m_iRadius * cosf( a + m_flStep ) + m_vecPosition.x, m_iRadius * sinf( a + m_flStep ) + m_vecPosition.y, m_vecPosition.z );
//
//        if ( D::WorldToScreen( m_vecStart, m_vecStart2D ) && D::WorldToScreen( m_vecEnd, m_vecEnd2D ) )
//        {
//            AddLine( ImVec2( m_vecStart2D.x, m_vecStart2D.y ), ImVec2( m_vecEnd2D.x, m_vecEnd2D.y ), m_col, m_flThickness );
//        }
//    }
//}
//
//void D::Draw3DCircleFilled( Vector m_vecPosition, Color m_col, int m_iPointCount, float m_flRadius, float m_flThickness )
//{
//    float               m_flStep = ( float )D3DX_PI * 2.0f / m_iPointCount;
//    std::vector<ImVec2> m_vecPoints;
//    for ( float m_flLat = 0.f; m_flLat <= D3DX_PI * 2.0f; m_flLat += m_flStep )
//    {
//        const Vector& m_vecPoint3D = Vector( sin( m_flLat ), cos( m_flLat ), 0.f ) * m_flRadius;
//        ImVec2        m_vecPoint2D;
//        if ( D::WorldToScreen( m_vecPosition + m_vecPoint3D, m_vecPoint2D ) )
//            m_vecPoints.push_back( ImVec2( m_vecPoint2D.x, m_vecPoint2D.y ) );
//    }
//    D::AddPolygon( m_vecPoints, m_col, 2U, Color( 0, 0, 0, 255 ), true, m_flThickness );
//}
//
//#pragma endregion