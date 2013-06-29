#ifndef OFFSCREEN_WINDOW_BUFFER_H
#define OFFSCREEN_WINDOW_BUFFER_H

#include <hybris/eglplatformcommon/nativewindowbase.h>

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

#endif
