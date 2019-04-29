//
// 俯瞰式地图的座标转换
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__coordinate_h__
#define	__coordinate_h__

enum	{
	MAPGRID_WIDTH = 64,
	MAPGRID_HEIGHT = 32,
} ;

inline CPoint PointToIndex(int x, int y)
{
	y = (y - MAPGRID_HEIGHT * 13 / 2) * 2;
#ifdef	notdef		// 当使用Shift时
	return CPoint((x - y) >> 6, (x + y) >> 6);
#else
	return CPoint((x - y + MAPGRID_WIDTH * 10) / MAPGRID_WIDTH - 10,
		(x + y + MAPGRID_WIDTH * 10) / MAPGRID_WIDTH - 10);
#endif
}

inline CPoint PointToIndex(CPoint point)
{
	return PointToIndex(point.x, point.y);
}

inline CPoint IndexToPoint(int x, int y)
{
	return CPoint((x + y) * (MAPGRID_WIDTH / 2),
		(12 - x + y) * (MAPGRID_HEIGHT / 2));
}

inline CPoint IndexToPoint(CPoint point)
{
	return IndexToPoint(point.x, point.y);
}

#endif
