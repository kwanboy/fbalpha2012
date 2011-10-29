// CPU Byte Order Utilities

// File_Extractor 1.0.0
#ifndef BLARGG_ENDIAN_H
#define BLARGG_ENDIAN_H

#include "blargg_common.h"

// BLARGG_CPU_CISC: Defined if CPU has very few general-purpose registers (< 16)
#if defined (_XBOX)
	#define BLARGG_CPU_POWERPC 1
	#define BLARGG_CPU_RISC 1
	#define BLARGG_BIG_ENDIAN 1
	#define BLARGG_NONPORTABLE 1
#elif defined (SN_TARGET_PS3)
	#define BLARGG_CPU_POWERPC 1
	#define BLARGG_CPU_RISC 1
	#define BLARGG_BIG_ENDIAN 1
	#define BLARGG_NONPORTABLE 1	 
#elif defined (__i386__) || defined (__x86_64__) || defined (_M_IX86) || defined (_M_X64)
	#define BLARGG_CPU_X86 1
	#define BLARGG_CPU_CISC 1
#endif

#if defined (__powerpc__) || defined (__ppc__) || defined (__ppc64__) || \
		defined (__POWERPC__) || defined (__powerc)
	#define BLARGG_CPU_POWERPC 1
	#define BLARGG_CPU_RISC 1
#endif

// BLARGG_BIG_ENDIAN, BLARGG_LITTLE_ENDIAN: Determined automatically, otherwise only
// one may be #defined to 1. Only needed if something actually depends on byte order.
#if !defined (BLARGG_BIG_ENDIAN) && !defined (BLARGG_LITTLE_ENDIAN)
#ifdef __GLIBC__
	// GCC handles this for us
	#include <endian.h>
	#if __BYTE_ORDER == __LITTLE_ENDIAN
		#define BLARGG_LITTLE_ENDIAN 1
	#elif __BYTE_ORDER == __BIG_ENDIAN
		#define BLARGG_BIG_ENDIAN 1
	#endif
#else

#if defined (LSB_FIRST) || defined (__LITTLE_ENDIAN__) || BLARGG_CPU_X86 || \
		(defined (LITTLE_ENDIAN) && LITTLE_ENDIAN+0 != 1234)
	#define BLARGG_LITTLE_ENDIAN 1
#endif

#if defined (MSB_FIRST)     || defined (__BIG_ENDIAN__) || defined (WORDS_BIGENDIAN) || \
	defined (__sparc__)     ||  BLARGG_CPU_POWERPC || \
	(defined (BIG_ENDIAN) && BIG_ENDIAN+0 != 4321)
	#define BLARGG_BIG_ENDIAN 1
#elif !defined (__mips__)
	// No endian specified; assume little-endian, since it's most common
	#define BLARGG_LITTLE_ENDIAN 1
#endif
#endif
#endif

#if BLARGG_LITTLE_ENDIAN && BLARGG_BIG_ENDIAN
	#undef BLARGG_LITTLE_ENDIAN
	#undef BLARGG_BIG_ENDIAN
#endif

inline void blargg_verify_byte_order()
{
	#ifndef NDEBUG
		#if BLARGG_BIG_ENDIAN
			volatile int i = 1;
			assert( *(volatile char*) &i == 0 );
		#elif BLARGG_LITTLE_ENDIAN
			volatile int i = 1;
			assert( *(volatile char*) &i != 0 );
		#endif
	#endif
}

inline unsigned get_le16( void const* p )
{
	return  (unsigned) ((unsigned char const*) p) [1] << 8 |
			(unsigned) ((unsigned char const*) p) [0];
}

inline unsigned get_be16( void const* p )
{
	return  (unsigned) ((unsigned char const*) p) [0] << 8 |
			(unsigned) ((unsigned char const*) p) [1];
}

inline unsigned get_le32( void const* p )
{
	return  (unsigned) ((unsigned char const*) p) [3] << 24 |
			(unsigned) ((unsigned char const*) p) [2] << 16 |
			(unsigned) ((unsigned char const*) p) [1] <<  8 |
			(unsigned) ((unsigned char const*) p) [0];
}

inline unsigned get_be32( void const* p )
{
	return  (unsigned) ((unsigned char const*) p) [0] << 24 |
			(unsigned) ((unsigned char const*) p) [1] << 16 |
			(unsigned) ((unsigned char const*) p) [2] <<  8 |
			(unsigned) ((unsigned char const*) p) [3];
}

inline void set_le16( void* p, unsigned n )
{
	((unsigned char*) p) [1] = (unsigned char) (n >> 8);
	((unsigned char*) p) [0] = (unsigned char) n;
}

inline void set_be16( void* p, unsigned n )
{
	((unsigned char*) p) [0] = (unsigned char) (n >> 8);
	((unsigned char*) p) [1] = (unsigned char) n;
}

inline void set_le32( void* p, unsigned n )
{
	((unsigned char*) p) [0] = (unsigned char) n;
	((unsigned char*) p) [1] = (unsigned char) (n >> 8);
	((unsigned char*) p) [2] = (unsigned char) (n >> 16);
	((unsigned char*) p) [3] = (unsigned char) (n >> 24);
}

inline void set_be32( void* p, unsigned n )
{
	((unsigned char*) p) [3] = (unsigned char) n;
	((unsigned char*) p) [2] = (unsigned char) (n >> 8);
	((unsigned char*) p) [1] = (unsigned char) (n >> 16);
	((unsigned char*) p) [0] = (unsigned char) (n >> 24);
}

#if BLARGG_NONPORTABLE
	// Optimized implementation if byte order is known
	#if BLARGG_LITTLE_ENDIAN
		#define GET_LE16( addr )        (*(BOOST::uint16_t const*) (addr))
		#define GET_LE32( addr )        (*(BOOST::uint32_t const*) (addr))
		#define SET_LE16( addr, data )  (void) (*(BOOST::uint16_t*) (addr) = (data))
		#define SET_LE32( addr, data )  (void) (*(BOOST::uint32_t*) (addr) = (data))
	#elif BLARGG_BIG_ENDIAN
		#define GET_BE16( addr )        (*(BOOST::uint16_t const*) (addr))
		#define GET_BE32( addr )        (*(BOOST::uint32_t const*) (addr))
		#define SET_BE16( addr, data )  (void) (*(BOOST::uint16_t*) (addr) = (data))
		#define SET_BE32( addr, data )  (void) (*(BOOST::uint32_t*) (addr) = (data))

		#if BLARGG_CPU_POWERPC
			// PowerPC has special byte-reversed instructions
			#if defined (__MWERKS__)
				#define GET_LE16( addr )        (__lhbrx( addr, 0 ))
				#define GET_LE32( addr )        (__lwbrx( addr, 0 ))
				#define SET_LE16( addr, in )    (__sthbrx( in, addr, 0 ))
				#define SET_LE32( addr, in )    (__stwbrx( in, addr, 0 ))
			#elif defined (__GNUC__)
				#define GET_LE16( addr )        ({unsigned short ppc_lhbrx_; __asm__ volatile( "lhbrx %0,0,%1" : "=r" (ppc_lhbrx_) : "r" (addr) : "memory" ); ppc_lhbrx_;})
				#define GET_LE32( addr )        ({unsigned short ppc_lwbrx_; __asm__ volatile( "lwbrx %0,0,%1" : "=r" (ppc_lwbrx_) : "r" (addr) : "memory" ); ppc_lwbrx_;})
				#define SET_LE16( addr, in )    ({__asm__ volatile( "sthbrx %0,0,%1" : : "r" (in), "r" (addr) : "memory" );})
				#define SET_LE32( addr, in )    ({__asm__ volatile( "stwbrx %0,0,%1" : : "r" (in), "r" (addr) : "memory" );})
			#endif
		#endif
	#endif
#endif

#ifndef SN_TARGET_PS3
#ifndef GET_LE16
	#define GET_LE16( addr )        get_le16( addr )
	#define SET_LE16( addr, data )  set_le16( addr, data )
#endif

#ifndef GET_LE32
	#define GET_LE32( addr )        get_le32( addr )
	#define SET_LE32( addr, data )  set_le32( addr, data )
#endif
#endif

#ifndef GET_BE16
	#define GET_BE16( addr )        get_be16( addr )
	#define SET_BE16( addr, data )  set_be16( addr, data )
#endif

#ifndef GET_BE32
	#define GET_BE32( addr )        get_be32( addr )
	#define SET_BE32( addr, data )  set_be32( addr, data )
#endif

// auto-selecting versions

#ifndef SN_TARGET_PS3
inline void set_le( BOOST::uint16_t* p, unsigned n ) { SET_LE16( p, n ); }
inline void set_le( BOOST::uint32_t* p, unsigned n ) { SET_LE32( p, n ); }
inline unsigned get_le( BOOST::uint16_t * p ) { return GET_LE16( p ); }
inline unsigned get_le( BOOST::uint32_t * p ) { return GET_LE32( p ); }
#endif
inline unsigned get_be( BOOST::uint16_t * p ) { return GET_BE16( p ); }
inline unsigned get_be( BOOST::uint32_t * p ) { return GET_BE32( p ); }
inline void set_be( BOOST::uint16_t* p, unsigned n ) { SET_BE16( p, n ); }
inline void set_be( BOOST::uint32_t* p, unsigned n ) { SET_BE32( p, n ); }

#endif
