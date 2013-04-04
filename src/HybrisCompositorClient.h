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

#include <QObject>
#include <QSocketNotifier>
#include <glib.h>
#include <string>

#include <EGL/eglhybris.h>

class HybrisCompositorClient : QObject
{
    Q_OBJECT
public:
    HybrisCompositorClient(QObject *parent = 0);
    ~HybrisCompositorClient();

    void postBuffer(int winId, OffscreenNativeWindowBuffer *buffer);

signals:
    void serverDisconnected();
    void serverConnected();

private slots:
    void onIncomingData();

private:
    int m_socketFd;
    std::string m_socketPath;
    QSocketNotifier *m_socketNotifier;

private Q_SLOTS:
    void init();
};

#endif /* HYBRISCOMPOSITORCLIENT_H */
