#pragma once

#include "Common.hlsli"
#include "Interval.hlsli"

interface Object
{
    bool hit(Ray ray, Interval ray_t, out HitRecord rec);
};