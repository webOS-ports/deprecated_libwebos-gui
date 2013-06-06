#ifndef OFFSCREEN_WINDOW_H
#define OFFSCREEN_WINDOW_H

#include <list>
#include <linux/fb.h>
#include <android/hardware/gralloc.h>
#include <hybris/eglplatformcommon/nativewindowbase.h>
#include <glib.h>

#include "WebosSurfaceManagerClient.h"

class OffscreenNativeWindowBuffer : public BaseNativeWindowBuffer
{
	friend class OffscreenNativeWindow;

protected:
	OffscreenNativeWindowBuffer(unsigned int width, unsigned int height,
								unsigned int format, unsigned int usage);

public:
	OffscreenNativeWindowBuffer();
	~OffscreenNativeWindowBuffer();

	int writeToFd(int fd);
	int readFromFd(int fd);

	buffer_handle_t getHandle();

	unsigned int index() const { return m_index; }
	void setIndex(unsigned int index) { m_index = index; }

	bool busy() const { return m_busy; }
	bool setBusy(bool busy) { m_busy = busy; }

private:
	enum {
		ownNone = 0,
		ownHandle = 1,
		ownData = 2,
	};

private:
	unsigned int m_index;
	uint8_t m_owner;
	bool m_busy;
};

class OffscreenNativeWindow : public BaseNativeWindow, public IBufferManager
{
public:
	OffscreenNativeWindow(unsigned int width, unsigned int height, unsigned int format = 5);
	~OffscreenNativeWindow();

	void resize(unsigned int width, unsigned int height);

	virtual void releaseBuffer(unsigned int index);

protected:
	virtual unsigned int platformWindowId() = 0;

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
	WebosSurfaceManagerClient m_surfaceClient;
	GMutex m_bufferMutex;
	GCond m_nextBufferCondition;
private:
	OffscreenNativeWindowBuffer* allocateBuffer();

};

#endif
