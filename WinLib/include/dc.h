//
// Framework (DC 的 Wrapper)
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__dc_h__
#define	__dc_h__

class CWindow;

//
//	CDC Class
//
class CDC {
  public:
	CDC(): hDC(0) {}
	CDC(HDC dc): hDC(dc) {}
	virtual ~CDC();

	operator HDC() const { return hDC; }

	int GetDIBits(HBITMAP hbmp, UINT uStartScan, UINT cScanLines, LPVOID lpvBits,
		LPBITMAPINFO lpbi, UINT uUsage);
	BOOL GetTextExtentPoint32(LPCTSTR lpsz, int cbString, LPSIZE lpSize);
	BOOL ExtTextOut(int X, int Y, UINT fuOptions, CONST RECT *lprc,
		LPCTSTR lpszString, UINT cbCount, CONST INT *lpDx=NULL);
	COLORREF SetTextColor(COLORREF crColor);
	COLORREF SetBkColor(COLORREF crColor);
	int SetBkMode(int bkMode);
	int GetDeviceCaps(int nIndex);
	HBITMAP	SelectObject(HBITMAP hBitmap);
	HFONT SelectObject(HFONT hFont);

  protected:
	HDC	hDC;
} ;

// inline成員函式

inline int CDC::GetDIBits(HBITMAP hbmp, UINT uStartScan, UINT cScanLines,
	LPVOID lpvBits, LPBITMAPINFO lpbi, UINT uUsage)
{
	return ::GetDIBits(hDC, hbmp, uStartScan, cScanLines, lpvBits, lpbi, uUsage);
}

inline BOOL CDC::GetTextExtentPoint32(LPCTSTR lpsz, int cbString, LPSIZE lpSize)
{
	return ::GetTextExtentPoint32(hDC, lpsz, cbString, lpSize);
}

inline BOOL CDC::ExtTextOut(int X, int Y, UINT fuOptions, CONST RECT *lprc,
	LPCTSTR lpszString, UINT cbCount, CONST INT *lpDx)
{
	return ::ExtTextOut(hDC, X, Y, fuOptions, lprc, lpszString, cbCount, lpDx);
}

inline COLORREF CDC::SetTextColor(COLORREF crColor)
{
	return ::SetTextColor(hDC, crColor);
}

inline COLORREF CDC::SetBkColor(COLORREF crColor)
{
	return ::SetBkColor(hDC, crColor);
}

inline int CDC::SetBkMode(int bkMode)
{
	return ::SetBkMode(hDC, bkMode);
}

inline int CDC::GetDeviceCaps(int nIndex)
{
	return ::GetDeviceCaps(hDC, nIndex);
}

inline HBITMAP CDC::SelectObject(HBITMAP hBitmap)
{
	return (HBITMAP)::SelectObject(hDC, hBitmap);
}

inline HFONT CDC::SelectObject(HFONT hFont)
{
	return (HFONT)::SelectObject(hDC, hFont);
}

//
//	CClientDC class(GetDC的wrapper)
//
class CClientDC: public CDC {
  public:
	CClientDC(CWindow *win);
	~CClientDC();

  protected:
	HWND hWnd;
} ;

//
//	CPaintDC class (BeginPaint的wrapper)
//
class CPaintDC: public CDC {
  public:
	CPaintDC(CWindow *win);
	~CPaintDC();

  public:
	PAINTSTRUCT ps;

  protected:
	HWND hWnd;
} ;

//
//	CMemoryDC class(CreateCompatibleDC的wrapper)
//
class CMemoryDC: public CDC {
  public:
	CMemoryDC(HDC hDC);
	~CMemoryDC();
} ;

//
//	CScreenDC class
//
class CScreenDC: public CDC {
  public:
	CScreenDC();
	~CScreenDC();
} ;

#endif
