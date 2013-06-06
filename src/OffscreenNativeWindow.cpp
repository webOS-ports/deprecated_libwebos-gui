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

#define TRACE(message, ...) g_message(message, ##__VA_ARGS__)

OffscreenNativeWindow::OffscreenNativeWindow(unsigned int aWidth, unsigned int aHeight, unsigned int aFormat)
	: m_width(aWidth)
	, m_height(aHeight)
	, m_defaultWidth(aWidth)
	, m_defaultHeight(aHeight)
	, m_format(aFormat)
	, m_usage(GRALLOC_USAGE_HW_RENDER | GRALLOC_USAGE_HW_TEXTURE)
	, m_surfaceClient(this)
{
	// set refcount manually here as we're not caring about references of the native
	// window yet
	refcount = 1;

	setBufferCount(3);

	g_mutex_init(&m_bufferMutex);
	g_cond_init(&m_nextBufferCondition);
}

OffscreenNativeWindow::~OffscreenNativeWindow()
{
	// XXX cleanup all used buffers
	g_cond_clear(&m_nextBufferCondition);
	g_mutex_clear(&m_bufferMutex);
}

void OffscreenNativeWindow::identify(unsigned int windowId)
{
	m_surfaceClient.identify(windowId);
}

OffscreenNativeWindowBuffer* OffscreenNativeWindow::allocateBuffer()
{
	int usage = m_usage | GRALLOC_USAGE_HW_TEXTURE;
	static unsigned int bufferIndex = 0;

	OffscreenNativeWindowBuffer *buffer = new OffscreenNativeWindowBuffer(width(), height(),
														m_format, usage);
	buffer->incStrong(0);
	buffer->setIndex(++bufferIndex);

	return buffer;
}

int OffscreenNativeWindow::setBufferCount(int count)
{
	TRACE("%s: count=%i", __PRETTY_FUNCTION__, count);

	if (m_buffers.size() < count) {
		for (int n = 0; n < m_buffers.size() - count; n++) {
			OffscreenNativeWindowBuffer *buffer = allocateBuffer();
			m_buffers.push_back(buffer);
		}
	}

	return NO_ERROR;
}

int OffscreenNativeWindow::setSwapInterval(int interval)
{
	return 0;
}

void OffscreenNativeWindow::releaseBuffer(unsigned int index)
{
	TRACE("%s", __PRETTY_FUNCTION__);

	std::list<OffscreenNativeWindowBuffer*>::iterator iter;

	g_mutex_lock(&m_bufferMutex);

	for (iter = m_buffers.begin(); iter != m_buffers.end(); iter++) {
		OffscreenNativeWindowBuffer *buffer = *iter;

		if (buffer->index() == index)
			buffer->setBusy(false);
	}

	g_mutex_unlock(&m_bufferMutex);

	g_cond_broadcast(&m_nextBufferCondition);
}

int OffscreenNativeWindow::dequeueBuffer(BaseNativeWindowBuffer **buffer, int *fenceFd)
{
	TRACE("%s", __PRETTY_FUNCTION__);

	OffscreenNativeWindowBuffer *selectedBuffer = 0;
	std::list<OffscreenNativeWindowBuffer*>::iterator iter;

	g_mutex_lock(&m_bufferMutex);

	while (1) {
		for (iter = m_buffers.begin(); iter != m_buffers.end(); iter++) {
			OffscreenNativeWindowBuffer *currentBuffer = *iter;
			if (!currentBuffer->busy()) {
				TRACE("%s: Found buffer ready to be used", __PRETTY_FUNCTION__);

				selectedBuffer = currentBuffer;
				selectedBuffer->setBusy(true);
				break;
			}
		}

		if (selectedBuffer)
			break;

		TRACE("%s: Waiting for buffer to be released", __PRETTY_FUNCTION__);
		g_cond_wait(&m_nextBufferCondition, &m_bufferMutex);
	}

	g_mutex_unlock(&m_bufferMutex);

	*buffer = selectedBuffer;

	return NO_ERROR;
}

int OffscreenNativeWindow::queueBuffer(BaseNativeWindowBuffer* buffer, int fenceFd)
{
	TRACE("%s", __PRETTY_FUNCTION__);

	OffscreenNativeWindowBuffer* buf = static_cast<OffscreenNativeWindowBuffer*>(buffer);
	m_surfaceClient.postBuffer(buf);

	return NO_ERROR;
}

int OffscreenNativeWindow::lockBuffer(BaseNativeWindowBuffer* buffer)
{
	return NO_ERROR;
}

int OffscreenNativeWindow::cancelBuffer(BaseNativeWindowBuffer* buffer, int fenceFd)
{
	return 0;
}

unsigned int OffscreenNativeWindow::width() const
{
	return m_width;
}

unsigned int OffscreenNativeWindow::height() const
{
	return m_height;
}

unsigned int OffscreenNativeWindow::format() const
{
	return m_format;
}

unsigned int OffscreenNativeWindow::defaultWidth() const
{
	return m_defaultWidth;
}

unsigned int OffscreenNativeWindow::defaultHeight() const
{
	return m_defaultHeight;
}

unsigned int OffscreenNativeWindow::queueLength() const
{
	return 1;
}

unsigned int OffscreenNativeWindow::type() const
{
	return NATIVE_WINDOW_SURFACE_TEXTURE_CLIENT;
}

unsigned int OffscreenNativeWindow::transformHint() const
{
	return 0;
}

int OffscreenNativeWindow::setBuffersFormat(int format)
{
	m_format = format;
	return NO_ERROR;
}

int OffscreenNativeWindow::setBuffersDimensions(int width, int height)
{
	return NO_ERROR;
}

int OffscreenNativeWindow::setUsage(int usage)
{
	m_usage = usage;
	return NO_ERROR;
}

void OffscreenNativeWindow::resize(unsigned int width, unsigned int height)
{
	m_width = width;
	m_defaultWidth = width;
	m_height = height;
	m_defaultHeight = height;
}
