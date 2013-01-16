/* ------------------------------------------------------------------------- */
/*
 *  icmp.h
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
 *  Last-modified: Tue 03 Nov 2009 21:53:03 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_ICMP_H
#define CLX_ICMP_H

#include "config.h"
#include <cstring>
#include <string>
#include "ip.h"
#include "sockaddress.h"
#include "sockmanager.h"

#define ICMP_ECHO_REPLY 0
#define ICMP_ECHO_REQUEST 8

/* ------------------------------------------------------------------------- */
//  icmphdr
/* ------------------------------------------------------------------------- */
struct icmphdr {
	u_int8_t type;
	u_int8_t code;
	u_int16_t checksum;
	u_int16_t id;
	u_int16_t sequence;
};

namespace clx {
	namespace icmp {
		/* ----------------------------------------------------------------- */
		//  packet_header
		/* ----------------------------------------------------------------- */
		class packet_header {
		public:
			typedef struct iphdr ip_type;
			typedef struct icmphdr icmp_type;
			typedef size_t size_type;
			
			packet_header() : ip_(), icmp_() {}
			
			explicit packet_header(const char* packet) : ip_(), icmp_() {
				this->assign(packet);
			}
			
			virtual ~packet_header() throw() {}
			
			packet_header& operator=(const char* packet) {
				return this->assign(packet);
			}
			
			packet_header& assign(const char* packet) {
				std::memcpy(reinterpret_cast<char*>(&ip_), packet, this->ip_size());
				std::memcpy(reinterpret_cast<char*>(&icmp_), (packet + ip_.ihl * 4), this->icmp_size());
				return *this;
			}
			
			void reset() {
				std::memset(reinterpret_cast<char*>(&ip_), 0, this->ip_size());
				std::memset(reinterpret_cast<char*>(&icmp_), 0, this->icmp_size());
			}
			
			size_type size() const { return (sizeof(ip_) + sizeof(icmp_)); }
			size_type ip_size() const { return sizeof(ip_); }
			size_type icmp_size() const { return sizeof(icmp_); }
			
			ip_type* ip() { return &ip_; }
			icmp_type* icmp() { return &icmp_; }
			
			const ip_type* ip() const { return &ip_; }
			const icmp_type* icmp() const { return &icmp_; }
			
		private:
			ip_type ip_;
			icmp_type icmp_;
		};
		
		/* ----------------------------------------------------------------- */
		//  basic_socket
		/* ----------------------------------------------------------------- */
		template <int Family>
		class basic_socket : public basic_rawsocket<SOCK_RAW, Family, IPPROTO_ICMP> {
		private:
			typedef basic_rawsocket<SOCK_RAW, Family, IPPROTO_ICMP> super;
		public:
			typedef basic_sockaddress<Family, IPPROTO_ICMP> address_type;
			typedef char char_type;
			typedef std::basic_string<char> string_type;
			
			basic_socket() : super(), from_(), to_() {}
			
			basic_socket(const basic_socket& cp) :
				super(cp), from_(cp.from_), to_(cp.to_) {}
			
			basic_socket& operator=(const basic_socket& cp) {
				from_ = cp.from_;
				to_ = cp.to_;
				this->assign(cp);
			}
			
			explicit basic_socket(socket_int s, const address_type& addr) :
				super(s), from_(), to_(addr) {}
			
			explicit basic_socket(const char_type* host) :
				super(), from_(), to_() {
				this->connect(host);
			}
			
			explicit basic_socket(const string_type& host) :
				super(), from_(), to_() {
				this->connect(host);
			}
			
			virtual ~basic_socket() throw() {}
			
			// operator
			basic_socket& connect(const char_type* host) {
				if (this->is_open()) this->close();
				to_.assign(host, 0);
				this->open();
				if (!this->is_open()) throw socket_error("open");
				return *this;
			}
			
			basic_socket& connect(const string_type& host) {
				return this->connect(host.c_str());
			}
			
			int send_to(const char_type* src, int n, const address_type& addr) {
				struct icmphdr* ph = (struct icmphdr*)src;
				ph->checksum = clx::ip::checksum((unsigned short*)src, n);
				return sendto(this->socket(), src, n, 0, (struct sockaddr*)addr.data(), addr.size());
			}
			
			int send_to(const string_type& src, const address_type& addr) {
				return this->send_to(src.c_str(), src.size(), addr);
			}
			
			int send_to(const char_type* src, int n, const char_type* host) {
				address_type addr(host, 0);
				return this->send_to(src, n, addr);
			}
			
			int send_to(const string_type& src, const string_type& host) {
				return this->send_to(src.c_str(), src.size(), host.c_str());
			}
			
			int send(const char_type* src, int n) {
				return this->send_to(src, n, to_);
			}
			
			int send(const string_type& src) {
				return this->send(src.c_str(), src.size());
			}
			
			int recv(char_type* dest, int n) {
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
			const address_type& from() const { return from_; }
			const address_type& to() const { return to_; }
			
		private:
			address_type from_;
			address_type to_;
		};
		
		typedef basic_rawsocket<SOCK_RAW, AF_INET, IPPROTO_ICMP> rawsocket;
		typedef basic_socket<AF_INET> socket;
		typedef basic_sockaddress<AF_INET, IPPROTO_ICMP> sockaddress;
		typedef basic_sockmanager<SOCK_RAW, AF_INET, IPPROTO_ICMP> sockmanager;
	}
}

#endif // CLX_ICMP_H
