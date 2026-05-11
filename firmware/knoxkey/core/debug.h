#ifndef knoxkey_DEBUG_H
#define knoxkey_DEBUG_H

// 0: asserts: disabled, enabled log levels: none
// 1: asserts: disabled, enabled log levels: error
// 2: asserts:  enabled, enabled log levels: error, info
// 3: asserts:  enabled, enabled log levels: error, info, debug
#ifndef knoxkey_DEBUG_LEVEL
	#define knoxkey_DEBUG_LEVEL 3
#endif

#define lion_enum_str(enum, value) debug_str_##enum[value]

#endif // knoxkey_DEBUG_H
