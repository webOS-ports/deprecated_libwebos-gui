/*
 * Copyright (c) 2013 Christophe Chapuis <chris.chapuis@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <hybris/eglplatformcommon/ws.h>
#include "OffscreenNativeWindow.h"
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>
extern "C" {
#include <hybris/eglplatformcommon/eglplatformcommon.h>
};

extern "C" int offscreenws_IsValidDisplay(EGLNativeDisplayType display)
{
	return display == EGL_DEFAULT_DISPLAY;
}

extern "C" EGLNativeWindowType offscreenws_CreateWindow(EGLNativeWindowType win, EGLNativeDisplayType display)
{
	EGLNativeWindowType eglNativeWindow = 0;

	IWebosEglWindow *pWebosEglWindow = (IWebosEglWindow*)win;
	if (pWebosEglWindow) {
		eglNativeWindow = pWebosEglWindow->getNativeWindow();

		if (0 == eglNativeWindow) {
			// No native window yet ? Create one.
			WebosSurfaceManagerClient *pSurfaceClient = dynamic_cast<WebosSurfaceManagerClient*>(pWebosEglWindow);

			OffscreenNativeWindow *pNewOffscreenWindow = new OffscreenNativeWindow(pSurfaceClient);
			eglNativeWindow = pNewOffscreenWindow->getNativeWindow();
		}
	}

	return eglNativeWindow;
}

extern "C" __eglMustCastToProperFunctionPointerType offscreenws_eglGetProcAddress(const char *procname)
{
	return eglplatformcommon_eglGetProcAddress(procname);
}

extern "C" void offscreenws_passthroughImageKHR(EGLenum *target, EGLClientBuffer *buffer)
{
	eglplatformcommon_passthroughImageKHR(target, buffer);
}

struct ws_module ws_module_info = {
	offscreenws_IsValidDisplay,
	offscreenws_CreateWindow,
	offscreenws_eglGetProcAddress,
	offscreenws_passthroughImageKHR,
	eglplatformcommon_eglQueryString
};

// vim:ts=4:sw=4:noexpandtab
