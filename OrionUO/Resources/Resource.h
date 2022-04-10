#pragma once

#include "Core/Minimal.h"

namespace Resources
{

struct Resource
{
    Resource()
        : m_references_(0)
    {
    }
    virtual ~Resource() {}

private:
    friend struct ResourcePtr;
    u32 m_references_;
};
template<typename TResource>
struct ResourcePtr
{
    TResource* operator->() { return m_p_; }
    TResource& operator*() { return *m_p_; }
    ResourcePtr(TResource* a_p)
        : m_p_(a_p)
    {
        ++m_p_->m_references_;
    }
    ~ResourcePtr()
    {
        if (--m_p_->m_references_ == 0)
            delete m_p_;
    }
    ResourcePtr(ResourcePtr const& a_p)
        : m_p_(a_p.m_p_)
    {
        ++m_p_->m_references_;
    }
    ResourcePtr& operator=(ResourcePtr const& a_p)
    {
        TResource* const old = m_p_;
        m_p_            = a_p.m_p_;
        ++m_p_->m_references_;
        if (--old->m_references_ == 0)
            delete old;
        return *this;
    }

private:
    TResource* m_p_;
};

} // namespace Resources
