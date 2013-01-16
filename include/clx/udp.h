/* ------------------------------------------------------------------------- */
/*
 *  udp.h
 *
 *  Copyright (c) 2004 - 2009, clown. All rights reserved.
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
 *  Last-modified: Tue 03 Nov 2009 21:59:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_UDP_H
#define CLX_UDP_H

#include "config.h"
#include <cstdlib>
#include <cstring>
#include <string>
#include "resolver.h"
#include "socket.h"
#include "sockaddress.h"
#include "sockstream.h"
#include "sockmanager.h"

namespace clx {
	namespace udp {
		/* ----------------------------------------------------------------- */
		//  basic_socket
		/* ----------------------------------------------------------------- */
		template <int Family>
		class basic_socket : public basic_rawsocket<SOCK_DGRAM, Family, 0> {
		private:
			typedef basic_rawsocket<SOCK_DGRAM, Family, 0> super;
		public:
			typedef basic_sockaddress<Family, IPPROTO_UDP> address_type;
			typedef char char_type;
			typedef std::basic_string<char> string_type;
			
			// constructor and destructor
			basic_socket() : super(), from_(), to_(), is_bind_(false) {}
			
			basic_socket(const basic_socket& cp) :
				super(cp), from_(cp.from_), to_(cp.to_), is_bind_(cp.is_bind_) {}
			
			basic_socket& operator=(const basic_socket& cp) {
				from_ = cp.from_;
				to_ = cp.to_;
				is_bind_ = cp.is_bind_;
				this->assign(cp);
			}
			
			explicit basic_socket(socket_int s, const address_type& addr) :
				super(s), from_(), to_(addr), is_bind_(false) {}
			
			explicit basic_socket(const char_type* host, int port) :
				super(), from_(), to_(), is_bind_(false) {
				this->connect(host, port);
			}
			
			explicit basic_socket(const string_type& host, int port) :
				super(), from_(), to_(), is_bind_(false) {
				this->connect(host, port);
			}
			
			explicit basic_socket(int port) :
				super(), from_(), to_(), is_bind_(false) {
				this->bind(port);
			}
			
			virtual ~basic_socket() throw() {}
			
			// operator
			basic_socket& connect(const char_type* host, int port) {
				if (this->is_open()) this->close();
				to_.assign(host, port);
				this->open();
				if (!this->is_open()) throw socket_error("open");
				return *this;
			}
			
			basic_socket& connect(const string_type& host, int port) {
				return this->connect(host.c_str(), port);
			}
			
			basic_socket& bind(int port) {
				to_.assign(port);
				if (this->is_open()) this->close();
				this->open();
				
				if (!this->is_open()) throw socket_error("open");
				if (::bind(this->socket(), (struct sockaddr*)to_.data(), to_.size()) == -1) {
					throw socket_error("bind");
				}
				
				is_bind_ = true;
				return *this;
			}
			
			int send_to(const char_type* src, int n, const address_type& addr) {
				return sendto(this->socket(), src, n, 0, (struct sockaddr*)addr.data(), addr.size());
			}
			
			int send_to(const string_type& src, const address_type& addr) {
				return this->send_to(src.c_str(), src.size(), addr);
			}
			
			int send_to(const char_type* src, int n, const char_type* host, int port) {
				address_type addr(host, port);
				return this->send_to(src, n, addr);
			}
			
			int send_to(const string_type& src, const string_type& host, int port) {
				address_type addr(host, port);
				return this->send_to(src, addr);
			}
			
			int send(const char_type* src, int n) {
				int len = -1;
				
				if (!this->is_bind()) len = this->send_to(src, n, to_);
				else len = this->send_to(src, n, from_);
				return len;
			}
			
			int send(const string_type& src) {
				return this->send(src.c_str(), src.size());
			}
			
			int recv(char_type* dest, int n) {
				if (!this->is_bind()) return -1;
				
				typename address_type::inet_type tmp;
				int len = sizeof(tmp);
				std::memset(dest, 0, n);
				
				int ret = recvfrom(this->socket(), dest, n, 0, (struct sockaddr*)&tmp, (socklen_type*)&len);
				if (ret > 0) from_.assign(tmp);
				return ret;
			}
			
			template <std::size_t N>
			int recv(char_type (&dest)[N]) {
				return this->recv(dest, N);
			}
			
			// accessor
			bool is_bind() const { return is_bind_; }
			const address_type& from() const { return from_; }
			const address_type& to() const { return to_; }
			
		private:
			address_type from_;
			address_type to_;
			bool is_bind_;
		};
		
		/* ----------------------------------------------------------------- */
		//  port
		/* ----------------------------------------------------------------- */
		inline int port(const char* service) {
			return clx::port(service, "udp");
		}
		
		inline int port(const std::basic_string<char>& service) {
			return clx::udp::port(service.c_str());
		}
		
		typedef basic_rawsocket<SOCK_DGRAM, AF_INET, 0> rawsocket;
		typedef basic_socket<AF_INET> socket;
		typedef basic_sockaddress<AF_INET, IPPROTO_UDP> sockaddress;
		typedef basic_sockbuf<socket> sockbuf;
		typedef basic_sockstream<socket> sockstream;
		typedef basic_sockmanager<SOCK_DGRAM, AF_INET, 0> sockmanager;
	};
}

#endif // CLX_UDP_H
