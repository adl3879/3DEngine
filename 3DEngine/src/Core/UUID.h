#pragma once

#include <stdint.h>
#include <string>

namespace Engine
{
class UUID
{
public:
    UUID();
    UUID(uint64_t uuid);
    UUID(const UUID &other) = default;

    const std::string ToString() const;

    operator uint64_t() const { return m_UUID; }
    // bool operator==(const UUID &other) const { return m_UUID == other.m_UUID; }
    // bool operator==(int other) const { return m_UUID == other; }

private:
    uint64_t m_UUID;
};
} // namespace Engine

namespace std
{
template <> struct hash<Engine::UUID>
{
    size_t operator()(const Engine::UUID &uuid) const
    {
        //
        return hash<uint64_t>()(static_cast<uint64_t>(uuid));
    }
};
} // namespace std
