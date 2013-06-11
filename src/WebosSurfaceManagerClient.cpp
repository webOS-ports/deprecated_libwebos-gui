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

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/fcntl.h>

#include "WebosMessages.h"
#include "WebosSurfaceManagerClient.h"
#include "OffscreenNativeWindow.h"

WebosSurfaceManagerClient::WebosSurfaceManagerClient(IBufferManager *manager)
    : m_socketFd(-1),
      m_bufferManager(manager)
{
    m_socketPath = g_strdup("/tmp/surface_manager");

    m_thread = g_thread_new("surface_client", WebosSurfaceManagerClient::startupCallback, this);
    assert(m_thread != g_thread_self());
}

WebosSurfaceManagerClient::~WebosSurfaceManagerClient()
{
    g_thread_join(m_thread);
}

gpointer WebosSurfaceManagerClient::startupCallback(gpointer user_data)
{
    WebosSurfaceManagerClient *client = reinterpret_cast<WebosSurfaceManagerClient*>(user_data);
    client->startup();
    return FALSE;
}

void WebosSurfaceManagerClient::startup()
{
    struct sockaddr_un socketAddr;
    int ret;

    assert(g_thread_self() == m_thread);

    m_socketFd = ::socket(PF_LOCAL, SOCK_STREAM, 0);
    if (m_socketFd < 0) {
        g_critical("%s: %d Failed to create socket: %s",
                   __PRETTY_FUNCTION__, __LINE__, strerror(errno));
        return;
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

    while(1) {
        fd_set fds;

        FD_ZERO(&fds);
        FD_SET(m_socketFd, &fds);

        ret = select(m_socketFd+1, &fds, NULL, NULL, NULL);
        if (ret < 0) {
            g_warning("%s: connection was closed from server site", __PRETTY_FUNCTION__);
            break;
        }
        else if (ret && FD_ISSET(m_socketFd, &fds)) {
            onIncomingData();
        }
    }

    close(m_socketFd);
    m_socketFd = -1;
}

void WebosSurfaceManagerClient::onIncomingData()
{
    int ret;
    unsigned int bufferIndex = 0;
    WebosMessageHeader hdr;
    OffscreenNativeWindowBuffer *buffer;

    ret = read(m_socketFd, &hdr, sizeof(WebosMessageHeader));
    if (ret <= 0) {
        g_message("%s: Server closed connection. Shutting down ...", __PRETTY_FUNCTION__);
        close(m_socketFd);
        return;
    }

    switch (hdr.command) {
    case WEBOS_MESSAGE_TYPE_RELEASE_BUFFER:
        ret = read(m_socketFd, &bufferIndex, sizeof(unsigned int));
        if (ret != sizeof(unsigned int)) {
            g_warning("%s: Failed to read buffer index", __PRETTY_FUNCTION__);
            return;
        }

        if (bufferIndex == 0) {
            g_warning("%s: Skipping invalid buffer index %i", __PRETTY_FUNCTION__, bufferIndex);
            return;
        }

        m_bufferManager->releaseBuffer(bufferIndex);
        break;
    default:
        g_warning("%s: unhandled message type %i",
                  __PRETTY_FUNCTION__, hdr.command);
        break;
    }
}

void WebosSurfaceManagerClient::identify(unsigned int windowId)
{
    WebosMessageHeader hdr;
    int ret;

    memset(&hdr, 0, sizeof(WebosMessageHeader));
    hdr.command = WEBOS_MESSAGE_TYPE_IDENTIFY;

    ret = write(m_socketFd, &hdr, sizeof(WebosMessageHeader));
    if (ret != sizeof(WebosMessageHeader)) {
        g_critical("%s: Failed to write header!", __PRETTY_FUNCTION__);
        return;
    }

    ret = write(m_socketFd, &windowId, sizeof(unsigned int));
    if (ret != sizeof(unsigned int)) {
        g_critical("%s: Failed to write window id!", __PRETTY_FUNCTION__);
        return;
    }
}

void WebosSurfaceManagerClient::postBuffer(OffscreenNativeWindowBuffer *buffer)
{
    WebosMessageHeader hdr;
    int ret;

    memset(&hdr, 0, sizeof(WebosMessageHeader));
    hdr.command = WEBOS_MESSAGE_TYPE_POST_BUFFER;

    ret = write(m_socketFd, &hdr, sizeof(WebosMessageHeader));
    if (ret != sizeof(WebosMessageHeader)) {
        g_critical("%s: Failed to write header!", __PRETTY_FUNCTION__);
        return;
    }

    ret = buffer->writeToFd(m_socketFd);
    if (ret < 0) {
        g_critical("%s: Failed to send buffer to remote!", __PRETTY_FUNCTION__);
        return;
    }
}
