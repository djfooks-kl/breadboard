#pragma once

#include <functional>

namespace xc
{
    template<typename TResourceType>
    struct ResourceId
    {
        using TType = TResourceType;

        ResourceId()
            : m_Name(nullptr)
        {
        }

        ResourceId(const ResourceId<TResourceType>& other)
            : m_Name(other.m_Name)
        {
        }

        ResourceId<TResourceType>& operator=(const ResourceId<TResourceType>& other)
        {
            m_Name = other.m_Name;
            return *this;
        }

        static ResourceId<TResourceType> Create(const char* name)
        {
            return ResourceId<TResourceType>(name);
        }

        bool operator==(const ResourceId<TResourceType> other) const
        {
            return m_Name == other.m_Name;
        }

        bool IsEmpty() const
        {
            return !m_Name;
        }

        operator bool() const
        {
            return m_Name;
        }

        const char* GetName() const
        {
            return m_Name;
        }

    private:
        explicit ResourceId(const char* name)
            : m_Name(name)
        {
        }

        const char* m_Name;
    };
}

namespace std
{
    template <typename T>
    struct hash<xc::ResourceId<T>>
    {
        size_t operator()(const xc::ResourceId<T>& key) const
        {
            return hash<const char*>()(key.GetName());
        }
    };
}
