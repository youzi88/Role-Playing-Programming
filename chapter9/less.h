//
// •¶š—ñ”äŠrƒNƒ‰ƒX(‘å•¶š^¬•¶š‚ğ‹æ•Ê‚µ‚È‚¢)
//
//		Copyright (c) 2000-2001 Chihiro.SAKAMOTO (HyperWorks)
//
#ifndef	__less_h__
#define	__less_h__

class ic_less: public binary_function<const char *, const char *, bool> {
  public:
	result_type operator()(first_argument_type x, second_argument_type y) const
	{
		return stricmp(x, y) < 0;
	}
} ;

class ics_less: public binary_function<const string &, const string &, bool> {
  public:
	result_type operator()(first_argument_type x, second_argument_type y) const
	{
		return stricmp(x.c_str(), y.c_str()) < 0;
	}
} ;

#endif
