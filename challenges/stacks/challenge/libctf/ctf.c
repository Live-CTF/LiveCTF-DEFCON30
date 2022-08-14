/*
 * ctf.c | CTF Library (Source File)
 *
 * Copyright (c) 2012-2015 Alexander Taylor <ajtaylor@fuzyll.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "ctf.h"

#ifndef _IPV6
static const int domain = AF_INET;
typedef struct sockaddr_in sockaddr_inX;
#define sinX_port   sin_port
#define sinX_family sin_family
#define sinX_addr   sin_addr.s_addr
#define INXADDR_ANY INADDR_ANY
#else
static const int domain = AF_INET6;
typedef struct sockaddr_in6 sockaddr_inX;
#define sinX_port   sin6_port
#define sinX_family sin6_family
#define sinX_addr   sin6_addr
#define INXADDR_ANY in6addr_any
#endif

/*
 * Reads from a file descriptor until given length is reached.
 * Returns number of bytes received.
 */
int ctf_readn(const int fd, void *msg_, const unsigned int len)
{
    int prev = 0;  // previous amount of bytes we read
    unsigned int count = 0;
    char * msg = (char *) msg_;

    if ((fd >= 0) && msg && len) {
        // keep reading bytes until we've got the whole message
        for (count = 0; count < len; count += prev) {
            prev = read(fd, msg + count, len - count);
            if (prev <= 0) {
#ifdef _DEBUG
                warnx("Unable to read entire message");
#endif
                break;
            }
        }
    }

    return count;
}


/*
 * Reads from a file descriptor until a newline or maximum length is reached.
 * Returns number of bytes read, including the newline (which is now NULL).
 */
int ctf_readsn(const int fd, char *msg, const unsigned int len)
{
    unsigned int count = 0;
    char tmp;  // temporary storage for each character read

    if ((fd >= 0) && msg && len) {
        for (count = 0; count < len; count++) {
            // read character
            if (read(fd, &tmp, 1) <= 0) {
#ifdef _DEBUG
                warnx("Unable to read entire message");
#endif
                break;
            }

            // break loop if we received a newline
            if (tmp == '\n') {
                msg[count] = '\0';
                break;
            }

            // add character to our message
            msg[count] = tmp;
        }
    }

    return count;
}


/*
 * Wrapper for ctf_writen() that does strlen() for you.
 * Returns number of bytes written (or <= 0 for failure).
 */
int ctf_writes(const int fd, const char *msg)
{
    return ctf_writen(fd, msg, strlen(msg));
}


/*
 * Writes a given message of a given length to a given file descriptor.
 * Returns number of bytes written (or <= 0 for failure).
 */
int ctf_writen(const int fd, const void *msg_, const unsigned int len)
{
    int prev = 0;  // previous amount of bytes we wrote
    unsigned int count = 0;
    const char * msg = (const char*) msg_;

    // write entire message (in chunks if we have to)
    if ((fd >= 0) && msg && len) {
        for (count = 0; count < len; count += prev) {
            prev = write(fd, msg + count, len - count);
            if (prev <= 0) {
#ifdef _DEBUG
                warnx("Unable to write entire message");
#endif
                return prev;
            }
        }
    }

    return count;
}
