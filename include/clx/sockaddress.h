/* ------------------------------------------------------------------------- */
/*
 *  sockaddress.h
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
 *  Last-modified: Tue 03 Nov 2009 20:56:03 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_SOCKADDRESS_H
#define CLX_SOCKADDRESS_H

#include "config.h"
#include <string>
#include <map>
#include <exception>
#include "socket.h"
#include "resolver.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  sockaddress_error
	/* --------------------------------------------------------------------- */
	class sockaddress_error : public std::exception {
	public:
		typedef char char_type;
		typedef std::string string_type;
		
		sockaddress_error() : std::exception(), factor_() {}
		
		explicit sockaddress_error(const string_type& factor) :
			std::exception(), factor_(factor), message_() {
			this->xinit(factor);
		}
		
		explicit sockaddress_error(const char_type* factor) :
			std::exception(), factor_(factor), message_() {
			string_type tmp(factor);
			this->xinit(tmp);
		}
		
		virtual ~sockaddress_error() throw() {}
		
		virtual const char_type* what() { return message_.c_str(); }
		
		const string_type& factor() const { return factor_; }
	
	private:
		string_type factor_;
		string_type message_;
		
		void xinit(const string_type& s) {
			message_ = s + ": Non-existent address or service";
		}
	};
	
	/* --------------------------------------------------------------------- */
	//  basic_sockaddress
	/* --------------------------------------------------------------------- */
	template <
		int Family,
		int Protocol
	>
	class basic_sockaddress {
	public:
		typedef struct sockaddr_in inet_type;
		typedef char char_type;
		typedef std::basic_string<char> string_type;
		typedef size_t size_type;
		
		// constructor and destructor
		basic_sockaddress() : addr_() {}
		
		explicit basic_sockaddress(const inet_type& addr) : addr_(addr) {}
		
		explicit basic_sockaddress(int port) : addr_() {
			this->assign(port);
		}
		
		explicit basic_sockaddress(const char_type* host, int port) : addr_() {
			this->assign(host, port);
		}
		
		explicit basic_sockaddress(const string_type& host, int port) : addr_() {
			this->assign(host, port);
		}
		
		virtual ~basic_sockaddress() throw() {}
		
		// operator
		basic_sockaddress& assign(const inet_type& addr) {
			addr_ = addr;
			return *this;
		}
		
		basic_sockaddress& assign(int port) {
			assign_sockaddr(addr_, Family, port);
			return *this;
		}
		
		basic_sockaddress& assign(const char_type* host, int port, int n = 0) {
			struct in_addr tmp;
			if (!::inet_aton(host, &tmp)) {
				basic_resolver<Family> inet;
				inet.lookup(host);
				if (inet.addresses().empty()) throw sockaddress_error(host);
				tmp = inet.addresses().at(n);
			}
			assign_sockaddr(addr_, Family, tmp, port);
			return *this;
		}
		
		basic_sockaddress& assign(const string_type& host, int port, int n = 0) {
			return this->assign(host.c_str(), port, n);
		}
		
		// get information about socket address
		string_type ipaddr() const {
			string_type tmp(::inet_ntoa(addr_.sin_addr));
			return tmp;
		}
		
		int port() const { return ::ntohs(addr_.sin_port); }
		size_type size() const { return sizeof(addr_); }
		const inet_type* data() const { return &addr_; }
		
	private:
		inet_type addr_;
	};
}

#endif // CLX_SOCKADDRESS_H
