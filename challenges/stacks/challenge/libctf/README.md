# CTF Service Library #

This library is intended to provide a common set of functionality for writing
CTF services. This is defined as setting up a simple forking server listening
on a port that spawns children executing a connection handler with privileges
dropped from root to some service-specific user.

The primary goal is to be as close to Kenshoto, DDTEK, and LBS's
implementations from the DEFCON CTF Finals as possible while retaining a
featureset similar to that of Ghost in the Shellcode's implementation. To this
end, most of this code was written while reverse engineering CTF services from
those competitions.

This library currently explicitly supports services written in C, C++, and D.
Other languages able to interface directly with C should also work, but have
not been tested.


## Usage ##

This library provides the following standard functions as its API:

```
int ctf_listen(const unsigned short port, const int proto, const char *iface)
    Binds a socket to a port and begins listening.

void ctf_server(int sd, const char *user, int (*handler)(int))
    Accepts connections and forks off child processes to handle them.

void ctf_privdrop(const char *user)
    Drops privileges to chosen user.

int ctf_randfd(int old)
    Randomizes a given file descriptor.

int ctf_readn(const int fd, char *msg, const unsigned int len)
    Reads until given length is reached and returns number of bytes read.

int ctf_readsn(const int fd, char *msg, const unsigned int len)
    Reads in a string and returns number of bytes read (including NULL).

int ctf_writen(const int fd, const char *msg, const unsigned int len)
    Writes a message of given length and returns number of bytes written.

int ctf_writes(const int fd, const char *msg)
    Wrapper for ctf_writen() for NULL-terminated strings.

int ctf_writef(const int fd, const char *msg, ...)
    Wrapper for ctf_writen() to allow formatted messages.
```

Please see `ctf.c` for additional comments and documentation.


## Configuration ##

This library supports some compile-time options in the form of DEFINEs.
Supported DEFINEs are:

```
-D_DEBUG
    Removes alarm and dropping privileges in ctf_server().
    Adds a few helpful warnings to read/write functions.

-D_IPV6
    Switches socket from IPV4 to IPV6 in ctf_listen().

-D_SCTP
    Adds support for SCTP sockets in the header file.

-D_CHROOT
    Modifies ctf_privdrop()'s behavior to chroot to the service user's directory when changing to it.

-D_RANDFD
    Enables randomizing the socket descriptor in ctf_server().
```


## Compilation ##

Compiling libctf should be as easy as doing:

```
$(CC) --std=c11 -D_GNU_SOURCE -c ctf.c -o ctf.o
```

Linking libctf should be as easy as (assuming a service named "sample"):

```
$(CC) sample.c ctf.o -o sample
```

Typical usage of this library is to link against a custom-compiled
libctf object on a per-service basis. This allows each service to specify
custom options (listening on IPv4 vs. IPv6, for example). As such, no Makefile
is provided (the above should be enough for you to create your own).


## Examples ##

An example sample.c implementing a basic CTF service using this library
would look something like the following:

```
#include "ctf.h"

const char *USER = "sample";            // user to drop privileges to
const unsigned short PORT = 65535;      // port to bind and listen on

int child_main(int sd)                  // handler for incoming connections
{
    char buf[32];
    int len;
    if ((len = ctf_readsn(sd, buf, 256)) > 0) {
        ctf_writes(sd, buf);
    }
    return 0;
}

int main(int argc, char **argv)         // main function
{
    (void)argc;
    (void)argv;
    int sd;                             // socket descriptor
    sd = ctf_listen(PORT, IPPROTO_TCP, NULL);
    ctf_server(sd, USER, child_main);
    return 0;
}
```

If you're using xinetd or socat instead of binding and listening on a port
yourself, the example would instead look something like:

```
#include "ctf.h"

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    char buf[32];
    int len;
    setvbuf(stdout, NULL, _IONBF, 0);
    if ((len = ctf_readsn(fileno(stdin), buf, 256)) > 0) {
        ctf_writes(fileno(stdout), buf);
    }
    return 0;
}
```

With xinetd, however, you will need to add a configuration file
in /etc/xinetd.d/ that looks like something like this:

```
service sample
{
    id          = sample
    user        = sample
    server      = /home/sample/sample
    disable     = no
    port        = 65535
    socket_type = stream
    protocol    = tcp
    wait        = no
    type        = UNLISTED
}
```


## Questions ##

Answers to some common questions include:

#### Question 1: How can I get a service to chroot when running with xinetd? ####

Originally, I added a separate function to handle this "special case". After
working with it, I realized it could never work that way because of how xinetd
drops privileges. The correct answer is to simply modify your xinetd config's
`server` line to be the following two lines:

```
    server      = /usr/sbin/chroot
    server_args = /home/sample /home/sample/sample
```

The first argument is the directory you'd like to chroot to. The second
argument is the service you'd like to run.

#### Question 2: How can I build libctf as a shared library? ####

Building libctf as a shared library should look something like this:

```
$(CC) --std=c11 -D_GNU_SOURCE -fpic -c ctf.c -o ctf.o
$(CC) ctf.o -shared -o libctf.so
```

Linking against it should look something like:

```
$(CC) sample.c -o sample -L /path/to/libctf.so -lctf
```

Keep in mind that you will need `/path/to/libctf.so` in your LD_LIBRARY_PATH.
Otherwise, you will get an error like:

```
./sample: error while loading shared libraries: libctf.so: cannot open shared object file: No such file or directory
```

As mentioned above, this is not a typical usage scenario because a number of
useful features are provided as compile-time defines. You are, however, free
to do as you wish.

#### Question 3: Do you plan to support languages like C#, Python, and Ruby? ####

I currently have no plans to support extra languages. There's probably some
merit in supporting bytecode-y languages. Truth be told, they might actually
work fine already. I've just never tested them.

* For C#, try compiling libctf as a shared object (see above) and use DllImport
* For Python, try ctypes
* For Ruby, try FFI

Please let me know if you do actually try this out!


## Roadmap ##

The following is a list of features that have yet to be added and/or tested:

* Haven't actually built on FreeBSD yet
* Haven't compiled to any architectures other than x86/x86-64
* Haven't fully tested SCTP and RAW codepaths
* Haven't fully tested D bindings (can't get gdc to link properly)
* Test and/or create bindings to other languages (C#, Java, Python, Ruby, Lua)
* Currently no support whatsoever for Windows services (in Wine or otherwise)
* Haven't yet implemented DDTEK's backdoor stuff from DEFCON CTF Finals 19 and 20
