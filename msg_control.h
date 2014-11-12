/*
    Copyright (c) 2014 Martin Sustrik  All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom
    the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.
*/

#ifndef MSG_CONTROL_INCLUDED
#define MSG_CONTROL_INCLUDED

#include <sys/socket.h>
#include <stddef.h>

/* These functions manipulate socket ancillary data. They deal with
   msg_control and msg_conrollen member of msghdr and leave the rest of the
   structure alone. */

/* Allocate buffer for control data. The buffer will be of size zero, however,
   to minimise number of (re)allocations you should specify the capacity in
   such a way as o fit all the ancillary data. Returns 0 in case of success,
   -1 if the buffer cannot be allocated. */
void msg_control_init (struct msghdr *hdr, size_t capacity);

/* Deallocate control data. */
void msg_control_term (struct msghdr *hdr);

/* Return property specified by level and type. If the size of the property
   is known in advance, 'size' may be set to NULL. Returns 0 in case of success,
   -1 if the property in question is not present. */
int msg_control_get (struct msghdr *hdr,
     int level, int type, void **data, size_t *size);

/* Add the property specified by level and type to the buffer. Returns 0 in case
   of success, -1 if additional space in the buffer cannot be allocated. */
int msg_control_set (struct msghdr *hdr,
     int level, int type, void *data, size_t size);

/* Remove the property specified by level and type from the buffer. Returns 0
   in case of success, -1 if the property in question is not present. */
int msg_control_rm (struct msghdr *hdr, int level, int type);

/* Use this function instead of sendmsg(2). */
ssize_t msg_control_send (int fd, struct msghdr *hdr, int flags);

/* Use this function instead of recvmsg(2). */
ssize_t msg_control_recv (int fd, struct msghdr *hdr, int flags);

#endif

