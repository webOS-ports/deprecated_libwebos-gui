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

#ifndef WEBOSSURFACEMESSAGE_H_
#define WEBOSSURFACEMESSAGE_H_

#include <stdint.h>

enum WebosMessageTypes {
    WEBOS_MESSAGE_TYPE_POST_BUFFER = 1,
    WEBOS_MESSAGE_TYPE_RESTORE_BUFFER = 2,
};

typedef struct {
    uint32_t windowId;
    uint32_t command;
} WebosMessageHeader;

#endif
