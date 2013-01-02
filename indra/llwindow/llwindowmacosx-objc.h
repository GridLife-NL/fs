/** 
 * @file llwindowmacosx-objc.h
 * @brief Prototypes for functions shared between llwindowmacosx.cpp
 * and llwindowmacosx-objc.mm.
 *
 * $LicenseInfo:firstyear=2006&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2010, Linden Research, Inc.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 * Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
 * $/LicenseInfo$
 */
#include <boost/tr1/functional.hpp>
typedef std::tr1::function<void(unsigned short, unsigned int)> KeyCallback;
typedef std::tr1::function<void(unsigned int)> ModifierCallback;
typedef std::tr1::function<void(float*, unsigned int)> MouseCallback;
typedef std::tr1::function<void(wchar_t, unsigned int)> UnicodeCallback;
typedef std::tr1::function<void(unsigned int, unsigned int)> ResizeCallback;
typedef std::tr1::function<void(float)> ScrollWheelCallback;
typedef std::tr1::function<void()> VoidCallback;

// This will actually hold an NSCursor*, but that type is only available in objective C.
typedef void *CursorRef;
typedef void *NSWindowRef;
typedef void *GLViewRef;

/* Defined in llwindowmacosx-objc.mm: */
void setupCocoa();
CursorRef createImageCursor(const char *fullpath, int hotspotX, int hotspotY);
short releaseImageCursor(CursorRef ref);
short setImageCursor(CursorRef ref);
void setArrowCursor();
void setIBeamCursor();
void setPointingHandCursor();
void setCopyCursor();
void setCrossCursor();
void hideNSCursor();
void showNSCursor();
void hideNSCursorTillMove(bool hide);

NSWindowRef createNSWindow(int x, int y, int width, int height);

#include <OpenGL/OpenGL.h>
GLViewRef createOpenGLView(NSWindowRef window);
void glSwapBuffers(void* context);
CGLContextObj getCGLContextObj(GLViewRef view);
void getContentViewBounds(NSWindowRef window, float* bounds);
void getWindowSize(NSWindowRef window, float* size);
void setWindowSize(NSWindowRef window, int width, int height);
void getCursorPos(NSWindowRef window, float* pos);
void makeWindowOrderFront(NSWindowRef window);
void convertScreenToWindow(NSWindowRef window, float *coord);
void convertWindowToScreen(NSWindowRef window, float *coord);
void convertScreenToView(NSWindowRef window, float *coord);
void setWindowPos(NSWindowRef window, float* pos);
void closeWindow(NSWindowRef window);
void removeGLView(GLViewRef view);

// These are all implemented in llwindowmacosx.cpp.
// This is largely for easier interop between Obj-C and C++ (at least in the viewer's case due to the BOOL vs. BOOL conflict)
void callKeyUp(unsigned short key, unsigned int mask);
void callKeyDown(unsigned short key, unsigned int mask);
void callUnicodeCallback(wchar_t character, unsigned int mask);
void callRightMouseDown(float *pos, unsigned int mask);
void callRightMouseUp(float *pos, unsigned int mask);
void callLeftMouseDown(float *pos, unsigned int mask);
void callLeftMouseUp(float *pos, unsigned int mask);
void callDoubleClick(float *pos, unsigned int mask);
void callResize(unsigned int width, unsigned int height);
void callMouseMoved(float *pos, unsigned int mask);
void callScrollMoved(float delta);
void callMouseExit();
void callWindowFocus();
void callWindowUnfocus();
void callDeltaUpdate(float *delta, unsigned int mask);
void callMiddleMouseDown(float *pos, unsigned int mask);
void callMiddleMouseUp(float *pos, unsigned int mask);
void callFocus();
void callFocusLost();

NSWindowRef getMainAppWindow();
GLViewRef getGLView();

unsigned int getModifiers();
