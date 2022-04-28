#include "GLFrameBuffer.h"
#include "GLEngine/GLEngine.h"

CGLFrameBuffer::CGLFrameBuffer()
{
}

CGLFrameBuffer::~CGLFrameBuffer()
{
    Free();
}

bool CGLFrameBuffer::Init(int a_width, int a_height)
{
    Free();

    bool result = false;

    if (g_GL.CanUseFrameBuffer && (a_width != 0) && (a_height != 0))
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &m_texture.Texture);
        glBindTexture(GL_TEXTURE_2D, m_texture.Texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA8,
            a_width,
            a_height,
            0,
            GL_BGRA,
            GL_UNSIGNED_INT_8_8_8_8,
            nullptr);

        GLint currentFrameBuffer = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFrameBuffer);

        glGenFramebuffers(1, &m_frameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

        glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture.Texture, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        {
            m_texture.Width  = a_width;
            m_texture.Height = a_height;

            result    = true;
            m_isReady = true;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, currentFrameBuffer);
    }

    return result;
}

void CGLFrameBuffer::Free()
{
    m_texture.Clear();

    if (g_GL.CanUseFrameBuffer && m_frameBuffer != 0)
    {
        glDeleteFramebuffers(1, &m_frameBuffer);
        m_frameBuffer = 0;
    }

    m_oldFrameBuffer = 0;
}

void CGLFrameBuffer::Release()
{
    if (g_GL.CanUseFrameBuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_oldFrameBuffer);

        glBindTexture(GL_TEXTURE_2D, m_texture.Texture);
        glGenerateMipmap(GL_TEXTURE_2D);

        g_GL.RestorePort();
    }
}

bool CGLFrameBuffer::Ready(int a_width, int a_height)
{
    return (
        g_GL.CanUseFrameBuffer && m_isReady && m_texture.Width == a_width &&
        m_texture.Height == a_height);
}

bool CGLFrameBuffer::ReadyMinSize(int a_width, int a_height)
{
    return (
        g_GL.CanUseFrameBuffer && m_isReady && m_texture.Width >= a_width &&
        m_texture.Height >= a_height);
}

bool CGLFrameBuffer::Use()
{
    bool result = false;

    if (g_GL.CanUseFrameBuffer && m_isReady)
    {
        glEnable(GL_TEXTURE_2D);

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFrameBuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
        glBindTexture(GL_TEXTURE_2D, m_texture.Texture);

        glViewport(0, 0, m_texture.Width, m_texture.Height);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        glOrtho(0.0, m_texture.Width, 0.0, m_texture.Height, -150.0, 150.0);

        glMatrixMode(GL_MODELVIEW);

        result = true;
    }

    return result;
}

void CGLFrameBuffer::Draw(int a_x, int a_y)
{
    if (g_GL.CanUseFrameBuffer && m_isReady)
    {
        g_GL.OldTexture = 0;
        g_GL.GL1_Draw(m_texture, a_x, a_y);
    }
}
