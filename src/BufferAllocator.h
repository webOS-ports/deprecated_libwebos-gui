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

#ifndef BUFFER_ALLOCATOR_H_
#define BUFFER_ALLOCATOR_H_

#include <android/cutils/native_handle.h>
#include <android/hardware/gralloc.h>

class BufferAllocator
{
public:
	static inline BufferAllocator& get() {
		static BufferAllocator instance;
		return instance;
	}

	int alloc(uint32_t width, uint32_t height, uint32_t format, uint32_t usage,
			  buffer_handle_t *handle, int32_t *stride);

	int free(buffer_handle_t handle);

private:
	BufferAllocator();
	~BufferAllocator();

	BufferAllocator(BufferAllocator const&);
	void operator=(BufferAllocator const&);

	alloc_device_t *mAllocDev;
};

#endif
