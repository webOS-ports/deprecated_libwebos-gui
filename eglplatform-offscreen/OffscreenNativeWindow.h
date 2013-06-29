#ifndef OFFSCREEN_WINDOW_H
#define OFFSCREEN_WINDOW_H

#include <list>
#include <linux/fb.h>
#include <android/hardware/gralloc.h>
#include <hybris/eglplatformcommon/nativewindowbase.h>
#include <glib.h>

#include <EGL/egl.h>

#include "WebosSurfaceManagerClient.h"

class OffscreenNativeWindowBuffer;

class OffscreenNativeWindow : public BaseNativeWindow, public IBufferManager
{
public:
	OffscreenNativeWindow(WebosSurfaceManagerClient *ipSurfaceClient);
	~OffscreenNativeWindow();

	void identify(unsigned int windowId);
	void resize(unsigned int width, unsigned int height);
	EGLNativeWindowType getNativeWindow();

	virtual void releaseBuffer(unsigned int index);

protected:
	// overloads from BaseNativeWindow
	virtual int setSwapInterval(int interval);
	virtual int dequeueBuffer(BaseNativeWindowBuffer **buffer, int *fenceFd);
	virtual int queueBuffer(BaseNativeWindowBuffer* buffer, int fenceFd);
	virtual int cancelBuffer(BaseNativeWindowBuffer* buffer, int fenceFd);
	virtual int lockBuffer(BaseNativeWindowBuffer* buffer);
	virtual unsigned int type() const;
	virtual unsigned int width() const;
	virtual unsigned int height() const;
	virtual unsigned int format() const;
	virtual unsigned int defaultWidth() const;
	virtual unsigned int defaultHeight() const;
	virtual unsigned int queueLength() const;
	virtual unsigned int transformHint() const;
	// perform calls
	virtual int setUsage(int usage);
	virtual int setBufferCount(int cnt);
	virtual int setBuffersFormat(int format);
	virtual int setBuffersDimensions(int width, int height);
private:
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_format;
	unsigned int m_defaultWidth;
	unsigned int m_defaultHeight;
	unsigned int m_usage;
	std::list<OffscreenNativeWindowBuffer*> m_buffers;
	WebosSurfaceManagerClient *m_surfaceClient;
	GMutex m_bufferMutex;
	GCond m_nextBufferCondition;
private:
	OffscreenNativeWindowBuffer* allocateBuffer();

};

#endif
