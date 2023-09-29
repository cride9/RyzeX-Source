#pragma once
#include "../../utilities.h"

#define CONCAT(a, b) a##b
#define PAD_NAME(n) CONCAT(pad, n)

#define PAD(size) \
private: \
    std::byte PAD_NAME(__COUNTER__) [size]; \
public:

template <typename T>
class UtlVector {
public:
	constexpr T& operator[]( int i ) noexcept { return memory[ i ]; };
	constexpr const T& operator[]( int i ) const noexcept { return memory[ i ]; };

	T* memory;
	int allocationCount;
	int growSize;
	int size;
	T* elements;
};

template <typename T>
struct UtlMemory {
	T& operator[]( int i ) noexcept { return memory[ i ]; };

	T* memory;
	int allocationCount;
	int growSize;
};

template <typename Key, typename Value>
struct Node {
	int left;
	int right;
	int parent;
	int type;
	Key key;
	Value value;
};

template <typename Key, typename Value>
struct UtlMap
{
	auto begin() const noexcept { return memory.memory; }
	auto end() const noexcept { return memory.memory + numElements; }

	int find(Key key) const noexcept
	{
		auto curr = root;

		while (curr != -1)
		{
			const auto el = memory[curr];

			if (el.key < key)
				curr = el.right;
			else if (el.key > key)
				curr = el.left;
			else
				break;
		}
		return curr;
	}

	void* lessFunc;
	UtlMemory<Node<Key, Value>> memory;
	int root;
	int numElements;
	int firstFree;
	int lastAlloc;
	Node<Key, Value>* elements;
};

struct String
{
	UtlMemory<char> buffer;
	int length;

	char* data( ) noexcept { return buffer.memory; }
};


struct PaintKit_t
{
	int m_nID;
	String m_szName;
	String m_szDescription;
	String m_szDescriptionTag;
	String m_szUnk1;
	String m_szPattern;
	String m_szUnk2;
	String m_szLogoMaterial;
	bool m_bBaseDiffuseOverride;
	int m_nRarity;
	int m_nStyle;
	Color m_rgbaColor[ 4 ];
	Color m_rgbaLogoColor[ 4 ];
	float m_flWearDefault;
	float m_flWearRemapMin;
	float m_flWearRemapMax;
	unsigned char m_nFixedSeed;
	unsigned char m_uchPhongExponent;
	unsigned char m_uchPhongAlbedoBoost;
	unsigned char m_uchPhongIntensity;
	float m_flPatternScale;
	float m_flPatternOffsetXStart;
	float m_flPatternOffsetXEnd;
	float m_flPatternOffsetYStart;
	float m_flPatternOffsetYEnd;
	float m_flPatternRotateStart;
	float m_flPatternRotateEnd;
	float m_flLogoScale;
	float m_flLogoOffsetX;
	float m_flLogoOffsetY;
	float m_flLogoRotation;
	bool m_bIgnoreWeaponSizeScale;
	char m_chUnk3[ 3 ];
	int m_nViewModelExponentOverrideSize;
	bool m_bOnlyFirstMaterial;
	bool m_bUnk4;
	char m_chUnk5[ 2 ];
	float m_flPearlescent;
};

struct StickerKit
{
	int id;
	int rarity;
	String name;
	String description;
	String itemName;
	PAD( 2 * sizeof( String ) )
		String inventoryImage;
};

template <typename T, std::size_t Idx, typename ...Args>
constexpr T call( void* classBase, Args... args ) noexcept
{
	return ( *reinterpret_cast< T( __thiscall*** )( void*, Args... ) >( classBase ) )[ Idx ]( classBase, args... );
}

class EconItemDefinition {
public:

	short getWeaponId() noexcept {

		return call<short, 0>(this);
	}
	const char* getItemBaseName() noexcept {

		return call<const char*, 2>(this);
	}
	const char* getItemTypeName() noexcept {

		return call<const char*, 3>(this);
	}
	const char* getPlayerDisplayModel() noexcept {

		return call<const char*, 6>(this);
	}
	const char* getWorldDisplayModel() noexcept {

		return call<const char*, 7>(this);
	}
	std::uint8_t getRarity() noexcept {

		return call<std::uint8_t, 12>(this);
	}
	int getCapabilities( ) noexcept
	{
		return *reinterpret_cast< int* >( this + 0x148 );
	}

	bool isPaintable( ) noexcept
	{
		return getCapabilities( ) & 1; // ITEM_CAP_PAINTABLE
	}

	const char* getDefinitionName( ) noexcept
	{
		return *reinterpret_cast< const char** >( this + 0x1DC ); // TODO: update linux
	}
};


struct ItemListEntry {
	int itemDef;
	int paintKit;
	PAD( 20 )

		auto weaponId( ) const noexcept
	{
		return static_cast< short >( itemDef );
	}
};

class EconLootListDefinition {
public:
	const char* getName() noexcept {

		return call<const char*, 0>(this);
	}
	const UtlVector<ItemListEntry>& getLootListContents() noexcept {

		return call<const UtlVector<ItemListEntry>&, 1>(this);
	}
};

class EconItemSetDefinition {
public:
	const char* getLocKey() noexcept {

		return call<const char*, 1>(this);
	}
	int getItemCount() noexcept {

		return call<int, 4>(this);
	}
	short getItemDef(int index) noexcept {

		return call<short, 5>(this, index);
	}
	int getItemPaintKit(int index) noexcept {

		return call<int, 6>(this, index);
	}
};

struct EconItemQualityDefinition {
	int value;
	const char* name;
	unsigned weight;
	bool explicitMatchesOnly;
	bool canSupportSet;
	const char* hexColor;
};

struct UtlString {
	UtlMemory<char> buffer;
	int length;

	const char* data( ) const noexcept { return buffer.memory; }
};

struct AlternateIconData {
	UtlString simpleName;
	UtlString largeSimpleName;
	UtlString iconURLSmall;
	UtlString iconURLLarge;
	PAD( 28 )
};

struct EconMusicDefinition {
	int id;
	const char* name;
	const char* nameLocalized;
	PAD( 2 * sizeof( const char* ) )
		const char* inventoryImage;
};

class ItemSchema_t
{
public:
	PAD( ( 0x88 ) )
		UtlMap<int, EconItemQualityDefinition> qualities;
	PAD( ( 0x48 ) )
		UtlMap<int, EconItemDefinition*> itemsSorted;
	PAD( ( 0x60 ) )
		UtlMap<int, const char*> revolvingLootLists;
	PAD( ( 0x80 ) )
		UtlMap<std::uint64_t, AlternateIconData> alternateIcons;
	PAD( ( 0x48 ) )
		UtlMap<int, PaintKit_t*> m_pPaintKits;
	UtlMap<int, StickerKit*> stickerKits;
	PAD( ( 0x11C ) )
		UtlMap<int, EconMusicDefinition*> musicKits;

	EconItemDefinition* getItemDefinitionInterface(short id) noexcept {

		return call<EconItemDefinition*, 4>(this, id);
	}
	const char* getRarityName(int rarity) noexcept {

		return call<const char*, 19>(this, rarity);
	}
	int getItemSetCount() noexcept {

		return call<int, 28>(this);
	}
	EconItemSetDefinition* getItemSet(int index) noexcept {

		return call<EconItemSetDefinition*, 29>(this, index);
	}
	EconLootListDefinition* getLootList(int index) noexcept {

		return call<EconLootListDefinition*, 32>(this, index);
	}
	int getLootListCount() noexcept {

		return call<int, 34>(this);
	}
	EconItemDefinition* getItemDefinitionByName(const char* name) noexcept {

		return call<EconItemDefinition*, 42>(this, name);
	}
};

class IItemSystem
{
public:
	__forceinline ItemSchema_t* GetItemSchema()
	{
		return util::GetVFunc< ItemSchema_t * (__thiscall*)(void*) >(this, 0)(this);
	}
};