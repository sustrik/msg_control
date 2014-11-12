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

#include <sys/socket.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

int msg_control_init (struct msghdr *hdr, size_t capacity)
{
    size_t *chunk;

    chunk = malloc (sizeof (size_t) + capacity);
    if (chunk == NULL)
        return -1;
    *chunk = capacity;
    hdr->msg_control = chunk + 1;
    hdr->msg_controllen = 0;

    return 0;
}

void msg_control_term (struct msghdr *hdr)
{
    if (hdr->msg_control)
        free (((size_t*) hdr->msg_control) - 1);
    hdr->msg_control = NULL;
    hdr->msg_controllen = 0;
}

int msg_control_get (struct msghdr *hdr,
     int level, int type, void **data, size_t *size)
{
    struct cmsghdr *cmsg;

    /* Find the property. */
    cmsg = CMSG_FIRSTHDR (hdr);
    while (1) {
        if (!cmsg)
            return -1;
        if (cmsg->cmsg_level == level && cmsg->cmsg_type == type)
            break;
        cmsg = CMSG_NXTHDR (hdr, cmsg);
    }

    /* Return the data. */
    if (data)
        *data = CMSG_DATA (cmsg);
    if (size)
        *size = cmsg->cmsg_len;

    return 0;
}

int msg_control_rm (struct msghdr *hdr, int level, int type)
{
    struct cmsghdr *cmsg;
    char *src;
    size_t sz;

    /* Find the property. */
    cmsg = CMSG_FIRSTHDR (hdr);
    while (1) {
        if (!cmsg)
            return -1;
        if (cmsg->cmsg_level == level && cmsg->cmsg_type == type)
            break;
        cmsg = CMSG_NXTHDR (hdr, cmsg);
    }

    /* Delete the data. */
    /* TODO: We are depending on RFC 3542 extensions to POSIX here! */
    sz = CMSG_SPACE (cmsg->cmsg_len);
    src = ((char*) cmsg) + sz;
    memmove (cmsg, src,
        hdr->msg_controllen - (src - ((char*) hdr->msg_control)));
    hdr->msg_controllen -= sz;
}

int msg_control_set (struct msghdr *hdr,
     int level, int type, void *data, size_t size)
{
    size_t *chunk;
    size_t newsz;
    size_t capacity;
    struct cmsghdr *cmsg;

    /* Remove the property if it already exists. */
    msg_control_rm (hdr, level, type);

    /* Reallocate the buffer to fit the new property. */
    /* TODO: We are depending on RFC 3542 extensions to POSIX here! */
    chunk = ((size_t*) hdr->msg_control) - 1;
    newsz = hdr->msg_controllen + CMSG_SPACE (size);
    capacity = *chunk;
    if (newsz > capacity) {
        while (newsz > capacity)
            capacity *= 2;
        chunk = realloc (chunk, capacity);
        if (!chunk)
            return -1;
        *chunk = capacity;
        hdr->msg_control = chunk + 1;
        hdr->msg_controllen = newsz;
    }

    /* Fill in the new property. */
    cmsg = (struct cmsghdr*)
        (((char*) hdr->msg_control) + hdr->msg_controllen);
    cmsg->cmsg_len = size;
    cmsg->cmsg_level = level;
    cmsg->cmsg_type = type;
    memcpy (CMSG_DATA (cmsg), data, size);

    return 0;
}

int msg_control_get_int (struct msghdr *hdr,
     int level, int type, int *result)
{
    int rc;
    void *data;
    size_t size;

    rc = msg_control_get (hdr, level, type, &data, &size);
    if (rc < 0)
        return rc;
    if (size != sizeof (int))
        return -1;
    if (result)
        *result = *((int*) data);
    return 0;
}

int msg_control_set_int (struct msghdr *hdr, int level, int type, int value)
{
    return msg_control_set (hdr, level, type, &value, sizeof (int));
}

ssize_t msg_control_send (int fd, struct msghdr *hdr, int flags)
{
    return sendmsg (fd, hdr, flags);
}

ssize_t msg_control_recv (int fd, struct msghdr *hdr, int flags)
{
    ssize_t sz;
    size_t oldsz;

    oldsz = hdr->msg_controllen;
    hdr->msg_controllen = *(((size_t*) hdr->msg_control) - 1);
    sz = recvmsg (fd, hdr, flags);
    if (sz < 0)
        hdr->msg_controllen = oldsz;

    return sz;
}

