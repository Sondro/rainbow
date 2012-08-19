/// Platform specific definitions. Makes sure correct code is being compiled.
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

#ifndef PLATFORM_H_
#define PLATFORM_H_

#if defined(__ANDROID__)
#	define RAINBOW_ANDROID
#elif defined(__APPLE__)
#	define RAINBOW_IOS
#elif defined(__linux)
#	define RAINBOW_UNIX
#elif defined(_WIN32)
#	define RAINBOW_WIN
#	if defined(_MSC_VER) && (defined(_M_X64) || _M_IX86_FP >= 2)
#		define __SSE2__ 1
#	endif
#endif

// Platforms that make use of physical buttons (keyboards, gamepads, etc.)
// and of SDL
#if defined(RAINBOW_UNIX) || defined(RAINBOW_WIN)
#	define RAINBOW_BUTTONS
#	define RAINBOW_SDL
#endif

// Platforms with accelerometer and touch screen
#if defined(RAINBOW_ANDROID) || defined(RAINBOW_IOS)
#	define RAINBOW_ACCELERATED 1
#	define RAINBOW_TOUCHED 1
#else
#	define RAINBOW_ACCELERATED 0
#	define RAINBOW_TOUCHED 0
#endif

#define RAINBOW_BUILD "Rainbow / Bifrost Entertainment Property / Built " __DATE__

// Retrieve GCC compiler version
#ifdef __GNUC__
#	ifndef __GNUC_PATCHLEVEL__
#		define __GNUC_VERSION__ (__GNUC__ * 10000 + __GNUC_MINOR__ * 100)
#	else
#		define __GNUC_VERSION__ (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#	endif
#else
#	define __GNUC_VERSION__ 0
#endif

// Define nullptr for non-C++11-compliant compilers
#if __MINGW32__
#	define nullptr 0
#	define override
#elif __cplusplus <= 19971
#	if __GNUC_VERSION__ < 40600

const
class nullptr_t
{
public:
	template<class T>
	inline operator T*() const { return 0; }

	template<class C, class T>
	inline operator T C::*() const { return 0; }

private:
	void operator&() const;
} nullptr = {};

#	endif
#	if __GNUC_VERSION__ < 40700
#		define override
#	endif
#endif

#endif  // PLATFORM_H_
