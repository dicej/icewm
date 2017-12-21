#ifndef __YPAINT_H
#define __YPAINT_H

#include "ref.h"
#include "ypixmap.h"
#include "yimage.h"
#include "mstring.h"

#ifdef CONFIG_SHAPE
#define __YIMP_XUTIL__
#include <X11/extensions/shape.h>
#endif

#ifdef CONFIG_XRANDR
#include <X11/extensions/Xrandr.h>
#endif

#ifdef CONFIG_XFREETYPE //------------------------------------------------------
#include <ft2build.h>
#include <X11/Xft/Xft.h>
#define INIT_XFREETYPE(Member, Value) , Member(Value)
#else
#define INIT_XFREETYPE(Member, Value)
#endif // CONFIG_XFREETYPE -----------------------------------------------------

class YWindow;
class YIcon;

enum YDirection {
    Up, Left, Down, Right
};

/******************************************************************************/
/******************************************************************************/

class YColor {
public:
    YColor(unsigned short red, unsigned short green, unsigned short blue);
    YColor(unsigned long pixel);
    YColor(char const * clr);
    ~YColor();

    unsigned long pixel() const { return fPixel; }

    YColor * darker();
    YColor * brighter();

    static YColor * black;
    static YColor * white;

    bool operator==(const YColor& c) const {
        return fRed == c.fRed && fGreen == c.fGreen && fBlue == c.fBlue;
    }
    bool operator!=(const YColor& c) const {
        return !(*this == c);
    }

#ifdef CONFIG_XFREETYPE
    operator XftColor*() { return xftColor ? xftColor : allocXft(); }
private:
    XftColor* allocXft();
#endif

private:
    YColor(const YColor&);
    YColor& operator=(const YColor&);

    void alloc();

    unsigned long fPixel;
    unsigned short fRed;
    unsigned short fGreen;
    unsigned short fBlue;
    YColor * fDarker; //!!! remove this (needs color caching...)
    YColor * fBrighter; //!!! removethis

#ifdef CONFIG_XFREETYPE
    XftColor * xftColor;
#endif
};

struct YDimension {
    YDimension(unsigned w, unsigned h): w(w), h(h) {}
    unsigned w, h;
};

/******************************************************************************/
/******************************************************************************/

class YFont: public virtual refcounted {
public:
    static ref<YFont> getFont(ustring name, ustring xftFont, bool antialias = true);

    virtual ~YFont() {}

    virtual bool valid() const = 0;
    virtual int height() const { return ascent() + descent(); }
    virtual int descent() const = 0;
    virtual int ascent() const = 0;
    virtual int textWidth(const ustring &s) const = 0;
    virtual int textWidth(char const * str, int len) const = 0;

    virtual void drawGlyphs(class Graphics & graphics, int x, int y,
                            char const * str, int len) = 0;

    int textWidth(char const * str) const;
    int multilineTabPos(char const * str) const;
    YDimension multilineAlloc(char const * str) const;
    YDimension multilineAlloc(const ustring &str) const;
};

/******************************************************************************/
/******************************************************************************/

struct YSurface {
    YSurface(class YColor * color, ref<YPixmap> pixmap,
             ref<YImage> gradient):
    color(color), pixmap(pixmap), gradient(gradient) {}

    class YColor * color;
    ref<YPixmap> pixmap;
    ref<YImage> gradient;
};

class Graphics {
public:
    Graphics(YWindow & window, unsigned long vmask, XGCValues * gcv);
    Graphics(YWindow & window);
    Graphics(const ref<YPixmap> &pixmap, int x_org, int y_org);
    Graphics(Drawable drawable, unsigned w, unsigned h, unsigned depth, unsigned long vmask, XGCValues * gcv);
    Graphics(Drawable drawable, unsigned w, unsigned h, unsigned depth);
    ~Graphics();

    void copyArea(const int x, const int y, const unsigned width, const unsigned height,
                  const int dx, const int dy);
    void copyDrawable(const Drawable d, const int x, const int y,
                      const unsigned w, const unsigned h, const int dx, const int dy);
#if 0
    void copyImage(XImage * im, const int x, const int y,
                   const unsigned w, const int unsigned, const int dx, const int dy);
    void copyImage(XImage * im, const int x, const int y) {
        copyImage(im, 0, 0, im->width, im->height, x, y);
    }
#endif
    void copyPixmap(const ref<YPixmap> &p, const int x, const int y,
                     const unsigned w, const unsigned h, const int dx, const int dy);

    void drawPoint(int x, int y);
    void drawLine(int x1, int y1, int x2, int y2);
    void drawLines(XPoint * points, int n, int mode = CoordModeOrigin);
    void drawSegments(XSegment * segments, int n);
    void drawRect(int x, int y, unsigned width, unsigned height);
    void drawRects(XRectangle * rects, unsigned n);
    void drawArc(int x, int y, unsigned width, unsigned height, int a1, int a2);
    void drawArrow(YDirection direction, int x, int y, unsigned size, bool pressed = false);

    void drawChars(const ustring &s, int x, int y);
    void drawChars(char const * data, int offset, int len, int x, int y);
    void drawCharUnderline(int x, int y, char const * str, int charPos);

    void drawCharUnderline(int x, int y, const ustring &str, int charPos);

    void drawString(int x, int y, char const * str);
    void drawStringEllipsis(int x, int y, char const * str, int maxWidth);
    void drawStringEllipsis(int x, int y, const ustring &str, int maxWidth);
    void drawStringMultiline(int x, int y, char const * str);
    void drawStringMultiline(int x, int y, const ustring &str);

    void drawPixmap(ref<YPixmap> pix, int const x, int const y);
    void drawPixmap(ref<YPixmap> pix, int const x, int const y, unsigned w, unsigned h, int dx, int dy);
    void drawImage(ref<YImage> pix, int const x, int const y);
    void drawImage(ref<YImage> pix, int const x, int const y, unsigned w, unsigned h, int dx, int dy);
    void compositeImage(ref<YImage> pix, int const x, int const y, unsigned w, unsigned h, int dx, int dy);
    void drawMask(ref<YPixmap> pix, int const x, int const y);
    void drawClippedPixmap(Pixmap pix, Pixmap clip,
                           int x, int y, unsigned w, unsigned h, int toX, int toY);
    void fillRect(int x, int y, unsigned width, unsigned height);
    void fillRects(XRectangle * rects, int n);
    void fillPolygon(XPoint * points, int const n, int const shape,
                     int const mode);
    void fillArc(int x, int y, unsigned width, unsigned height, int a1, int a2);
    void setColor(YColor * aColor);
    void setColorPixel(unsigned long pixel);
    void setFont(ref<YFont> aFont);
    void setThinLines(void) { setLineWidth(0); }
    void setWideLines(unsigned width = 1) { setLineWidth(width >= 1 ? width : 1); }
    void setLineWidth(unsigned width);
    void setPenStyle(bool dotLine = false); ///!!!hack
    void setFunction(int function = GXcopy);

    void draw3DRect(int x, int y, unsigned w, unsigned h, bool raised);
    void drawBorderW(int x, int y, unsigned w, unsigned h, bool raised);
    void drawBorderM(int x, int y, unsigned w, unsigned h, bool raised);
    void drawBorderG(int x, int y, unsigned w, unsigned h, bool raised);
    void drawBorderS(int x, int y, int w, int h, YColor* color);
    void drawCenteredPixmap(int x, int y, unsigned w, unsigned h, ref<YPixmap> pixmap);
    void drawOutline(int l, int t, int r, int b, unsigned iw, unsigned ih);
    void repHorz(Drawable drawable, unsigned pw, unsigned ph, int x, int y, unsigned w);
    void repVert(Drawable drawable, unsigned pw, unsigned ph, int x, int y, unsigned h);
    void fillPixmap(const ref<YPixmap> &pixmap, int x, int y, unsigned w, unsigned h,
                    int sx = 0, int sy = 0);

    void drawSurface(YSurface const & surface, int x, int y, unsigned w, unsigned h,
                     int const sx, int const sy, const unsigned sw, const unsigned sh);
    void drawSurface(YSurface const & surface, int x, int y, unsigned w, unsigned h) {
        drawSurface(surface, x, y, w, h, 0, 0, w, h);
    }

    void drawGradient(ref<YImage> gradient,
                      int const x, int const y, const unsigned w, const unsigned h,
                      int const gx, int const gy, const unsigned gw, const unsigned gh);
    void drawGradient(ref<YImage> gradient,
                      int const x, int const y, const unsigned w, const unsigned h) {
        drawGradient(gradient, x, y, w, h, 0, 0, w, h);
    }

    void repHorz(ref<YPixmap> p, int x, int y, unsigned w) {
        if (p != null)
            repHorz(p->pixmap(), p->width(), p->height(), x, y, w);
    }
    void repVert(ref<YPixmap> p, int x, int y, unsigned h) {
        if (p != null)
            repVert(p->pixmap(), p->width(), p->height(), x, y, h);
    }

    Drawable drawable() const { return fDrawable; }
    GC handleX() const { return gc; }
#ifdef CONFIG_XFREETYPE
    XftDraw *handleXft();
#endif

    YColor * color() const { return fColor; }
    ref<YFont> font() const { return fFont; }
    int function() const;
    int xorigin() const { return xOrigin; }
    int yorigin() const { return yOrigin; }
    unsigned rwidth() const { return rWidth; }
    unsigned rheight() const { return rHeight; }
    unsigned rdepth() const { return rDepth; }

    void setClipRectangles(XRectangle *rect, int count);
    void setClipMask(Pixmap mask = None);
    void resetClip();
private:
    Drawable fDrawable;
    GC gc;
#ifdef CONFIG_XFREETYPE
    XftDraw* fXftDraw;
#endif

    YColor * fColor;
    ref<YFont> fFont;
    int xOrigin, yOrigin;
    unsigned rWidth, rHeight, rDepth;
};

#endif

// vim: set sw=4 ts=4 et:
