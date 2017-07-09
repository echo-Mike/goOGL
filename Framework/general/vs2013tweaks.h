#ifndef TWEAKS_H
#define TWEAKS_H "[multy@vs2013tweaks.h]"
/**
*	DESCRIPTION:
*		Module contains implementation of some features of C++11 and C++14
*		that not implemented in VS2013.
*	AUTHOR:
*		Mikhail Demchenko
*		mailto:dev.echo.mike@gmail.com
*		https://github.com/echo-Mike
**/

/**
*	noexcept statement not supported by vs2013
*	Taken from :
*		https://stackoverflow.com/questions/18387640/how-to-deal-with-noexcept-in-visual-studio
*	Comment by :
*		https://stackoverflow.com/users/1540501/ivan-ukr
**/
#if !defined(HAS_NOEXCEPT)
	#if defined(__clang__)
		#if __has_feature(cxx_noexcept)
			#define HAS_NOEXCEPT
		#endif
	#else
		#if defined(__GXX_EXPERIMENTAL_CXX0X__) && __GNUC__ * 10 + __GNUC_MINOR__ >= 46 || \
			defined(_MSC_FULL_VER) && _MSC_FULL_VER >= 190023026
			#define HAS_NOEXCEPT
		#endif
	#endif
#endif

#ifdef HAS_NOEXCEPT
	#define NOEXCEPT noexcept
#else
	#define NOEXCEPT
#endif

/**
*	constexpr statement not supported by vs2013
**/
#ifdef HAS_CONSTEXPR
	#define CONSTEXPR constexpr
#else
	#define CONSTEXPR
#endif // HAS_CONSTEXPR

#ifndef CONST_OR_CONSTEXPR
	#ifdef HAS_CONSTEXPR
		#define CONST_OR_CONSTEXPR CONSTEXPR
	#else
		#define CONST_OR_CONSTEXPR const
	#endif // HAS_CONSTEXPR
#endif // !CONST_OR_CONSTEXPR

/**
*	ref-qualifier semantics not supported by vs2013
*	MSDN :
*		https://msdn.microsoft.com/ru-ru/library/hh567368.aspx
*	Details :
*		http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2439.htm
**/
#ifdef HAS_REF_QUALIFIER
	#define RREFQ &&
	#define LREFQ &
#else
	#define RREFQ
	#define LREFQ
#endif // HAS_REF_QUALIFIER

/**
*	move-constructor and move-assign generation not supported by vs2013
*	Uncomment if it supported in you compiller.
**/
//#define MOVE_GENERATION

/**
*	decltype(auto) statment not supported by vc2013
*	Uncomment if it supported in you compiller.
**/
//#define HAS_DECLTYPE_AUTO
#endif