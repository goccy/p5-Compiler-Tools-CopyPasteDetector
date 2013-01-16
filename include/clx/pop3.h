/* ------------------------------------------------------------------------- */
/*
 *  pop3.h
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
 *  Last-modified: Thu 25 Dec 2008 07:46:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_POP3_H
#define CLX_POP3_H

#include "config.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include "tcp.h"
#include "salgorithm.h"
#include "md5.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  pop3_error
	/* --------------------------------------------------------------------- */
	class pop3_error : public std::runtime_error {
	public:
		typedef char char_type;
		typedef std::string string_type;
		typedef std::stringstream stream_type;
		
		explicit pop3_error(const string_type& command, const string_type& what_arg) :
			std::runtime_error(what_arg), message_(command) {}
		
		virtual ~pop3_error() throw() {}
		
		virtual const char_type* what() {
			message_ = message_ + ": " + std::runtime_error::what();
			return message_.c_str();
		}
		
	private:
		string_type message_;
	};
	
	/* --------------------------------------------------------------------- */
	//  basic_pop3
	/* --------------------------------------------------------------------- */
	template <class Socket>
	class basic_pop3 : public Socket {
	public:
		typedef Socket socket_type;
		typedef char char_type;
		typedef std::basic_string<char_type> string_type;
		typedef std::vector<string_type> list_type;
		
		basic_pop3() : socket_type(), host_(), port_(0), timestamp_() {}
		
		basic_pop3(const basic_pop3& cp) :
			socket_type(cp), host_(cp.host_), port_(cp.port_), timestamp_(cp.timestamp_) {}
		
		explicit basic_pop3(const socket_type& cp) :
			socket_type(cp), host_(), port_(0), timestamp_() {
			host_ = cp.address().ipaddr();
			port_ = cp.address().port();
		}
		
		explicit basic_pop3(const string_type& host, int port = 110) :
			socket_type(), host_(host), port_(port), timestamp_() {
			this->start();
		}
		
		explicit basic_pop3(const char_type* host, int port = 110) :
			socket_type(), host_(host), port_(port), timestamp_() {
			this->start();
		}
		
		virtual ~basic_pop3() throw() { this->finish(); }
		
		basic_pop3& start() {
			if (!this->is_open()) this->connect(host_, port_);
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			
			string_type buf;
			if (!this->xresponse(ss, buf)) throw pop3_error("HELO", buf);
			
			string_type::size_type pos = buf.find("<");
			if (pos != string_type::npos) timestamp_ = buf.substr(pos);
			
			return *this;
		}
		
		basic_pop3& start(const string_type& host, int port = 110) {
			host_ = host;
			port_ = port;
			return this->start();
		}
		
		basic_pop3& start(const char_type* host, int port = 110) {
			string_type tmp(host);
			return this->start(tmp, port);
		}
		
		void finish() {
			if (this->is_open()) {
				string_type buf;
				this->xcommand("QUIT", buf);
				this->close();
			}
		}
		
		basic_pop3& login(const string_type& id, const string_type& pass, bool apop = false) {
			if (apop) return this->apop(id, pass);
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			
			// basic login has 2 steps
			string_type buf;
			if (!this->xcommand(ss, "USER " + id, buf)) throw pop3_error("USER", buf);
			if (!this->xcommand(ss, "PASS " + pass, buf)) throw pop3_error("PASS", buf);
			
			return *this;
		}
		
		basic_pop3& login(const char_type* id, const char_type* pass) {
			string_type tmp_id(id);
			string_type tmp_pass(pass);
			return this->login(tmp_id, tmp_pass);
		}
		
		basic_pop3& apop(const string_type& id, const string_type& pass) {
			if (timestamp_.empty()) throw pop3_error("APOP", "not supported");
			string_type buf = timestamp_ + pass;
			md5 digest(buf);
			if (!this->xcommand("APOP " + id + " " + digest.to_string(), buf)) {
				throw pop3_error("APOP", buf);
			}
			
			return *this;
		}
		
		basic_pop3& apop(const char_type* id, const char_type* pass) {
			string_type tmp_id(id);
			string_type tmp_pass(pass);
			return this->apop(tmp_id, tmp_pass);
		}
		
		string_type status() {
			string_type buf;
			if (!this->xcommand("STAT", buf)) throw pop3_error("STAT", buf);
			return buf.substr(4);
		}
		
		list_type list() {
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			
			string_type buf;
			if (!this->xcommand(ss, "LIST", buf)) throw pop3_error("LIST", buf);
			
			list_type dest;
			while (std::getline(ss, buf)) {
				chomp(buf);
				if (buf[0] == '.') break;
				dest.push_back(buf);
			}
			
			return dest;
		}
		
		string_type list(int n) {
			string_type buf;
			if (!this->xcommand("LIST " + lexical_cast<string_type>(n), buf)) {
				throw pop3_error("LIST", buf);
			}
			return buf.substr(4);
		}
		
		list_type uid_list() {
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			
			string_type buf;
			if (!this->xcommand(ss, "UIDL", buf)) throw pop3_error("UIDL", buf);
			
			list_type dest;
			while (std::getline(ss, buf)) {
				chomp(buf);
				if (buf[0] == '.') break;
				dest.push_back(buf);
			}
			
			return dest;
		}
		
		string_type uid_list(int n) {
			string_type buf;
			if (!this->xcommand("UIDL " + lexical_cast<string_type>(n), buf)) {
				throw pop3_error("UIDL", buf);
			}
			return buf.substr(4);
		}
		
		template <class Functor>
		basic_pop3& retrieve(int n, Functor f) {
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			
			string_type buf;
			if (!this->xcommand(ss, "RETR " + lexical_cast<string_type>(n), buf)) {
				throw pop3_error("RETR", buf);
			}
			
			f(ss);
			return *this;
		}
		
		string_type retrieve(int n) {
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			
			string_type buf;
			if (!this->xcommand(ss, "RETR " + lexical_cast<string_type>(n), buf)) {
				throw pop3_error("RETR", buf);
			}
			
			std::basic_stringstream<char_type> dest;
			while (std::getline(ss, buf)) {
				chomp(buf);
				if (buf[0] == '.') break;
				dest << buf << std::endl;
			}
			
			return dest.str();
		}
		
		string_type top(int n, int line) {
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			
			string_type buf = "TOP " + lexical_cast<string_type>(n) + " " +
				lexical_cast<string_type>(line) + "\r\n";
			if (!this->xcommand(ss, buf, buf)) throw pop3_error("TOP", buf);
			
			std::basic_stringstream<char_type> dest;
			while (std::getline(ss, buf)) {
				chomp(buf);
				if (buf[0] == '.') break;
				dest << buf << std::endl;;
			}
			
			return dest.str();
		}
		
		basic_pop3& erase(int n) {
			string_type buf;
			this->xcommand("DELE " + lexical_cast<string_type>(n), buf);
			return *this;
		}
		
		basic_pop3& reset() {
			string_type buf;
			this->xcommand("RSET", buf);
			return *this;
		}
		
		basic_pop3& noop() {
			string_type buf;
			this->xcommand("NOOP", buf);
			return *this;
		}
		
		bool apop_support() const { return timestamp_.empty() ? false : true; }
		
	private:
		string_type host_;
		int port_;
		string_type timestamp_;
		
		template <class InputStream>
		bool xresponse(InputStream& sin, string_type& dest) {
			dest.clear();
			std::getline(sin, dest);
			chomp(dest);
			if (dest.empty() || dest.find("-ERR") != string_type::npos) return false;
			return true;
		}
		
		template <class IOStream>
		bool xcommand(IOStream& ss, const string_type& cmd, string_type& dest) {
			ss << (cmd + "\r\n");
			return this->xresponse(ss, dest);
		}
		
		bool xcommand(const string_type& cmd, string_type& dest) {
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			return this->xcommand(ss, cmd, dest);
		}
	};
	
	typedef basic_pop3<tcp::socket> pop3;
}

#endif // CLX_POP3_H
