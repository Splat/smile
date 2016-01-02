
#ifndef __SMILE_PLATFORM_WINDOWS_MSVC_X86_PLATFORM_H__
#define __SMILE_PLATFORM_WINDOWS_MSVC_X86_PLATFORM_H__

//------------------------------------------------------------------------------------------------
//  OS: Microsoft Windows.  Compiler: Microsoft Visual C++.  Architecture: Intel x86.

#ifdef SMILE_PLATFORM_IS_DEFINED
#error Only one platform can be selected at a time!
#endif
#define SMILE_PLATFORM_IS_DEFINED

//------------------------------------------------------------------------------------------------
//  Portable type definitions.

// Portable fixed-size and fixed-sign types.
typedef char SByte;
typedef unsigned char Byte;
typedef char Int8;
typedef unsigned char UInt8;
typedef short Int16;
typedef unsigned short UInt16;
typedef long Int32;
typedef unsigned long UInt32;
typedef __int64 Int64;
typedef unsigned __int64 UInt64;

// Portable pointer-casting types.
typedef UInt32 PtrInt;		// An unsigned integer type that is the same size as a pointer.
typedef Int32 Int;			// A signed integer type that matches the native platform's "best" register size.
typedef UInt32 UInt;		// An unsigned integer type that matches the native platform's "best" register size.

#define SizeofPtrInt 4
#define SizeofInt 4

// Portable binary floating-point types.
typedef float Float32;
typedef double Float64;
typedef struct { UInt64 value[2]; } Float128;

// Portable decimal floating-point types.
typedef struct { UInt32 value; } Real32;
typedef struct { UInt64 value; } Real64;
typedef struct { UInt64 value[2]; } Real128;

//------------------------------------------------------------------------------------------------
//  Declaration prefixes.

// How to make functions behave as 'inline' in this compiler.
#undef Inline
#define Inline static __inline

// How to declare thread-local data.
#define SMILE_HAS_THREAD_LOCAL True
#define SMILE_THREAD_LOCAL __declspec(thread)

// How to export public functions outside SmileLib.
#undef SMILE_API_FUNC
#undef SMILE_API_DATA
#ifdef SMILELIB_BUILD
	#define SMILE_API_FUNC extern __declspec(dllexport)
	#define SMILE_API_DATA extern __declspec(dllexport)
#else
	#define SMILE_API_FUNC extern __declspec(dllimport)
	#define SMILE_API_DATA extern __declspec(dllimport)
#endif

// How to align data structures in memory.
#undef SMILE_ALIGN
#define SMILE_ALIGN(__n__) __declspec(align(__n__))

// Compatibility macros.
#undef SMILE_DECLARATION_STATIC_PROTOTYPE
#define SMILE_DECLARATION_STATIC_PROTOTYPE extern
#undef SMILE_DECLARATION_EXTERN_OF_UNKNOWN_SIZE
#define SMILE_DECLARATION_EXTERN_OF_UNKNOWN_SIZE

//------------------------------------------------------------------------------------------------
//  Entropy.

// Get a reasonable degree of entropy from wherever this platform keeps it, as quickly as possible.
// This doesn't need to be crypto-secure; it just needs to be suitably semi-random.
Inline UInt64 GetBaselineEntropy(void)
{
    UInt64 tsc;
    __asm {
        cpuid
        rdtsc
        mov dword ptr [tsc+0], eax
        mov dword ptr [tsc+4], edx
    }
    return tsc;
}

#endif