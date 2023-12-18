/**
 * 所有资产的基类
 * 包含每个资产唯一的id属性
 */
#pragma once

#include <stdint.h>

namespace Yutrel
{

    class Asset
    {

    public:
        uint32_t ID() const { return this->m_id; }

    protected:
        Asset()          = default;
        virtual ~Asset() = default;

        Asset(const Asset&)            = delete;
        Asset& operator=(const Asset&) = delete;
        Asset(Asset&& other) noexcept;
        Asset& operator=(Asset&& other) noexcept;

        virtual void Bind() const {}
        virtual void Unbind() const {}

        virtual void Bind(uint32_t index) const {}
        virtual void Unbind(uint32_t index) const {}

    protected:
        uint32_t m_id;
    };
} // namespace Yutrel