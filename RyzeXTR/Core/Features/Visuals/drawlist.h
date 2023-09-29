#pragma once
#include <deque>
#include <any>
#include "../../Interface/interfaces.h"
#include "../../SDK/DataTyes/Vector.h"
#include "../../SDK/DataTyes/Color.h"

enum DRAWTYPES : int
{
	DRAWTYPE_LINE,
	DRAWTYPE_CIRCLE,
	DRAWTYPE_RECT,
	DRAWTYPE_TEXT,
	DRAWTYPE_MAX
};

enum DRAWFLAGS : int
{
	DRAWFLAGS_OUTLINE,
	DRAWFLAGS_FILLED,
	DRAWFLAGS_MAX
};

struct DrawObjects_t
{
	DrawObjects_t( const DRAWTYPES nType, std::any&& pObject ) :
		nType( nType ), pObject( std::move( pObject ) ) { }

	DRAWTYPES nType = DRAWTYPE_MAX;
	std::any pObject = { };
};

struct LineObject_t
{
	Vector2D vecPosition = Vector2D( 0, 0 );
	Vector2D vecEndPosition = Vector2D( 0, 0 );
	Color colDrawColor = Color( 255, 255, 255, 255 );
};

struct CircleObject_t
{
	Vector2D vecPosition = Vector2D( 0, 0 );
	int iRadius = 0;
	int iSegments = 0;
	Color colDrawColor = Color( 255, 255, 255, 255 );
};

struct RectangleObject_t
{
	Vector2D vecPosition = Vector2D( 0, 0 );
	Vector2D vecEndPosition = Vector2D( 0, 0 );
	DRAWFLAGS nFlags = DRAWFLAGS_MAX;
	Color colDrawColor = Color( 255, 255, 255, 255 );
};

struct TextObject_t
{
	Vector2D vecPosition = Vector2D( 0, 0 );
	unsigned long hFont = { };
	bool bCenter = false;
	Color colDrawColor = Color( 255, 255, 255, 255 );
	std::string szText = { };
};

namespace drawlist {

	inline std::deque<DrawObjects_t> deqList{ };

	inline void AddLine( Vector2D vecPosition, Vector2D vecEndPosition, Color colDrawColor )
	{
		deqList.emplace_back( DRAWTYPE_LINE, std::make_any<LineObject_t>( LineObject_t{ vecPosition, vecEndPosition, colDrawColor } ) );
	}

	inline void AddCircle( Vector2D vecPosition, int iRadius, int iSegments, Color colDrawColor )
	{
		deqList.emplace_back( DRAWTYPE_CIRCLE, std::make_any<CircleObject_t>( CircleObject_t{ vecPosition, iRadius, iSegments, colDrawColor } ) );
	}

	inline void AddRect( Vector2D vecPosition, Vector2D vecEndPosition, DRAWFLAGS nFlags, Color colDrawColor )
	{
		deqList.emplace_back( DRAWTYPE_RECT, std::make_any<RectangleObject_t>( RectangleObject_t{ vecPosition, vecEndPosition, nFlags, colDrawColor } ) );
	}

	inline void AddText( Vector2D vecPosition, unsigned long hFont, bool bCenter, Color colDrawColor, std::string szText )
	{
		deqList.emplace_back( DRAWTYPE_TEXT, std::make_any<TextObject_t>( TextObject_t{ vecPosition, hFont, bCenter, colDrawColor, szText } ) );
	}

	inline void Draw( ) {

		for (DrawObjects_t& Object : deqList)
		{
			switch (Object.nType)
			{
				case DRAWTYPE_LINE:
				{
					const auto& pObject = std::any_cast< LineObject_t >( Object.pObject );
					i::Surface->DrawSetColor( pObject.colDrawColor.Get<COLOR_R>( ), pObject.colDrawColor.Get<COLOR_G>( ), pObject.colDrawColor.Get<COLOR_B>( ), pObject.colDrawColor.Get<COLOR_A>( ) );
					i::Surface->DrawLine( pObject.vecPosition.x, pObject.vecPosition.y, pObject.vecEndPosition.x, pObject.vecEndPosition.y );
					break;
				}
				case DRAWTYPE_CIRCLE:
				{
					const auto& pObject = std::any_cast< CircleObject_t >( Object.pObject );
					i::Surface->DrawSetColor( pObject.colDrawColor.Get<COLOR_R>( ), pObject.colDrawColor.Get<COLOR_G>( ), pObject.colDrawColor.Get<COLOR_B>( ), pObject.colDrawColor.Get<COLOR_A>( ) );
					i::Surface->DrawOutlinedCircle( pObject.vecPosition.x, pObject.vecPosition.y, pObject.iRadius, pObject.iSegments );
					break;
				}
				case DRAWTYPE_RECT:
				{
					const auto& pObject = std::any_cast< RectangleObject_t >( Object.pObject );
					if (pObject.nFlags == DRAWFLAGS_FILLED)
					{
						i::Surface->DrawSetColor( pObject.colDrawColor.Get<COLOR_R>( ), pObject.colDrawColor.Get<COLOR_G>( ), pObject.colDrawColor.Get<COLOR_B>( ), pObject.colDrawColor.Get<COLOR_A>( ) );
						i::Surface->DrawFilledRect( pObject.vecPosition.x, pObject.vecPosition.y, pObject.vecEndPosition.x, pObject.vecEndPosition.y );
					}

					if (pObject.nFlags == DRAWFLAGS_OUTLINE)
					{
						i::Surface->DrawSetColor( pObject.colDrawColor.Get<COLOR_R>( ), pObject.colDrawColor.Get<COLOR_G>( ), pObject.colDrawColor.Get<COLOR_B>( ), pObject.colDrawColor.Get<COLOR_A>( ) );
						i::Surface->DrawOutlinedRect( pObject.vecPosition.x, pObject.vecPosition.y, pObject.vecEndPosition.x, pObject.vecEndPosition.y );
					}
					break;
				}
				case DRAWTYPE_TEXT:
				{
					const auto& pObject = std::any_cast< TextObject_t >( Object.pObject );
					i::Surface->DrawT( pObject.vecPosition.x, pObject.vecPosition.y, pObject.colDrawColor, pObject.hFont, pObject.bCenter, pObject.szText.c_str( ) );
					break;
				}
			}
		}

		// we are done drawing so let's clear the deque
		if (!deqList.empty( ))
			deqList.clear( );
	}
}