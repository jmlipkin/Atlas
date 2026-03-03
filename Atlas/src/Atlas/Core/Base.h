#pragma once

// Platform detection using predefined macros
#ifdef _WIN32
	/* Windows x64/x86 */
	#ifdef _WIN64
		/* Windows x64  */
		#define AT_PLATFORM_WINDOWS
		#error "Windows is not supported!"
	#else
		/* Windows x86 */
		#error "x86 Builds are not supported!"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	/* TARGET_OS_MAC exists on all the platforms
	* so we must check all of them (in this order)
	* to ensure that we're running on MAC
	* and not some other Apple platform */
	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS simulator is not supported!"
	#elif TARGET_OS_IPHONE == 1
		#define AT_PLATFORM_IOS
		#error "IOS is not supported!"
	#elif TARGET_OS_MAC == 1
		#define AT_PLATFORM_MACOS
	#else
		#error "Unknown Apple platform!"
	#endif
/* We also have to check __ANDROID__ before __linux__
* since android is based on the linux kernel
* it has __linux__ defined */
#elif defined(__ANDROID__)
	#define AT_PLATFORM_ANDROID
	#error "Android is not supported!"
#elif defined(__linux__)
	#define AT_PLATFORM_LINUX
	#error "Linux is not supported!"
#else
	/* Unknown compiler/platform */
	#error "Unknown platform!"
#endif  // End of platform detection

#ifdef AT_DEBUG_ENABLE
	#if defined(AT_PLATFORM_WINDOWS)
		#define AT_DEBUG_BREAK() __debugbreak()
	#elif defined(__clang__) || defined(__GNUC__)
		#define AT_DEBUG_BREAK() __builtin_trap()
	#else
		#include <cstdlib>
		#define AT_DEBUG_BREAK() std::raise(SIGTRAP);
	#endif
	#define AT_ENABLE_ASSERTS
#else
	#define AT_DEBUG_BREAK()
#endif

#ifdef AT_ENABLE_ASSERTS
	#define AT_INTERNAL_ASSERT_IMPL(logger, check, ...)							\
			if (!(check)) {                           				            \
				logger(                                      				    \
					__VA_OPT__(__VA_ARGS__ )									\
					__VA_OPT__(,)												\
					"Assertion '{}' failed at {}:{}",					   	    \
					#check,                           				            \
					std::filesystem::path(__FILE__).filename().string(), 		\
					__LINE__                                      				\
				);                                                				\
				AT_DEBUG_BREAK();                                  				\
			}                                                     

	#define AT_ASSERT(check, ...) AT_EXPAND_MACRO(AT_INTERNAL_ASSERT_IMPL(AT_ERROR, check, ##__VA_ARGS__))
	#define AT_CORE_ASSERT(check, ...) AT_EXPAND_MACRO(AT_INTERNAL_ASSERT_IMPL(AT_CORE_ERROR, check, ##__VA_ARGS__))
#else
	#define AT_ASSERT(check, ...)
	#define AT_CORE_ASSERT(check, ...)
#endif

#define AT_EXPAND_MACRO(x) x
#define AT_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define AT_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)