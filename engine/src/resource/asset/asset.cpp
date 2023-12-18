#include "yutrel_pch.hpp"

#include "asset.hpp"

namespace Yutrel
{
    Asset::Asset(Asset&& other) noexcept
        : m_id{std::exchange(other.m_id, 0)}
    {
    }

    Asset& Asset::operator=(Asset&& other) noexcept
    {
        if (this != &other)
        {
            this->m_id = std::exchange(other.m_id, 0);
        }
        return *this;
    }
} // namespace Yutrel