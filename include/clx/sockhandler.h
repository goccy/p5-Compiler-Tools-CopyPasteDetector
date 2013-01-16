/* ------------------------------------------------------------------------- */
/*
 *  sockhandler.h
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
 *  Last-modified: Wed 16 Jul 2008 18:50:10 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_SOCKHANDLER_H
#define CLX_SOCKHANDLER_H

#include "config.h"
#include <string>
#include "tcp.h"
#include "exception.h"

namespace clx {
	namespace tcp {
		/* ----------------------------------------------------------------- */
		/*
		 *  accept_handler
		 *
		 *  This handler accept a new client entry, and then add a task
		 *  to the sockmanager object with the appointed hendler (Service).
		 *  The accept_handler class calls the constuctor of Service class
		 *  with acceptor's socket ID. So if you use the class, implement
		 *  the constructor with the parameter socket_int.
		 */
		/* ----------------------------------------------------------------- */
		template <class Socket, class Service>
		class accept_handler {
		public:
			typedef Socket socket_type;
			typedef clx::tcp::acceptor acceptor_type;
			typedef Service service_handler;
			
			accept_handler() {}
			virtual ~accept_handler() throw() {}
			
			template <class T, class SockManager>
			bool operator()(T* s, SockManager& sm) {
				acceptor_type* server = dynamic_cast<acceptor_type*>(s);
				socket_type* client = new socket_type(server->accept());
				if (client == NULL) throw std::bad_alloc();
				
				sm.add(client, service_handler(server->socket()));
				return true;
			}
		};
		
		/* ----------------------------------------------------------------- */
		/*
		 *  echo_handler
		 *
		 *  Simple example of sockhandler. This handler receives a
		 *  messeage from the other end-host, and then send back the same
		 *  message.
		 */
		/* ----------------------------------------------------------------- */
		class echo_handler {
		public:
			typedef clx::tcp::sockstream socket_type;
			
			echo_handler() : accid_(-1) {}
			echo_handler(socket_int s) : accid_(s) {}
			virtual ~echo_handler() throw() {}
			
			template <class T, class SockManager>
			bool operator()(T* s, SockManager& sm) {
				socket_type* ss = dynamic_cast<socket_type*>(s);
				std::string buf;
				if (!std::getline(*ss, buf)) {
					ss->close();
					return false;
				}
				buf += "\n";
				*ss << buf << std::flush;
				return true;
			}
			
		private:
			socket_int accid_;
		};
	};
}

#endif // CLX_SOCKHANDLER_H
