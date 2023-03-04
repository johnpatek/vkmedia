/*
** Copyright 2023 John R. Patek Sr.
**
** Permission is hereby granted, free of charge, to any person obtaining a copy
** of this software and associated documentation files (the "Software"), to deal
** in the Software without restriction, including without limitation the rights
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
** copies of the Software, and to permit persons to whom the Software is
** furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in all
** copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
** SOFTWARE.
*/

#ifndef VKM_SERVER_H
#define VKM_SERVER_H

#include "stream.h"

typedef struct
{
    int rtsp_port;   
} vkm_server_parameters;

typedef vkm_handle vkm_server;

int vkm_server_create(const vkm_server_parameters * server_parameters, vkm_server *server);

void vkm_server_destroy(vkm_server server);

int vkm_server_start(vkm_server server);

int vkm_server_stop(vkm_server server);

int vkm_server_add_stream(vkm_server server, const char * path, vkm_stream stream);

int vkm_server_remove_stream(vkm_server server, const char * path);

#endif
