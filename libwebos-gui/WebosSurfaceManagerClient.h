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

#include <EGL/egl.h>

class OffscreenNativeWindowBuffer;

class IBufferManager
{
public:
    virtual void resize(unsigned int width, unsigned int height) = 0;
    virtual void releaseBuffer(unsigned int index) = 0;
};

class IWebosEglWindow
{
public:
    virtual void identify(unsigned int windowId) = 0;
    virtual void resize(unsigned int width, unsigned int height) = 0;
    virtual EGLNativeWindowType getNativeWindow() = 0;
};

class WebosSurfaceManagerClient: public IWebosEglWindow
{
public:
    WebosSurfaceManagerClient();
    ~WebosSurfaceManagerClient();

    void setBufferManager(IBufferManager *manager);
    void init();

    static void CreateNativeWindow(IWebosEglWindow *&oWebosNativeWindow);

    /* IWebosEglWindow interface */
    void identify(unsigned int winId);
    void resize(unsigned int width, unsigned int height);
    EGLNativeWindowType getNativeWindow();

    void postBuffer(OffscreenNativeWindowBuffer *buffer);

private:
    int m_socketFd;
    gchar *m_socketPath;
    GThread *m_thread;
    IBufferManager *m_bufferManager;
    EGLNativeWindowType m_NativeWindow;

private:
    static gpointer startupCallback(gpointer user_data);
    void onIncomingData();
    void startup();
};

#endif /* HYBRISCOMPOSITORCLIENT_H */
