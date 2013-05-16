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

#include "WebosSurfaceManager.h"
#include "WebosSurfaceManagerRemoteClient.h"

struct compositor_ctrl_hdr {
    uint32_t windowId;
    uint32_t command;
};

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
	struct compositor_ctrl_hdr hdr;

	memset(&hdr, 0, sizeof(struct compositor_ctrl_hdr));

	ret = read(m_socketFd, &hdr, sizeof(struct compositor_ctrl_hdr));
	if (ret <= 0) {
		g_message("%s: Client closed connection; removing ...", __PRETTY_FUNCTION__);
		close(m_socketFd);
		m_parent->onClientDisconnected(this);
		return;
	}

	OffscreenNativeWindowBuffer *buffer = new OffscreenNativeWindowBuffer();
	buffer->readFromFd(m_socketFd);

	g_message("Received buffer from remote index = %i", buffer->index());

	buffer->incStrong(0);

	handleIncomingBuffer(hdr.windowId, buffer);
}

void WebosSurfaceManagerRemoteClient::handleIncomingBuffer(int windowId, OffscreenNativeWindowBuffer *buffer)
{
}

