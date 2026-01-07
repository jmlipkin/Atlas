#pragma once

#ifdef AT_ENABLE_ASSERTS
	#define AT_ASSERT(x, ...) { if(!(x)) { AT_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
	#define AT_CORE_ASSERT(x, ...) { if(!(x)) { AT_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
	#define AT_ASSERT(x, ...)
	#define AT_CORE_ASSERT(x, ...)
#endif


#define BIT(x) (1 << x)

#define AT_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)