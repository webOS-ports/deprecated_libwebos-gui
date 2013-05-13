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

#ifndef HYBRISCOMPOSITORREMOTECLIENT_H_
#define HYBRISCOMPOSITORREMOTECLIENT_H_

#include <glib.h>

#include "OffscreenNativeWindow.h"

class WebosSurfaceManager;

class WebosSurfaceManagerRemoteClient
{
public:
	WebosSurfaceManagerRemoteClient(WebosSurfaceManager *parent, int socketDescriptor);
	virtual ~WebosSurfaceManagerRemoteClient();

	void onIncomingData();
	static gboolean onIncomingDataCb(GIOChannel *channel, GIOCondition condition, gpointer user_data);

protected:
	virtual void handleIncomingBuffer(int windowId, OffscreenNativeWindowBuffer *buffer);

private:
	WebosSurfaceManager *m_parent;
	int m_socketFd;
	GIOChannel *m_channel;
	gint m_socketWatch;
};

class WebosSurfaceManagerRemoteClientFactory
{
public:
	virtual WebosSurfaceManagerRemoteClient *create(WebosSurfaceManager *parent, int socketFd) = 0;
};

#endif
