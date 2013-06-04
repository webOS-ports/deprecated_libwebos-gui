/*
 * Copyright (c) 2012-2013 Carsten Munk <carsten.munk@gmail.com>
 *               2012-2013 Simon Busch <morphis@gravedo.de>
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

#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

#include "OffscreenNativeWindow.h"

OffscreenNativeWindow::OffscreenNativeWindow(unsigned int aWidth, unsigned int aHeight, unsigned int aFormat)
	: m_width(aWidth)
	, m_height(aHeight)
	, m_defaultWidth(aWidth)
	, m_defaultHeight(aHeight)
	, m_format(aFormat)
	, m_usage(GRALLOC_USAGE_HW_RENDER | GRALLOC_USAGE_HW_TEXTURE)
	, m_buffercount(3)
	, m_frontbuffer(m_buffercount - 1)
	, m_tailbuffer(0)
{
	m_buffers = new OffscreenNativeWindowBuffer*[m_buffercount];

	for(unsigned int i = 0; i < m_buffercount; i++)
		m_buffers[i] = 0;

	refcount = 1;
}

OffscreenNativeWindow::~OffscreenNativeWindow()
{
	TRACE("%s\n", __PRETTY_FUNCTION__);
}

// overloads from BaseNativeWindow
int OffscreenNativeWindow::setBufferCount(int cnt)
{
	TRACE("%s\n",__PRETTY_FUNCTION__);
	if( m_buffercount < cnt ) // only increase it
	{
		OffscreenNativeWindowBuffer** new_buffers = new OffscreenNativeWindowBuffer*[cnt];

		// transfer the pointers
		for(unsigned int i = 0; i < cnt; i++)
			new_buffers[i] = i < m_buffercount ? m_buffers[i] : 0;

		delete[] m_buffers; m_buffers = new_buffers;
		m_buffercount = cnt;
	}
	return NO_ERROR;
}

int OffscreenNativeWindow::setSwapInterval(int interval)
{
	TRACE("%s\n", __PRETTY_FUNCTION__);
	return 0;
}

OffscreenNativeWindowBuffer* OffscreenNativeWindow::allocateBuffer()
{
	int usage = m_usage | GRALLOC_USAGE_HW_TEXTURE;

	OffscreenNativeWindowBuffer *buffer = new OffscreenNativeWindowBuffer(width(), height(),
														m_format, usage);
	buffer->incStrong(0);

	return buffer;
}

int OffscreenNativeWindow::dequeueBuffer(BaseNativeWindowBuffer **buffer, int *fenceFd)
{
	TRACE("%s\n",__PRETTY_FUNCTION__);

	OffscreenNativeWindowBuffer *selectedBuffer = NULL;

	if(m_buffers[m_tailbuffer] == 0) {
		m_buffers[m_tailbuffer] = allocateBuffer();
		m_buffers[m_tailbuffer]->setIndex(m_tailbuffer);

		TRACE("buffer %i is at %p (native %p) handle=%i stride=%i\n",
				m_tailbuffer, m_buffers[m_tailbuffer], (ANativeWindowBuffer*) m_buffers[m_tailbuffer],
				m_buffers[m_tailbuffer]->handle, m_buffers[m_tailbuffer]->stride);

		selectedBuffer = m_buffers[m_tailbuffer];
	}
	else
	{
		selectedBuffer = m_buffers[m_tailbuffer];

		waitForBuffer(selectedBuffer);

		if (selectedBuffer->width != m_width || selectedBuffer->height != m_height) {
			TRACE("%s buffer and window size doesn't match: recreating buffer ...\n", __PRETTY_FUNCTION__);

			// Let the current buffer be cleared up by it's own
			selectedBuffer->decStrong(0);

			m_buffers[m_tailbuffer] = allocateBuffer();
			m_buffers[m_tailbuffer]->setIndex(m_tailbuffer);
			selectedBuffer = m_buffers[m_tailbuffer];
		}
	}

	*buffer = selectedBuffer;

	TRACE("dequeued buffer is %i %p\n", m_tailbuffer, selectedBuffer);

	m_tailbuffer++;

	if(m_tailbuffer == m_buffercount)
		m_tailbuffer = 0;

	return NO_ERROR;
}

int OffscreenNativeWindow::lockBuffer(BaseNativeWindowBuffer* buffer)
{
	TRACE("%s\n", __PRETTY_FUNCTION__);
	return NO_ERROR;
}

int OffscreenNativeWindow::queueBuffer(BaseNativeWindowBuffer* buffer, int fenceFd)
{
	TRACE("%s\n", __PRETTY_FUNCTION__);

	OffscreenNativeWindowBuffer* buf = static_cast<OffscreenNativeWindowBuffer*>(buffer);

	m_frontbuffer++;
	if (m_frontbuffer == m_buffercount)
		m_frontbuffer = 0;

	postBuffer(buf);

	return NO_ERROR;
}

int OffscreenNativeWindow::cancelBuffer(BaseNativeWindowBuffer* buffer, int fenceFd)
{
	TRACE("%s\n", __PRETTY_FUNCTION__);
	return 0;
}

unsigned int OffscreenNativeWindow::width() const
{
	TRACE("%s value: %i\n", __PRETTY_FUNCTION__, m_width);
	return m_width;
}

unsigned int OffscreenNativeWindow::height() const
{
	TRACE("%s value: %i\n", __PRETTY_FUNCTION__, m_height);
	return m_height;
}

unsigned int OffscreenNativeWindow::format() const
{
	TRACE("%s value: %i\n", __PRETTY_FUNCTION__, m_format);
	return m_format;
}

unsigned int OffscreenNativeWindow::defaultWidth() const
{
	TRACE("%s value: %i\n", __PRETTY_FUNCTION__, m_defaultWidth);
	return m_defaultWidth;
}

unsigned int OffscreenNativeWindow::defaultHeight() const
{
	TRACE("%s value: %i\n", __PRETTY_FUNCTION__, m_defaultHeight);
	return m_defaultHeight;
}

unsigned int OffscreenNativeWindow::queueLength() const
{
	TRACE("%s\n", __PRETTY_FUNCTION__);
	return 1;
}

unsigned int OffscreenNativeWindow::type() const
{
	TRACE("%s\n", __PRETTY_FUNCTION__);
	return NATIVE_WINDOW_SURFACE_TEXTURE_CLIENT;
}

unsigned int OffscreenNativeWindow::transformHint() const
{
	TRACE("%s\n", __PRETTY_FUNCTION__);
	return 0;
}

int OffscreenNativeWindow::setBuffersFormat(int format)
{
	TRACE("%s format %i\n", __PRETTY_FUNCTION__, format);
	m_format = format;
	return NO_ERROR;
}

int OffscreenNativeWindow::setBuffersDimensions(int width, int height)
{
	TRACE("%s size %ix%i\n", __PRETTY_FUNCTION__, width, height);
	return NO_ERROR;
}

int OffscreenNativeWindow::setUsage(int usage)
{
	TRACE("%s usage %i\n", __PRETTY_FUNCTION__, usage);
	m_usage = usage;
	return NO_ERROR;
}

void OffscreenNativeWindow::resize(unsigned int width, unsigned int height)
{
	TRACE("%s width=%i height=%i\n", __PRETTY_FUNCTION__, width, height);

	m_width = width;
	m_defaultWidth = width;
	m_height = height;
	m_defaultHeight = height;
}

unsigned int OffscreenNativeWindow::bufferCount()
{
	return m_buffercount;
}
