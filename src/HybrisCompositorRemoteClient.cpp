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

#include <QDebug>
#include <QFile>

#include <glib.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <EGL/eglhybris.h>

#include "HybrisCompositor.h"
#include "HybrisCompositorRemoteClient.h"

struct compositor_ctrl_hdr {
    uint32_t windowId;
    uint32_t command;
};

HybrisCompositorRemoteClient::HybrisCompositorRemoteClient(HybrisCompositor *parent, int socketFd)
	: QObject(parent),
	  m_parent(parent),
	  m_socketFd(socketFd)
{
	m_socketNotifier = new QSocketNotifier(m_socketFd, QSocketNotifier::Read, this);
	connect(m_socketNotifier, SIGNAL(activated(int)), this, SLOT(onIncomingData()));
}

HybrisCompositorRemoteClient::~HybrisCompositorRemoteClient()
{
	m_socketNotifier->deleteLater();
}

void HybrisCompositorRemoteClient::onIncomingData()
{
	int ret;
	struct compositor_ctrl_hdr hdr;

	memset(&hdr, 0, sizeof(struct compositor_ctrl_hdr));

	ret = read(m_socketFd, &hdr, sizeof(struct compositor_ctrl_hdr));
	if (ret <= 0) {
		qDebug() << __PRETTY_FUNCTION__ << "Client closed connection; removing ...";
		close(m_socketFd);
		Q_EMIT disconnected();
		return;
	}

	OffscreenNativeWindowBuffer *buffer = new OffscreenNativeWindowBuffer();
	buffer->readFromFd(m_socketFd);

	hybris_register_buffer_handle(buffer->getHandle());

	handleIncomingBuffer(hdr.windowId, buffer);
}

void HybrisCompositorRemoteClient::handleIncomingBuffer(int windowId, OffscreenNativeWindowBuffer *buffer)
{
}

