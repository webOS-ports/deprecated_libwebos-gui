/**
 *  Copyright (c) 2013 Simon Busch <morphis@gravedo.de>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>

#include "WebosMessages.h"
#include "WebosSurfaceManagerClient.h"

WebosSurfaceManagerClient::WebosSurfaceManagerClient()
    : m_socketFd(-1),
      m_socketPath("/tmp/surface_manager"),
      m_channel(0),
      m_socketWatch(0)
{
    g_idle_add(WebosSurfaceManagerClient::initCb, this);
}

WebosSurfaceManagerClient::~WebosSurfaceManagerClient()
{
}

gboolean WebosSurfaceManagerClient::initCb(gpointer user_data)
{
    WebosSurfaceManagerClient *client = reinterpret_cast<WebosSurfaceManagerClient*>(user_data);
    client->init();
    return FALSE;
}

void WebosSurfaceManagerClient::init()
{
    struct sockaddr_un socketAddr;

    m_socketFd = ::socket(PF_LOCAL, SOCK_STREAM, 0);
    if (m_socketFd < 0) {
        g_critical("%s: %d Failed to create socket: %s",
                   __PRETTY_FUNCTION__, __LINE__, strerror(errno));
        return false;
    }

    memset(&socketAddr, 0, sizeof(socketAddr));
    socketAddr.sun_family = AF_LOCAL;
    strncpy(socketAddr.sun_path, m_socketPath, G_N_ELEMENTS(socketAddr.sun_path));
    socketAddr.sun_path[G_N_ELEMENTS(socketAddr.sun_path)-1] = '\0';

    if (::connect(m_socketFd, (struct sockaddr*) &socketAddr,
                  SUN_LEN(&socketAddr)) != 0) {
        g_critical("%s:%d Failed to connect to socket: %s",
                   __PRETTY_FUNCTION__, __LINE__, strerror(errno));
        return;
    }

    m_channel =  g_io_channel_unix_new(m_socketFd);
    m_socketWatch = g_io_add_watch_full(m_channel, G_PRIORITY_DEFAULT, G_IO_IN,
                                        onIncomingDataCb, this, NULL);
}

gboolean WebosSurfaceManagerClient::onIncomingDataCb(GIOChannel *channel, GIOCondition condition, gpointer user_data)
{
    WebosSurfaceManagerClient *client = reinterpret_cast<WebosSurfaceManagerClient*>(user_data);
    client->onIncomingData();
    return TRUE;
}

void WebosSurfaceManagerClient::onIncomingData()
{
    char buffer;
    int ret;

    ret = read(m_socketFd, &buffer, 1);
    if (ret <= 0) {
        close(m_socketFd);
        m_socketFd = -1;
    }
}

void WebosSurfaceManagerClient::postBuffer(int winId, OffscreenNativeWindowBuffer *buffer)
{
    WebosMessageHeader hdr;
    int ret;

    memset(&hdr, 0, sizeof(WebosMessageHeader));
    hdr.windowId = winId;
    hdr.command = WEBOS_MESSAGE_TYPE_POST_BUFFER;

    ret = write(m_socketFd, &hdr, sizeof(WebosMessageHeader));

    buffer->writeToFd(m_socketFd);
}
