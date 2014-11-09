msg_control
----------

These helper functions manipulate socket ancillary data. They deal with
msg_control and msg_conrollen member of msghdr and leave the rest of the
structure alone.

```
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
ssize_t msg_control_recv (int fd, struct msghdr *hdr, int *flags);
```

This project is licensed under MIT/X11 license.


