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
#include "OffscreenNativeWindow.h"

class WebosSurfaceManagerClient
{
public:
    WebosSurfaceManagerClient();
    ~WebosSurfaceManagerClient();

    void postBuffer(int winId, OffscreenNativeWindowBuffer *buffer);

    void onIncomingData();

    static gboolean onIncomingDataCb(GIOChannel *channel, GIOCondition condition, gpointer user_data);
    static gboolean initCb(gpointer user_data);

private:
    int m_socketFd;
    gchar *m_socketPath;
    GIOChannel *m_channel;
    guint m_socketWatch;

    void init();
};

#endif /* HYBRISCOMPOSITORCLIENT_H */
