#pragma once
#include <string>
namespace Lil::Assets {
    class AssetRef {
    public:

        AssetRef() = default;
        AssetRef(const char* key) : m_key(key) {}
        AssetRef(std::string key) : m_key(std::move(key)) {}

        const std::string& Key() const { return m_key; }
        bool IsSet() const { return m_key != "None"; }
        bool operator==(const AssetRef& o) const { return m_key == o.m_key; }
        
        size_t Hash() const { return std::hash<std::string>{}(m_key); }

        template <class Archive> std::string save_minimal(const Archive&) const { return m_key; }
        template <class Archive> void load_minimal(const Archive&, const std::string& s) { m_key = s; }
    private:
        std::string m_key = "None";
    };
}
#define LIL_ASSET_REF_HASH(T) \
template <> struct std::hash<T> { \
    size_t operator()(const T& r) const noexcept { return r.Hash(); } \
};

