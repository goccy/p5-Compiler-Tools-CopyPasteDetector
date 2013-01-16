/* ------------------------------------------------------------------------- */
/*
 *  unix/socket.h
 *
 *  Copyright (c) 2004 - 2008, clown. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    - Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    - Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    - No names of its contributors may be used to endorse or promote
 *      products derived from this software without specific prior written
 *      permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *  Last-modified: Wed 16 Jul 2008 14:38:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_UNIX_SOCKET_H
#define CLX_UNIX_SOCKET_H

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <stdexcept>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "system_error.h"

typedef int socket_int;
typedef socklen_t socklen_type;

namespace clx {
	/* --------------------------------------------------------------------- */
	//  socket_error
	/* --------------------------------------------------------------------- */
	typedef system_error socket_error;
	
	/* --------------------------------------------------------------------- */
	//  open_socket
	/* --------------------------------------------------------------------- */
	inline socket_int open_socket(int family, int type, int flag) {
		return socket(family, type, flag);
	}
	
	/* --------------------------------------------------------------------- */
	//  close_socket
	/* --------------------------------------------------------------------- */
	inline void close_socket(socket_int s) {
		close(s);
	}
	
	/* --------------------------------------------------------------------- */
	//  duplicate_socket
	/* --------------------------------------------------------------------- */
	inline void duplicate_socket(socket_int& dest, socket_int src) {
		dest = dup(src);
	}
	
	/* --------------------------------------------------------------------- */
	//  assign_sockaddr
	/* --------------------------------------------------------------------- */
	inline void assign_sockaddr(struct sockaddr_in& dest, int family, const in_addr& ipaddr, int port) {
		std::memset(&dest, 0, sizeof(dest));
		dest.sin_family = family;
		dest.sin_port = ::htons(port);
		std::memcpy((char*)&dest.sin_addr, &ipaddr, sizeof(ipaddr));
	}
	
	/* --------------------------------------------------------------------- */
	//  assign_sockaddr
	/* --------------------------------------------------------------------- */
	inline void assign_sockaddr(struct sockaddr_in& dest, int family, int port) {
			std::memset((char*)&dest, 0, sizeof(dest));
			dest.sin_family = family;
			dest.sin_port = ::htons(port);
			dest.sin_addr.s_addr = ::htonl(INADDR_ANY);
	}
}

#endif // CLX_UNIX_SOCKET_H
