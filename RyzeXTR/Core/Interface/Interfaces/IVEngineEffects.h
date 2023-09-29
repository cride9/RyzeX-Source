#pragma once
#include "../../utilities.h"
#include "../../SDK/DataTyes/Vector.h"

struct Dlight_t
{
    int        flags;
    Vector    origin;
    float    radius;
    Color    color;
    float    die;
    float    decay;
    float    minlight;
    int        key;
    int        style;
    Vector direction;
    float    innerAngle;
    float    outerAngle;
    const void* exclusiveLightReceiver;

    float GetRadius() const
    {
        return radius;
    }

    float GetRadiusSquared() const
    {
        return radius * radius;
    }

    float IsRadiusGreaterThanZero() const
    {
        return radius > 0.0f;
    }
};

struct WorldDLight_t
{
    Vector origin;
    Vector intensity;
    Vector normal;
    Vector shadow_cast_offset;
    int cluster;
    int type;
    int style;
    float stopdot;
    float stopdot2;
    float exponent;
    float radius;
    float constant_attn;
    float linear_attn;
    float quadratic_attn;
    int flags;
    int texinfo;
    int owner;
};

struct WorldBrushData_t
{
    uint8_t pad[0xA8];
    int numworldlights;
    WorldDLight_t* worldlights;
};

class IVEffects
{
public:
    Dlight_t* CL_AllocDlight(int key)
    {
        return util::CallVFunc<Dlight_t*>(this, 4, key);
    }

    Dlight_t* CL_AllocElight(int key)
    {
        return util::CallVFunc<Dlight_t*>(this, 5, key);
    }

    Dlight_t* GetElightByKey(int key)
    {
        return util::CallVFunc<Dlight_t*>(this, 8, key);
    }
};