#pragma once

class CGLHTMLTextTexture : public CGLTextTexture
{
private:
    //Список ссылок
    std::deque<WEB_LINK_RECT> m_WebLinkRect;

public:
    CGLHTMLTextTexture();
    virtual ~CGLHTMLTextTexture();

    //Очистка веб-ссылки
    void ClearWebLink() { m_WebLinkRect.clear(); }

    //Добавиление веб-ссылки
    void AddWebLink(WEB_LINK_RECT &wl) { m_WebLinkRect.push_back(wl); }

    //Проверка веб-ссылки под мышкой
    u16 WebLinkUnderMouse(int x, int y);
};
