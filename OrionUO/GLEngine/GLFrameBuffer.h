#pragma once

#include "GLTexture.h"

class CGLFrameBuffer
{
public:
    CGLFrameBuffer();
    ~CGLFrameBuffer();

    bool Init(int a_width, int a_height);
    bool Init(const Core::Vec2<i32>& a_size) { return Init(a_size.x, a_size.y); }
    void Free();
    void Release();
    bool Ready() const { return m_isReady; }
    bool Ready(int a_width, int a_height);
    bool Ready(const Core::Vec2<i32>& a_size) { return Ready(a_size.x, a_size.y); }
    bool ReadyMinSize(int a_width, int a_height);
    bool ReadyMinSize(const Core::Vec2<i32>& a_size) { return Ready(a_size.x, a_size.y); }
    bool Use();
    void Draw(int x, int y);

private:
    CGLTexture m_texture{ CGLTexture() };
    bool m_isReady         = false;
    GLint m_oldFrameBuffer = 0;
    GLuint m_frameBuffer   = 0;
};
