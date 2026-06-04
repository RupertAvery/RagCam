#ifndef GLAUX_COMPAT_H
#define GLAUX_COMPAT_H

// Drop-in replacement for the deprecated glaux AUX_RGBImageRec / auxDIBImageLoad.
// Uses Windows GDI to load a BMP and returns tightly-packed 24-bit BGR data,
// matching the layout that the original glaux returned.

#include <windows.h>
#include <gl\gl.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    GLsizei  sizeX;
    GLsizei  sizeY;
    GLubyte *data;
} AUX_RGBImageRec;

static AUX_RGBImageRec *auxDIBImageLoad(const char *filename)
{
    HBITMAP hBmp = (HBITMAP)LoadImageA(NULL, filename, IMAGE_BITMAP, 0, 0,
                                       LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (!hBmp) return NULL;

    BITMAP bm;
    GetObject(hBmp, sizeof(bm), &bm);

    int width  = bm.bmWidth;
    int height = (bm.bmHeight < 0) ? -bm.bmHeight : bm.bmHeight;
    int stride = (width * 3 + 3) & ~3;  // GetDIBits pads rows to DWORD boundary
    int packed = width * 3;              // glaux returns tightly packed rows

    BITMAPINFOHEADER bi = {0};
    bi.biSize        = sizeof(bi);
    bi.biWidth       = width;
    bi.biHeight      = height;           // positive = bottom-up, matches glaux
    bi.biPlanes      = 1;
    bi.biBitCount    = 24;
    bi.biCompression = BI_RGB;

    GLubyte *raw = (GLubyte *)malloc(stride * height);
    if (!raw) { DeleteObject(hBmp); return NULL; }

    HDC hdc = GetDC(NULL);
    GetDIBits(hdc, hBmp, 0, height, raw, (BITMAPINFO *)&bi, DIB_RGB_COLORS);
    ReleaseDC(NULL, hdc);
    DeleteObject(hBmp);

    // Strip DWORD-alignment padding so callers can index as data[3*(x + y*w)+c]
    GLubyte *data = (GLubyte *)malloc(packed * height);
    if (!data) { free(raw); return NULL; }
    for (int y = 0; y < height; y++)
        memcpy(data + y * packed, raw + y * stride, packed);
    free(raw);

    AUX_RGBImageRec *rec = (AUX_RGBImageRec *)malloc(sizeof(AUX_RGBImageRec));
    if (!rec) { free(data); return NULL; }
    rec->sizeX = width;
    rec->sizeY = height;
    rec->data  = data;
    return rec;
}

#endif // GLAUX_COMPAT_H
