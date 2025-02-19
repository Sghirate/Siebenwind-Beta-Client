#include "GLEngine.h"
#include "Core/Log.h"
#include "Core/Minimal.h"
#include "GameWindow.h"
#include "GLEngine/GLTextTexture.h"
#include "Globals.h"
#include "Managers/GumpManager.h"
#include "GameObjects/LandObject.h"

CGLEngine g_GL;

BIND_TEXTURE_16_FUNCTION g_GL_BindTexture16_Ptr = &CGLEngine::GL1_BindTexture16;
BIND_TEXTURE_32_FUNCTION g_GL_BindTexture32_Ptr = &CGLEngine::GL1_BindTexture32;

DRAW_LAND_TEXTURE_FUNCTION g_GL_DrawLandTexture_Ptr    = &CGLEngine::GL1_DrawLandTexture;
DRAW_TEXTURE_FUNCTION g_GL_Draw_Ptr                    = &CGLEngine::GL1_Draw;
DRAW_TEXTURE_ROTATED_FUNCTION g_GL_DrawRotated_Ptr     = &CGLEngine::GL1_DrawRotated;
DRAW_TEXTURE_MIRRORED_FUNCTION g_GL_DrawMirrored_Ptr   = &CGLEngine::GL1_DrawMirrored;
DRAW_TEXTURE_SITTING_FUNCTION g_GL_DrawSitting_Ptr     = &CGLEngine::GL1_DrawSitting;
DRAW_TEXTURE_SHADOW_FUNCTION g_GL_DrawShadow_Ptr       = &CGLEngine::GL1_DrawShadow;
DRAW_TEXTURE_STRETCHED_FUNCTION g_GL_DrawStretched_Ptr = &CGLEngine::GL1_DrawStretched;
DRAW_TEXTURE_RESIZEPIC_FUNCTION g_GL_DrawResizepic_Ptr = &CGLEngine::GL1_DrawResizepic;

CGLEngine::CGLEngine()
{
}

CGLEngine::~CGLEngine()
{
    if (PositionBuffer != 0)
    {
        glDeleteBuffers(1, &PositionBuffer);
        PositionBuffer = 0;
    }

    Uninstall();
}

bool CGLEngine::GLSetupPixelFormat()
{
    return true;
}

bool CGLEngine::Install()
{
    OldTexture = -1;

    // TODO: FIX!
    m_context = SDL_GL_CreateContext(static_cast<SDL_Window*>(g_gameWindow.GetHandle()));
    SDL_GL_MakeCurrent(static_cast<SDL_Window*>(g_gameWindow.GetHandle()), m_context);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    // ~TODO: FIX!

    int glewInitResult = glewInit();
    LOG_INFO(
        "GLEngine",
        "glewInit() = %i fb=%i v(%s) (shader: %i)",
        glewInitResult,
        GL_ARB_framebuffer_object,
        glGetString(GL_VERSION),
        GL_ARB_shader_objects);
    if (glewInitResult != 0)
    {
        return false;
    }

    LOG_INFO("GLEngine", "Graphics Successfully Initialized");
    LOG_INFO("GLEngine", "OpenGL Info:");
    LOG_INFO("GLEngine", "    Version: %s", glGetString(GL_VERSION));
    LOG_INFO("GLEngine", "     Vendor: %s", glGetString(GL_VENDOR));
    LOG_INFO("GLEngine", "   Renderer: %s", glGetString(GL_RENDERER));
    LOG_INFO("GLEngine", "    Shading: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));

    CanUseFrameBuffer =
        (GL_ARB_framebuffer_object && glBindFramebuffer && glDeleteFramebuffers &&
         glFramebufferTexture2D && glGenFramebuffers);

    CanUseBuffer =
        (GL_VERSION_1_5 && glBindBuffer && glBufferData && glDeleteBuffers && glGenBuffers);

    CanUseBuffer = false;

    if (CanUseBuffer)
    {
        glGenBuffers(3, &PositionBuffer);

        int positionArray[] = { 0, 1, 1, 1, 0, 0, 1, 0 };

        glBindBuffer(GL_ARRAY_BUFFER, PositionBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(positionArray), &positionArray[0], GL_STATIC_DRAW);

        g_GL_BindTexture16_Ptr = &CGLEngine::GL2_BindTexture16;
        g_GL_BindTexture32_Ptr = &CGLEngine::GL2_BindTexture32;

        g_GL_DrawLandTexture_Ptr = &CGLEngine::GL2_DrawLandTexture;
        g_GL_Draw_Ptr            = &CGLEngine::GL2_Draw;
        g_GL_DrawRotated_Ptr     = &CGLEngine::GL2_DrawRotated;
        g_GL_DrawMirrored_Ptr    = &CGLEngine::GL2_DrawMirrored;
        g_GL_DrawSitting_Ptr     = &CGLEngine::GL2_DrawSitting;
        g_GL_DrawShadow_Ptr      = &CGLEngine::GL2_DrawShadow;
        g_GL_DrawStretched_Ptr   = &CGLEngine::GL2_DrawStretched;
        g_GL_DrawResizepic_Ptr   = &CGLEngine::GL2_DrawResizepic;
    }

    LOG_INFO(
        "GLEngine", "g_UseFrameBuffer = %i; CanUseBuffer = %i", CanUseFrameBuffer, CanUseBuffer);

    if (!CanUseFrameBuffer)
    {
        LOG_WARNING("GLEngine", "Your graphics card does not support Frame Buffers!");
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
    glShadeModel(GL_SMOOTH);              // Enables Smooth Color Shading
    glClearDepth(1.0);                    // Depth Buffer Setup
    glDisable(GL_DITHER);

    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);   //Realy Nice perspective calculations
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);

    glEnable(GL_TEXTURE_2D);

    SDL_GL_SetSwapInterval(0); // 1 vsync

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glClearStencil(0);
    glStencilMask(1);

    glEnable(GL_LIGHT0);

    GLfloat lightPosition[] = { -1.0f, -1.0f, 0.5f, 0.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, &lightPosition[0]);

    GLfloat lightAmbient[] = { 2.0f, 2.0f, 2.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, &lightAmbient[0]);

    GLfloat lav                  = 0.8f;
    GLfloat lightAmbientValues[] = { lav, lav, lav, lav };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &lightAmbientValues[0]);

    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

    const auto size = g_gameWindow.GetSize();
    ViewPort(0, 0, size.x, size.y);

    return true;
}

void CGLEngine::Uninstall()
{
    if (m_context != nullptr)
    {
        SDL_GL_DeleteContext(m_context);
    }
}

void CGLEngine::UpdateRect()
{
    int width, height;
    //SDL_GL_GetDrawableSize(g_gameWindow.m_window, &width, &height);
    SDL_GetWindowSize(static_cast<SDL_Window*>(g_gameWindow.GetHandle()), &width, &height);

    ViewPort(0, 0, width, height);
    //ViewPort(0, 0, g_gameWindow.GetSize().Width, g_gameWindow.GetSize().Height);

    g_GumpManager.RedrawAll();
}

void CGLEngine::GL1_BindTexture16(CGLTexture& texture, int width, int height, u16* pixels)
{
    GLuint tex = 0;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB5_A1,
        width,
        height,
        0,
        GL_BGRA,
        GL_UNSIGNED_SHORT_1_5_5_5_REV,
        pixels);

    texture.Width   = width;
    texture.Height  = height;
    texture.Texture = tex;
    if (IgnoreHitMap)
    {
        return;
    }

    Core::DynamicBitSet& hitMap = texture.m_hitMap;
    hitMap.Init(width * height);
    for (int i = 0; i < (width * height); ++i)
        hitMap.Set(i, pixels[i] != 0);
}

void CGLEngine::GL1_BindTexture32(CGLTexture& texture, int width, int height, u32* pixels)
{
    GLuint tex = 0;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA4, width, height, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8, pixels);

    texture.Width   = width;
    texture.Height  = height;
    texture.Texture = tex;

    if (IgnoreHitMap)
    {
        return;
    }

    Core::DynamicBitSet& hitMap = texture.m_hitMap;
    hitMap.Init(width * height);
    for (int i = 0; i < (width * height); ++i)
        hitMap.Set(i, pixels[i] != 0);
}

void CGLEngine::GL2_CreateArrays(CGLTexture& texture, int width, int height)
{
    GLuint vbo[2] = { 0 };
    glGenBuffers(2, &vbo[0]);

    int vertexArray[] = { 0, height, width, height, 0, 0, width, 0 };

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), &vertexArray[0], GL_STATIC_DRAW);

    int mirroredVertexArray[] = { width, height, 0, height, width, 0, 0, 0 };

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(mirroredVertexArray), &mirroredVertexArray[0], GL_STATIC_DRAW);

    texture.VertexBuffer         = vbo[0];
    texture.MirroredVertexBuffer = vbo[1];
}

void CGLEngine::GL2_BindTexture16(CGLTexture& texture, int width, int height, u16* pixels)
{
    GL1_BindTexture16(texture, width, height, pixels);
    GL2_CreateArrays(texture, width, height);
}

void CGLEngine::GL2_BindTexture32(CGLTexture& texture, int width, int height, u32* pixels)
{
    GL1_BindTexture32(texture, width, height, pixels);
    GL2_CreateArrays(texture, width, height);
}

void CGLEngine::BeginDraw()
{
    Drawing = true;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_BLEND);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.0f);

    if (CanUseBuffer)
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }
}

void CGLEngine::EndDraw()
{
    Drawing = false;

    if (CanUseBuffer)
    {
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    glDisable(GL_ALPHA_TEST);

    // TODO: FIX!
    SDL_GL_SwapWindow(static_cast<SDL_Window*>(g_gameWindow.GetHandle()));
    // ~TODO: FIX!
}

void CGLEngine::BeginStencil()
{
    glEnable(GL_STENCIL_TEST);

    glColorMask(0u, 0u, 0u, 0u);

    glStencilFunc(GL_ALWAYS, 1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}

void CGLEngine::EndStencil()
{
    glColorMask(1u, 1u, 1u, 1u);

    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_NOTEQUAL, 1, 1);

    glDisable(GL_STENCIL_TEST);
}

void CGLEngine::ViewPortScaled(int x, int y, int width, int height)
{
    glViewport(x, g_gameWindow.GetSize().y - y - height, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLdouble left   = (GLdouble)x;
    GLdouble right  = (GLdouble)(width + x);
    GLdouble top    = (GLdouble)y;
    GLdouble bottom = (GLdouble)(height + y);

    GLdouble newRight  = right * g_GlobalScale;
    GLdouble newBottom = bottom * g_GlobalScale;

    left = (left * g_GlobalScale) - (newRight - right);
    top  = (top * g_GlobalScale) - (newBottom - bottom);

    glOrtho(left, newRight, newBottom, top, -150.0, 150.0);
    glMatrixMode(GL_MODELVIEW);
}

void CGLEngine::ViewPort(int x, int y, int width, int height)
{
    const auto size = g_gameWindow.GetSize();
    glViewport(x, size.y - y - height, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(x, width + x, height + y, y, -150.0, 150.0);
    glMatrixMode(GL_MODELVIEW);
}

void CGLEngine::RestorePort()
{
    glViewport(0, 0, g_gameWindow.GetSize().x, g_gameWindow.GetSize().y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, g_gameWindow.GetSize().x, g_gameWindow.GetSize().y, 0.0, -150.0, 150.0);
    glMatrixMode(GL_MODELVIEW);
}

void CGLEngine::PushScissor(int x, int y, int width, int height)
{
    PushScissor(Core::Rect<i32>(x, y, width, height));
}

void CGLEngine::PushScissor(const Core::Vec2<i32>& position, int width, int height)
{
    PushScissor(Core::Rect<i32>(position, width, height));
}

void CGLEngine::PushScissor(int x, int y, const Core::Vec2<i32>& size)
{
    PushScissor(Core::Rect<i32>(x, y, size));
}

void CGLEngine::PushScissor(const Core::Vec2<i32>& position, const Core::Vec2<i32>& size)
{
    PushScissor(Core::Rect<i32>(position, size));
}

void CGLEngine::PushScissor(const Core::Rect<i32>& rect)
{
    m_ScissorList.push_back(rect);

    glEnable(GL_SCISSOR_TEST);

    glScissor(rect.pos.x, rect.pos.y, rect.size.x, rect.size.y);
}

void CGLEngine::PopScissor()
{
    if (!m_ScissorList.empty())
    {
        m_ScissorList.pop_back();
    }

    if (m_ScissorList.empty())
    {
        glDisable(GL_SCISSOR_TEST);
    }
    else
    {
        Core::Rect<i32>& rect = m_ScissorList.back();
        glScissor(rect.pos.x, rect.pos.y, rect.size.x, rect.size.y);
    }
}

void CGLEngine::ClearScissorList()
{
    m_ScissorList.clear();

    glDisable(GL_SCISSOR_TEST);
}

inline void CGLEngine::BindTexture(GLuint texture)
{
    if (OldTexture != texture)
    {
        OldTexture = texture;
        glBindTexture(GL_TEXTURE_2D, texture);
    }
}

void CGLEngine::DrawLine(int x, int y, int targetX, int targetY)
{
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_LINES);
    glVertex2i(x, y);
    glVertex2i(targetX, targetY);
    glEnd();

    glEnable(GL_TEXTURE_2D);
}

void CGLEngine::DrawPolygone(int x, int y, int width, int height)
{
    glDisable(GL_TEXTURE_2D);

    glTranslatef((GLfloat)x, (GLfloat)y, 0.0f);

    glBegin(GL_TRIANGLE_STRIP);
    glVertex2i(0, height);
    glVertex2i(width, height);
    glVertex2i(0, 0);
    glVertex2i(width, 0);
    glEnd();

    glTranslatef((GLfloat)-x, (GLfloat)-y, 0.0f);

    glEnable(GL_TEXTURE_2D);
}

void CGLEngine::DrawCircle(float x, float y, float radius, int gradientMode)
{
    glDisable(GL_TEXTURE_2D);

    glTranslatef(x, y, 0.0f);

    glBegin(GL_TRIANGLE_FAN);

    glVertex2i(0, 0);

    if (gradientMode != 0)
    {
        glColor4f(0.0f, 0.0f, 0.0f, 0.0f);
    }

    float pi = (float)M_PI * 2.0f;

    for (int i = 0; i <= 360; i++)
    {
        float a = (i / 180.0f) * pi;
        glVertex2f(cos(a) * radius, sin(a) * radius);
    }

    glEnd();

    glTranslatef(-x, -y, 0.0f);

    glEnable(GL_TEXTURE_2D);
}

void CGLEngine::GL1_DrawLandTexture(const CGLTexture& texture, int x, int y, CLandObject* land)
{
    BindTexture(texture.Texture);

    float translateX = x - 22.0f;
    float translateY = y - 22.0f;

    const auto& rc              = land->m_rect;
    Core::Vec3<double>* normals = land->m_Normals;

    glTranslatef(translateX, translateY, 0.0f);

    glBegin(GL_TRIANGLE_STRIP);
    glNormal3f((GLfloat)normals[0].x, (GLfloat)normals[0].y, (GLfloat)normals[0].z);
    glTexCoord2i(0, 0);
    glVertex2i(22, -rc.x); //^

    glNormal3f((GLfloat)normals[3].x, (GLfloat)normals[3].y, (GLfloat)normals[3].z);
    glTexCoord2i(0, 1);
    glVertex2i(0, 22 - rc.y); //<

    glNormal3f((GLfloat)normals[1].x, (GLfloat)normals[1].y, (GLfloat)normals[1].z);
    glTexCoord2i(1, 0);
    glVertex2i(44, 22 - rc.h); //>

    glNormal3f((GLfloat)normals[2].x, (GLfloat)normals[2].y, (GLfloat)normals[2].z);
    glTexCoord2i(1, 1);
    glVertex2i(22, 44 - rc.w); //v
    glEnd();

    glTranslatef(-translateX, -translateY, 0.0f);
}

void CGLEngine::GL1_Draw(const CGLTexture& texture, int x, int y)
{
    BindTexture(texture.Texture);

    int width  = texture.Width;
    int height = texture.Height;

    glTranslatef((GLfloat)x, (GLfloat)y, 0.0f);

    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2i(0, 1);
    glVertex2i(0, height);
    glTexCoord2i(1, 1);
    glVertex2i(width, height);
    glTexCoord2i(0, 0);
    glVertex2i(0, 0);
    glTexCoord2i(1, 0);
    glVertex2i(width, 0);
    glEnd();

    glTranslatef((GLfloat)-x, (GLfloat)-y, 0.0f);
}

void CGLEngine::GL1_DrawRotated(const CGLTexture& texture, int x, int y, float angle)
{
    BindTexture(texture.Texture);

    int width  = texture.Width;
    int height = texture.Height;

    GLfloat translateY = (GLfloat)(y - height);

    glTranslatef((GLfloat)x, translateY, 0.0f);

    glRotatef(angle, 0.0f, 0.0f, 1.0f);

    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2i(0, 1);
    glVertex2i(0, height);
    glTexCoord2i(1, 1);
    glVertex2i(width, height);
    glTexCoord2i(0, 0);
    glVertex2i(0, 0);
    glTexCoord2i(1, 0);
    glVertex2i(width, 0);
    glEnd();

    glRotatef(angle, 0.0f, 0.0f, -1.0f);
    glTranslatef((GLfloat)-x, -translateY, 0.0f);
}

void CGLEngine::GL1_DrawMirrored(const CGLTexture& texture, int x, int y, bool mirror)
{
    BindTexture(texture.Texture);

    int width  = texture.Width;
    int height = texture.Height;

    glTranslatef((GLfloat)x, (GLfloat)y, 0.0f);

    glBegin(GL_TRIANGLE_STRIP);

    if (mirror)
    {
        glTexCoord2i(0, 1);
        glVertex2i(width, height);
        glTexCoord2i(1, 1);
        glVertex2i(0, height);
        glTexCoord2i(0, 0);
        glVertex2i(width, 0);
        glTexCoord2i(1, 0);
        glVertex2i(0, 0);
    }
    else
    {
        glTexCoord2i(0, 1);
        glVertex2i(0, height);
        glTexCoord2i(1, 1);
        glVertex2i(width, height);
        glTexCoord2i(0, 0);
        glVertex2i(0, 0);
        glTexCoord2i(1, 0);
        glVertex2i(width, 0);
    }

    glEnd();

    glTranslatef((GLfloat)-x, (GLfloat)-y, 0.0f);
}

void CGLEngine::GL1_DrawSitting(
    const CGLTexture& texture, int x, int y, bool mirror, float h3mod, float h6mod, float h9mod)
{
    BindTexture(texture.Texture);

    glTranslatef((GLfloat)x, (GLfloat)y, 0.0f);

    float width  = (float)texture.Width;
    float height = (float)texture.Height;

    float h03 = height * h3mod;
    float h06 = height * h6mod;
    float h09 = height * h9mod;

    float widthOffset = (float)(width + SittingCharacterOffset);
    glBegin(GL_TRIANGLE_STRIP);

    if (mirror)
    {
        if (h3mod != 0.0f)
        {
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(width, 0);
            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(0, 0);
            glTexCoord2f(0.0f, h3mod);
            glVertex2f(width, h03);
            glTexCoord2f(1.0f, h3mod);
            glVertex2f(0, h03);
        }

        if (h6mod != 0.0f)
        {
            if (h3mod == 0.0f)
            {
                glTexCoord2f(0.0f, 0.0f);
                glVertex2f(width, 0);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2f(0, 0);
            }

            glTexCoord2f(0.0f, h6mod);
            glVertex2f(widthOffset, h06);
            glTexCoord2f(1.0f, h6mod);
            glVertex2f(SittingCharacterOffset, h06);
        }

        if (h9mod != 0.0f)
        {
            if (h6mod == 0.0f)
            {
                glTexCoord2f(0.0f, 0.0f);
                glVertex2f(widthOffset, 0);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2f(SittingCharacterOffset, 0);
            }

            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(widthOffset, h09);
            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(SittingCharacterOffset, h09);
        }
    }
    else
    {
        if (h3mod != 0.0f)
        {
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(SittingCharacterOffset, 0);
            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(widthOffset, 0);
            glTexCoord2f(0.0f, h3mod);
            glVertex2f(SittingCharacterOffset, h03);
            glTexCoord2f(1.0f, h3mod);
            glVertex2f(widthOffset, h03);
        }

        if (h6mod != 0.0f)
        {
            if (h3mod == 0.0f)
            {
                glTexCoord2f(0.0f, 0.0f);
                glVertex2f(SittingCharacterOffset, 0);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2f(width + SittingCharacterOffset, 0);
            }

            glTexCoord2f(0.0f, h6mod);
            glVertex2f(0, h06);
            glTexCoord2f(1.0f, h6mod);
            glVertex2f(width, h06);
        }

        if (h9mod != 0.0f)
        {
            if (h6mod == 0.0f)
            {
                glTexCoord2f(0.0f, 0.0f);
                glVertex2f(0, 0);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2f(width, 0);
            }

            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(0, h09);
            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(width, h09);
        }
    }

    glEnd();

    glTranslatef((GLfloat)-x, (GLfloat)-y, 0.0f);
}

void CGLEngine::GL1_DrawShadow(const CGLTexture& texture, int x, int y, bool mirror)
{
    BindTexture(texture.Texture);

    float width  = (float)texture.Width;
    float height = texture.Height / 2.0f;

    GLfloat translateY = (GLfloat)(y + height * 0.75);

    glTranslatef((GLfloat)x, translateY, 0.0f);

    glBegin(GL_TRIANGLE_STRIP);

    float ratio = height / width;

    if (mirror)
    {
        glTexCoord2f(0, 1);
        glVertex2f(width, height);
        glTexCoord2f(1, 1);
        glVertex2f(0, height);
        glTexCoord2f(0, 0);
        glVertex2f(width * (ratio + 1.0f), 0);
        glTexCoord2f(1, 0);
        glVertex2f(width * ratio, 0);
    }
    else
    {
        glTexCoord2f(0, 1);
        glVertex2f(0, height);
        glTexCoord2f(1, 1);
        glVertex2f(width, height);
        glTexCoord2f(0, 0);
        glVertex2f(width * ratio, 0);
        glTexCoord2f(1, 0);
        glVertex2f(width * (ratio + 1.0f), 0);
    }

    glEnd();

    glTranslatef((GLfloat)-x, -translateY, 0.0f);
}

void CGLEngine::GL1_DrawStretched(
    const CGLTexture& texture, int x, int y, int drawWidth, int drawHeight)
{
    BindTexture(texture.Texture);

    int width  = texture.Width;
    int height = texture.Height;

    glTranslatef((GLfloat)x, (GLfloat)y, 0.0f);

    float drawCountX = drawWidth / (float)width;
    float drawCountY = drawHeight / (float)height;

    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0.0f, drawCountY);
    glVertex2i(0, drawHeight);
    glTexCoord2f(drawCountX, drawCountY);
    glVertex2i(drawWidth, drawHeight);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2i(0, 0);
    glTexCoord2f(drawCountX, 0.0f);
    glVertex2i(drawWidth, 0);
    glEnd();

    glTranslatef((GLfloat)-x, (GLfloat)-y, 0.0f);
}

void CGLEngine::GL1_DrawResizepic(CGLTexture** th, int x, int y, int width, int height)
{
    int offsetTop    = Core::Max(th[0]->Height, th[2]->Height) - th[1]->Height;
    int offsetBottom = Core::Max(th[5]->Height, th[7]->Height) - th[6]->Height;
    int offsetLeft   = Core::Max(th[0]->Width, th[5]->Width) - th[3]->Width;
    int offsetRight  = Core::Max(th[2]->Width, th[7]->Width) - th[4]->Width;

    for (int i = 0; i < 9; i++)
    {
        BindTexture(th[i]->Texture);

        int drawWidth    = th[i]->Width;
        int drawHeight   = th[i]->Height;
        float drawCountX = 1.0f;
        float drawCountY = 1.0f;
        int drawX        = x;
        int drawY        = y;

        switch (i)
        {
            case 1:
            {
                drawX += th[0]->Width;

                drawWidth = width - th[0]->Width - th[2]->Width;

                drawCountX = drawWidth / (float)th[i]->Width;

                break;
            }
            case 2:
            {
                drawX += width - drawWidth;
                drawY += offsetTop;

                break;
            }
            case 3:
            {
                drawY += th[0]->Height;
                drawX += offsetLeft;

                drawHeight = height - th[0]->Height - th[5]->Height;

                drawCountY = drawHeight / (float)th[i]->Height;

                break;
            }
            case 4:
            {
                drawX += width - drawWidth - offsetRight;
                drawY += th[2]->Height;

                drawHeight = height - th[2]->Height - th[7]->Height;

                drawCountY = drawHeight / (float)th[i]->Height;

                break;
            }
            case 5:
            {
                drawY += height - drawHeight;

                break;
            }
            case 6:
            {
                drawX += th[5]->Width;
                drawY += height - drawHeight - offsetBottom;

                drawWidth = width - th[5]->Width - th[7]->Width;

                drawCountX = drawWidth / (float)th[i]->Width;

                break;
            }
            case 7:
            {
                drawX += width - drawWidth;
                drawY += height - drawHeight;

                break;
            }
            case 8:
            {
                drawX += th[0]->Width;
                drawY += th[0]->Height;

                drawWidth = width - th[0]->Width - th[2]->Width;

                drawHeight = height - th[2]->Height - th[7]->Height;

                drawCountX = drawWidth / (float)th[i]->Width;
                drawCountY = drawHeight / (float)th[i]->Height;

                break;
            }
            default: break;
        }

        if (drawWidth < 1 || drawHeight < 1)
        {
            continue;
        }

        glTranslatef((GLfloat)drawX, (GLfloat)drawY, 0.0f);

        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, drawCountY);
        glVertex2i(0, drawHeight);
        glTexCoord2f(drawCountX, drawCountY);
        glVertex2i(drawWidth, drawHeight);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(0, 0);
        glTexCoord2f(drawCountX, 0.0f);
        glVertex2i(drawWidth, 0);
        glEnd();

        glTranslatef((GLfloat)-drawX, (GLfloat)-drawY, 0.0f);
    }
}

void CGLEngine::GL2_DrawLandTexture(const CGLTexture& texture, int x, int y, CLandObject* land)
{
    BindTexture(texture.Texture);

    float translateX = x - 22.0f;
    float translateY = y - 22.0f;

    glTranslatef(translateX, translateY, 0.0f);

    glBindBuffer(GL_ARRAY_BUFFER, land->VertexBuffer);
    glVertexPointer(2, GL_INT, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, land->PositionBuffer);
    glTexCoordPointer(2, GL_INT, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, land->NormalBuffer);
    glNormalPointer(GL_FLOAT, 0, (void*)0);

    glEnableClientState(GL_NORMAL_ARRAY);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glDisableClientState(GL_NORMAL_ARRAY);

    glTranslatef(-translateX, -translateY, 0.0f);
}

void CGLEngine::GL2_Draw(const CGLTexture& texture, int x, int y)
{
    BindTexture(texture.Texture);

    int width  = texture.Width;
    int height = texture.Height;

    glTranslatef((GLfloat)x, (GLfloat)y, 0.0f);

    glBindBuffer(GL_ARRAY_BUFFER, texture.VertexBuffer);
    glVertexPointer(2, GL_INT, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, PositionBuffer);
    glTexCoordPointer(2, GL_INT, 0, (void*)0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glTranslatef((GLfloat)-x, (GLfloat)-y, 0.0f);
}

void CGLEngine::GL2_DrawRotated(const CGLTexture& texture, int x, int y, float angle)
{
    BindTexture(texture.Texture);

    int width  = texture.Width;
    int height = texture.Height;

    GLfloat translateY = (GLfloat)(y - height);

    glTranslatef((GLfloat)x, translateY, 0.0f);

    glRotatef(angle, 0.0f, 0.0f, 1.0f);

    glBindBuffer(GL_ARRAY_BUFFER, texture.VertexBuffer);
    glVertexPointer(2, GL_INT, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, PositionBuffer);
    glTexCoordPointer(2, GL_INT, 0, (void*)0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glRotatef(angle, 0.0f, 0.0f, -1.0f);
    glTranslatef((GLfloat)-x, -translateY, 0.0f);
}

void CGLEngine::GL2_DrawMirrored(const CGLTexture& texture, int x, int y, bool mirror)
{
    BindTexture(texture.Texture);

    int width  = texture.Width;
    int height = texture.Height;

    glTranslatef((GLfloat)x, (GLfloat)y, 0.0f);

    if (mirror)
    {
        glBindBuffer(GL_ARRAY_BUFFER, texture.MirroredVertexBuffer);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, texture.VertexBuffer);
    }

    glVertexPointer(2, GL_INT, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, PositionBuffer);
    glTexCoordPointer(2, GL_INT, 0, (void*)0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glTranslatef((GLfloat)-x, (GLfloat)-y, 0.0f);
}

void CGLEngine::GL2_DrawSitting(
    const CGLTexture& texture, int x, int y, bool mirror, float h3mod, float h6mod, float h9mod)
{
    BindTexture(texture.Texture);

    glTranslatef((GLfloat)x, (GLfloat)y, 0.0f);

    float width  = (float)texture.Width;
    float height = (float)texture.Height;

    float h03 = height * h3mod;
    float h06 = height * h6mod;
    float h09 = height * h9mod;

    float widthOffset = (float)(width + SittingCharacterOffset);
    glBegin(GL_TRIANGLE_STRIP);

    if (mirror)
    {
        if (h3mod != 0.0f)
        {
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(width, 0);
            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(0, 0);
            glTexCoord2f(0.0f, h3mod);
            glVertex2f(width, h03);
            glTexCoord2f(1.0f, h3mod);
            glVertex2f(0, h03);
        }

        if (h6mod != 0.0f)
        {
            if (h3mod == 0.0f)
            {
                glTexCoord2f(0.0f, 0.0f);
                glVertex2f(width, 0);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2f(0, 0);
            }

            glTexCoord2f(0.0f, h6mod);
            glVertex2f(widthOffset, h06);
            glTexCoord2f(1.0f, h6mod);
            glVertex2f(SittingCharacterOffset, h06);
        }

        if (h9mod != 0.0f)
        {
            if (h6mod == 0.0f)
            {
                glTexCoord2f(0.0f, 0.0f);
                glVertex2f(widthOffset, 0);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2f(SittingCharacterOffset, 0);
            }

            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(widthOffset, h09);
            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(SittingCharacterOffset, h09);
        }
    }
    else
    {
        if (h3mod != 0.0f)
        {
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(SittingCharacterOffset, 0);
            glTexCoord2f(1.0f, 0.0f);
            glVertex2f(widthOffset, 0);
            glTexCoord2f(0.0f, h3mod);
            glVertex2f(SittingCharacterOffset, h03);
            glTexCoord2f(1.0f, h3mod);
            glVertex2f(widthOffset, h03);
        }

        if (h6mod != 0.0f)
        {
            if (h3mod == 0.0f)
            {
                glTexCoord2f(0.0f, 0.0f);
                glVertex2f(SittingCharacterOffset, 0);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2f(width + SittingCharacterOffset, 0);
            }

            glTexCoord2f(0.0f, h6mod);
            glVertex2f(0, h06);
            glTexCoord2f(1.0f, h6mod);
            glVertex2f(width, h06);
        }

        if (h9mod != 0.0f)
        {
            if (h6mod == 0.0f)
            {
                glTexCoord2f(0.0f, 0.0f);
                glVertex2f(0, 0);
                glTexCoord2f(1.0f, 0.0f);
                glVertex2f(width, 0);
            }

            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(0, h09);
            glTexCoord2f(1.0f, 1.0f);
            glVertex2f(width, h09);
        }
    }

    glEnd();

    glTranslatef((GLfloat)-x, (GLfloat)-y, 0.0f);
}

void CGLEngine::GL2_DrawShadow(const CGLTexture& texture, int x, int y, bool mirror)
{
    BindTexture(texture.Texture);

    float width  = (float)texture.Width;
    float height = texture.Height / 2.0f;

    GLfloat translateY = (GLfloat)(y + height * 0.75);

    glTranslatef((GLfloat)x, translateY, 0.0f);

    float ratio = height / width;
    float verticles[8];

    if (mirror)
    {
        verticles[0] = width;
        verticles[1] = height;
        verticles[2] = 0.0f;
        verticles[3] = height;
        verticles[4] = width * (ratio + 1.0f);
        verticles[5] = 0.0f;
        verticles[6] = width * ratio;
        verticles[7] = 0.0f;
    }
    else
    {
        verticles[0] = 0.0f;
        verticles[1] = height;
        verticles[2] = width;
        verticles[3] = height;
        verticles[4] = width * ratio;
        verticles[5] = 0.0f;
        verticles[6] = width * (ratio + 1.0f);
        verticles[7] = 0.0f;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexPointer(2, GL_FLOAT, 0, &verticles[0]);

    glBindBuffer(GL_ARRAY_BUFFER, PositionBuffer);
    glTexCoordPointer(2, GL_INT, 0, (void*)0);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glTranslatef((GLfloat)-x, -translateY, 0.0f);
}

void CGLEngine::GL2_DrawStretched(
    const CGLTexture& texture, int x, int y, int drawWidth, int drawHeight)
{
    BindTexture(texture.Texture);

    int width  = texture.Width;
    int height = texture.Height;

    glTranslatef((GLfloat)x, (GLfloat)y, 0.0f);

    float drawCountX = drawWidth / (float)width;
    float drawCountY = drawHeight / (float)height;

    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0.0f, drawCountY);
    glVertex2i(0, drawHeight);
    glTexCoord2f(drawCountX, drawCountY);
    glVertex2i(drawWidth, drawHeight);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2i(0, 0);
    glTexCoord2f(drawCountX, 0.0f);
    glVertex2i(drawWidth, 0);
    glEnd();

    glTranslatef((GLfloat)-x, (GLfloat)-y, 0.0f);
}

void CGLEngine::GL2_DrawResizepic(CGLTexture** th, int x, int y, int width, int height)
{
    for (int i = 0; i < 9; i++)
    {
        BindTexture(th[i]->Texture);

        int drawWidth    = th[i]->Width;
        int drawHeight   = th[i]->Height;
        float drawCountX = 1.0f;
        float drawCountY = 1.0f;
        int drawX        = x;
        int drawY        = y;

        switch (i)
        {
            case 1:
            {
                drawX += th[0]->Width;

                drawWidth = width - th[0]->Width - th[2]->Width;

                drawCountX = drawWidth / (float)th[i]->Width;

                break;
            }
            case 2:
            {
                drawX += width - drawWidth;

                break;
            }
            case 3:
            {
                drawY += th[0]->Height;

                drawHeight = height - th[0]->Height - th[5]->Height;

                drawCountY = drawHeight / (float)th[i]->Height;

                break;
            }
            case 4:
            {
                drawX += width - drawWidth;
                drawY += th[2]->Height;

                drawHeight = height - th[2]->Height - th[7]->Height;

                drawCountY = drawHeight / (float)th[i]->Height;

                break;
            }
            case 5:
            {
                drawY += height - drawHeight;

                break;
            }
            case 6:
            {
                drawX += th[5]->Width;
                drawY += height - drawHeight;

                drawWidth = width - th[5]->Width - th[7]->Width;

                drawCountX = drawWidth / (float)th[i]->Width;

                break;
            }
            case 7:
            {
                drawX += width - drawWidth;
                drawY += height - drawHeight;

                break;
            }
            case 8:
            {
                drawX += th[0]->Width;
                drawY += th[0]->Height;

                drawWidth = width - th[0]->Width - th[2]->Width;

                drawHeight = height - th[2]->Height - th[7]->Height;

                drawCountX = drawWidth / (float)th[i]->Width;
                drawCountY = drawHeight / (float)th[i]->Height;

                break;
            }
            default: break;
        }

        if (drawWidth < 1 || drawHeight < 1)
        {
            continue;
        }

        glTranslatef((GLfloat)drawX, (GLfloat)drawY, 0.0f);

        glBegin(GL_TRIANGLE_STRIP);
        glTexCoord2f(0.0f, drawCountY);
        glVertex2i(0, drawHeight);
        glTexCoord2f(drawCountX, drawCountY);
        glVertex2i(drawWidth, drawHeight);
        glTexCoord2f(0.0f, 0.0f);
        glVertex2i(0, 0);
        glTexCoord2f(drawCountX, 0.0f);
        glVertex2i(drawWidth, 0);
        glEnd();

        glTranslatef((GLfloat)-drawX, (GLfloat)-drawY, 0.0f);
    }
}
