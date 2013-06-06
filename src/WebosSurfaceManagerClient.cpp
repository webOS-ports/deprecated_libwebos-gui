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
#include "OffscreenNativeWindow.h"

WebosSurfaceManagerClient::WebosSurfaceManagerClient(IBufferManager *manager)
    : m_socketFd(-1),
      m_channel(0),
      m_socketWatch(0),
      m_bufferManager(manager)
{
    m_socketPath = g_strdup("/tmp/surface_manager");
    m_thread = g_thread_new("surface_client", WebosSurfaceManagerClient::startupCallback, this);
}

WebosSurfaceManagerClient::~WebosSurfaceManagerClient()
{
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
    GMainContext *context;

    g_message("%s: starting thread to handle client communication ...", __PRETTY_FUNCTION__);

    // create new context and mainloop for our thread
    context = g_main_context_new();
    g_main_context_push_thread_default(context);

    m_mainLoop = g_main_loop_new(context, FALSE);

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

    m_channel =  g_io_channel_unix_new(m_socketFd);
    m_socketWatch = g_io_add_watch_full(m_channel, G_PRIORITY_DEFAULT, G_IO_IN,
                                        onIncomingDataCb, this, NULL);

    g_main_loop_run(m_mainLoop);
}

gboolean WebosSurfaceManagerClient::onIncomingDataCb(GIOChannel *channel, GIOCondition condition, gpointer user_data)
{
    WebosSurfaceManagerClient *client = reinterpret_cast<WebosSurfaceManagerClient*>(user_data);
    client->onIncomingData();
    return TRUE;
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
        g_warning("%s: unhandled message type %i for window %i",
                  __PRETTY_FUNCTION__, hdr.command, hdr.windowId);
    }
}

void WebosSurfaceManagerClient::postBuffer(int winId, OffscreenNativeWindowBuffer *buffer)
{
    g_message("%s: winId=%i index=%i", __PRETTY_FUNCTION__, winId, buffer->index());
    WebosMessageHeader hdr;
    int ret;

    memset(&hdr, 0, sizeof(WebosMessageHeader));
    hdr.windowId = winId;
    hdr.command = WEBOS_MESSAGE_TYPE_POST_BUFFER;

    ret = write(m_socketFd, &hdr, sizeof(WebosMessageHeader));

    buffer->writeToFd(m_socketFd);
}
