#pragma once

#include <stdint.h>

namespace Engine
{
class UUID
{
  public:
    UUID();
    UUID(uint64_t uuid);
    UUID(const UUID &other) = default;

    operator uint64_t() const { return m_UUID; }

  private:
    uint64_t m_UUID;
};
} // namespace Engine

// namespace std
// {
// template <> struct hash<Engine::UUID>
// {
//     size_t operator()(const Engine::UUID &uuid) const
//     {
//         //
//         return hash<uint64_t>()(static_cast<uint64_t>(uuid));
//     }
// };
// } // namespace std