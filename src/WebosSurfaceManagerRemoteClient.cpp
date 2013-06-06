/* @@@LICENSE
*
* Copyright (c) 2013 Simon Busch <morphis@gravedo.de>
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
* LICENSE@@@ */

#include <glib.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <EGL/eglhybris.h>

#include "WebosMessages.h"
#include "WebosSurfaceManager.h"
#include "WebosSurfaceManagerRemoteClient.h"

WebosSurfaceManagerRemoteClient::WebosSurfaceManagerRemoteClient(WebosSurfaceManager *parent, int socketFd)
	: m_parent(parent),
	  m_socketFd(socketFd),
	  m_channel(0),
	  m_socketWatch(0)
{
	m_channel =  g_io_channel_unix_new(m_socketFd);
	m_socketWatch = g_io_add_watch_full(m_channel, G_PRIORITY_DEFAULT, G_IO_IN,
										onIncomingDataCb, this, NULL);
}

WebosSurfaceManagerRemoteClient::~WebosSurfaceManagerRemoteClient()
{
}

gboolean WebosSurfaceManagerRemoteClient::onIncomingDataCb(GIOChannel *channel, GIOCondition condition, gpointer user_data)
{
	WebosSurfaceManagerRemoteClient *client = reinterpret_cast<WebosSurfaceManagerRemoteClient*>(user_data);
	client->onIncomingData();
	return TRUE;
}

void WebosSurfaceManagerRemoteClient::onIncomingData()
{
	int ret;
	WebosMessageHeader hdr;
	OffscreenNativeWindowBuffer *buffer;

	memset(&hdr, 0, sizeof(WebosMessageHeader));

	ret = read(m_socketFd, &hdr, sizeof(WebosMessageHeader));
	if (ret <= 0) {
		g_message("%s: Client closed connection; removing ...", __PRETTY_FUNCTION__);
		close(m_socketFd);
		m_parent->onClientDisconnected(this);
		return;
	}

	switch (hdr.command) {
	case WEBOS_MESSAGE_TYPE_POST_BUFFER:
		buffer = new OffscreenNativeWindowBuffer();
		buffer->incStrong(0);

		buffer->readFromFd(m_socketFd);
		handleIncomingBuffer(hdr.windowId, buffer);
		break;
	default:
		g_warning("%s: unhandled message type %i for window %i",
			__PRETTY_FUNCTION__, hdr.command, hdr.windowId);
		break;
	}
}

void WebosSurfaceManagerRemoteClient::handleIncomingBuffer(int windowId, OffscreenNativeWindowBuffer *buffer)
{
}

void WebosSurfaceManagerRemoteClient::releaseBuffer(int windowId, OffscreenNativeWindowBuffer *buffer)
{
	WebosMessageHeader hdr;
	int ret;

	memset(&hdr, 0, sizeof(WebosMessageHeader));
	hdr.windowId = windowId;
	hdr.command = WEBOS_MESSAGE_TYPE_RELEASE_BUFFER;;

	// XXX: add proper error checking
	ret = write(m_socketFd, &hdr, sizeof(WebosMessageHeader));
	unsigned int bufferIndex = buffer->index();
	ret = write(m_socketFd, &bufferIndex, sizeof(unsigned int));
}
