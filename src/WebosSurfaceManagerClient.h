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


#ifndef HYBRISCOMPOSITORCLIENT_H
#define HYBRISCOMPOSITORCLIENT_H

#include <glib.h>
#include <string>

#include <EGL/eglhybris.h>

class OffscreenNativeWindowBuffer;

class IBufferManager
{
public:
    virtual void releaseBuffer(unsigned int index) = 0;
};

class WebosSurfaceManagerClient
{
public:
    WebosSurfaceManagerClient(IBufferManager *manager);
    ~WebosSurfaceManagerClient();

    void identify(unsigned int winId);
    void postBuffer(OffscreenNativeWindowBuffer *buffer);

private:
    int m_socketFd;
    gchar *m_socketPath;
    GIOChannel *m_channel;
    guint m_socketWatch;
    GThread *m_thread;
    GMainLoop *m_mainLoop;
    IBufferManager *m_bufferManager;
    GMutex m_socketMutex;

private:
    static gpointer startupCallback(gpointer user_data);
    void onIncomingData();
    void startup();
};

#endif /* HYBRISCOMPOSITORCLIENT_H */
