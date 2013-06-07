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
#include <glib/gstdio.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "WebosSurfaceManager.h"
#include "WebosSurfaceManagerRemoteClient.h"

static const int kMaxConnections = 100;

class WebosSurfaceManagerRemoteClientFactoryDefault : public WebosSurfaceManagerRemoteClientFactory
{
public:
	virtual WebosSurfaceManagerRemoteClient *create(WebosSurfaceManager *parent, int socketFd)
	{
		return new WebosSurfaceManagerRemoteClient(parent, socketFd);
	}
};

WebosSurfaceManager* WebosSurfaceManager::instance()
{
	static WebosSurfaceManager* s_server = 0;

	if (G_UNLIKELY(s_server == 0))
		s_server = new WebosSurfaceManager;

	return s_server;
}

WebosSurfaceManager::WebosSurfaceManager()
	: m_socketFd(-1),
	  m_channel(0),
	  m_socketWatch(-1),
	  m_remoteClientFactory(new WebosSurfaceManagerRemoteClientFactoryDefault)
{
	m_socketPath = g_strdup("/tmp/surface_manager");
	setup();
}

WebosSurfaceManager::~WebosSurfaceManager()
{
	if (m_socketWatch > 0)
		g_source_remove(m_socketWatch);

	if (m_channel != NULL)
		g_io_channel_shutdown(m_channel, TRUE, NULL);

	if (m_socketFd > 0)
		close(m_socketFd);
}

void WebosSurfaceManager::setRemoteClientFactory(WebosSurfaceManagerRemoteClientFactory *factory)
{
	if (m_remoteClientFactory)
		delete m_remoteClientFactory;

	m_remoteClientFactory = factory;
}

void WebosSurfaceManager::setup()
{
	if (g_file_test(m_socketPath, G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR))
		g_remove(m_socketPath);

	g_message("%s: %d Initializing buffer server ...",
			  __PRETTY_FUNCTION__, __LINE__);

	m_socketFd = ::socket(PF_LOCAL, SOCK_STREAM, 0);
	if (m_socketFd < 0) {
		g_critical("%s: %d Failed to create socket: %s",
				   __PRETTY_FUNCTION__, __LINE__, strerror(errno));
		exit(-1);
	}

	struct sockaddr_un socketAddr;
	socketAddr.sun_family = AF_LOCAL;
	::strncpy(socketAddr.sun_path, m_socketPath,
			  G_N_ELEMENTS(socketAddr.sun_path));
	socketAddr.sun_path[G_N_ELEMENTS(socketAddr.sun_path)-1] = '\0';

	if (::bind(m_socketFd, (struct sockaddr*) &socketAddr, SUN_LEN(&socketAddr)) != 0) {
		g_warning("%s: Failed to bind socket", __PRETTY_FUNCTION__);
		close(m_socketFd);
		m_socketFd = -1;
		return;
	}

	if (::listen(m_socketFd, kMaxConnections) != 0) {
		g_warning("%s: Failed to listen on socket", __PRETTY_FUNCTION__);
		close(m_socketFd);
		m_socketFd = -1;
		return;
	}

	m_channel =  g_io_channel_unix_new(m_socketFd);
	m_socketWatch = g_io_add_watch_full(m_channel, G_PRIORITY_DEFAULT, G_IO_IN,
										onNewConnectionCb, this, NULL);

	g_message("%s: %d Buffer server successfully initialized",
			  __PRETTY_FUNCTION__, __LINE__);
}

gboolean WebosSurfaceManager::onNewConnectionCb(GIOChannel *channel, GIOCondition condition, gpointer user_data)
{
	WebosSurfaceManager *manager = reinterpret_cast<WebosSurfaceManager*>(user_data);
	manager->onNewConnection();
	return TRUE;
}

void WebosSurfaceManager::onClientDisconnected(WebosSurfaceManagerRemoteClient *client)
{
	m_clients.remove(client);
	delete client;
}

void WebosSurfaceManager::onNewConnection()
{
	struct sockaddr_un  socketAddr;
	socklen_t socketAddrLen;
	int clientSocketFd = -1;

	g_message("%s: New buffer sharing client connected", __PRETTY_FUNCTION__);

	memset(&socketAddr, 0, sizeof(socketAddr));
	memset(&socketAddrLen, 0, sizeof(socketAddrLen));

	clientSocketFd = ::accept(m_socketFd, (struct sockaddr*) &socketAddr, &socketAddrLen);
	if (-1 == clientSocketFd) {
		g_warning("%s: %d Failed to accept inbound connection: %s",
				   __PRETTY_FUNCTION__, __LINE__, strerror(errno));
		return;
	}

	WebosSurfaceManagerRemoteClient *client = m_remoteClientFactory->create(this, clientSocketFd);
	m_clients.push_back(client);
}

WebosSurfaceManagerRemoteClient* WebosSurfaceManager::findClient(unsigned int windowId)
{
	WebosSurfaceManagerRemoteClient *client = 0;
	std::list<WebosSurfaceManagerRemoteClient*>::iterator iter;

	for (iter = m_clients.begin(); iter != m_clients.end(); iter++) {
		WebosSurfaceManagerRemoteClient *currentClient = *iter;

		if (currentClient->winId() == windowId) {
			client = currentClient;
			break;
		}
	}

	return client;
}
