#ifndef __GUNSLINGERED_UTILITY_H__
#define __GUNSLINGERED_UTILITY_H__

#define SafeDelete( p ) { \
	if( p ){ delete p; p = nullptr; } }
#define SafeDeleteArray( p ) { \
	if( p ){ delete [ ] p; p = nullptr; } }

#endif // __GUNSLINGERED_UTILITY_H__

