#include "SDL2Thread.h"
#include <SDL_thread.h>
#include <SDL_mutex.h>

namespace Core
{

Mutex::Mutex()
    : m_handle(nullptr)
{
    m_handle = SDL_CreateMutex();
}

Mutex::~Mutex()
{
    if (m_handle)
    {
        SDL_DestroyMutex(static_cast<SDL_mutex*>(m_handle));
        m_handle = nullptr;
    }
}

void Mutex::Lock()
{
    SDL_LockMutex(static_cast<SDL_mutex*>(m_handle));
}

void Mutex::Unlock()
{
    SDL_UnlockMutex(static_cast<SDL_mutex*>(m_handle));
}

} // namespace Core
