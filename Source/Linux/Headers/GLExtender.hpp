#ifndef __MULECLIENT_GLEXTENDER_HPP__
#define __MULECLIENT_GLEXTENDER_HPP__

#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glxext.h>
#include <GL/glext.h>

#ifdef __cplusplus
extern "C"
{
#endif

extern PFNGLBINDTEXTURE			__glBindTexture;

#define glBindTexture	__glBindTexture

#ifdef __cplusplus
}
#endif

#endif

