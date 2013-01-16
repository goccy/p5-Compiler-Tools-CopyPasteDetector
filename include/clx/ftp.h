/* ------------------------------------------------------------------------- */
/*
 *  ftp.h
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
 *  Last-modified: Tue 23 Dec 2008 09:26:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_FTP_H
#define CLX_FTP_H

#include "config.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <deque>
#include <vector>
#include <stdexcept>
#include "tcp.h"
#include "resolver.h"
#include "lexical_cast.h"
#include "strip.h"
#include "split.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  ftp_error
	/* --------------------------------------------------------------------- */
	class ftp_error : public std::runtime_error {
	public:
		typedef char char_type;
		typedef std::string string_type;
		
		explicit ftp_error(int code, const string_type& what_arg) :
			std::runtime_error(what_arg), code_(code) {}
		
		virtual ~ftp_error() throw() {}
		
		int code() const { return code_; }
		
	private:
		int code_;
	};
	
	/* --------------------------------------------------------------------- */
	//  basic_ftp
	/* --------------------------------------------------------------------- */
	template <
		class Socket,
		class Acceptor
	>
	class basic_ftp : public Socket {
	public:
		typedef Socket socket_type;
		typedef Acceptor acceptor_type;
		typedef char char_type;
		typedef std::basic_string<char_type> string_type;
		typedef std::pair<int, string_type> response_type;
		typedef std::deque<response_type> response_array;
		
		// control parameter about transfer and file type
		enum { active = 0x01, passive = 0x02 };
		enum { ascii = 0x04, binary = 0x08 };
		
		basic_ftp() : socket_type(), host_(), port_(0), dport_(32767) {}
		
		basic_ftp(const basic_ftp& cp) :
			socket_type(cp), host_(cp.host_), port_(cp.port_), dport_(32767) {}
		
		explicit basic_ftp(const string_type& host, int port = 21) :
			host_(host), port_(port), dport_(32767) {
			this->start();
		}
		
		explicit basic_ftp(const char_type* host, int port = 21) :
			host_(host), port_(port), dport_(32767) {
			this->start();
		}
		
		virtual ~basic_ftp() throw() { this->finish(); }
		
		basic_ftp& start() {
			if (!this->is_open()) this->connect(host_, port_);
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			this->xresponse(ss);
			return *this;
		}
		
		basic_ftp& start(const string_type& host, int port = 21) {
			host_ = host;
			port_ = port;
			return this->start();
		}
		
		basic_ftp& start(const char_type* host, int port = 21) {
			string_type tmp(host);
			return this->start(tmp, port);
		}
		
		void finish() {
			if (this->is_open()) {
				this->xcommand("QUIT");
				this->close();
			}
		}
		
		basic_ftp& login(const string_type& id, const string_type& pass) {
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			this->xcommand(ss, "USER " + id);
			this->xcommand(ss, "PASS " + pass);
			return *this;
		}
		
		basic_ftp& login(const char_type* id, const char_type* pass) {
			string_type tmp_id(id);
			string_type tmp_pass(pass);
			return this->login(tmp_id, tmp_pass);
		}
		
		basic_ftp& reset() {
			this->xcommand("REIN");
			res_.clear();
			return *this;
		}
		
		basic_ftp& cd(const string_type& dir) {
			return this->xcommand("CWD " + dir);
		}
		
		basic_ftp& cdup() {
			return this->xcommand("CDUP");
		}
		
		basic_ftp& rename(const string_type& from, const string_type& to) {
			this->xcommand("RNFR " + from);
			return this->xcommand("RNTO " + to);
		}
		
		basic_ftp& remove(const string_type& file) {
			return this->xcommand("DELE " + file);
		}
		
		basic_ftp& rmdir(const string_type& dir) {
			return this->xcommand("RMD " + dir);
		}
		
		basic_ftp& mkdir(const string_type& dir) {
			return this->xcommand("MKD " + dir);
		}
		
		basic_ftp& command(const string_type& cmd) {
			return this->xcommand("SITE " + cmd);
		}
		
		basic_ftp& noop() {
			return this->xcommand("NOOP");
		}
		
		// command related with getting information.
		string_type pwd() {
			this->xcommand("PWD");
			return res_.begin()->second;
		}
		
		string_type system() {
			this->xcommand("SYST");
			return res_.begin()->second;
		}
		
		string_type status(const string_type& name = string_type()) {
			if (!name.empty()) this->xcommand("STAT " + name);
			else this->xcommand("STAT");
			typename response_array::iterator pos = res_.begin();
			++pos;
			return pos->second;
		}
		
		string_type list(const string_type& name = string_type(), int mode = passive) {
			socket_type sock = this->xtransfer(ascii, mode);
			
			if (name.empty()) this->xcommand("LIST");
			else this->xcommand("LIST " + name);
			
			std::basic_stringstream<char_type> dest;
			char buf[1024];
			while (1) {
				std::memset(buf, 0, sizeof(buf));
				size_t len = sock.recv(buf, sizeof(buf) - 1);
				dest << buf;
				if (len < sizeof(buf) - 1) break;
			}
			sock.close();
			
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			this->xresponse(ss);
			
			return dest.str();
		}
		
		basic_ftp& retrieve(const string_type& from, const string_type& to, int type = ascii, int mode = passive) {
			socket_type sock = this->xtransfer(type, mode);
			
			std::ios_base::openmode flag = std::ios_base::out;
			if (type == binary) flag |= std::ios_base::binary;
			std::basic_ofstream<char_type> fs(to.c_str(), flag);
			
			this->xcommand("RETR " + from);
			char buf[32768];
			size_t received = 0;
			do {
				received = sock.recv(buf, sizeof(buf) - 1);
				fs.write(&buf[0], received);
			} while (received > 0);
			sock.close();
			fs.close();
			
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			this->xresponse(ss);
			
			return *this;
		}
		
		basic_ftp& store(const string_type& from, const string_type& to, int type = ascii, int mode = passive) {
			socket_type sock = this->xtransfer(type, mode);
			
			std::ios_base::openmode flag = std::ios_base::in;
			if (type == binary) flag |= std::ios_base::binary;
			std::basic_ifstream<char_type> fs(from.c_str(), flag);
			
			this->xcommand("STOR " + to);
			char buf[32768];
			do {
				fs.read(&buf[0], sizeof(buf));
				sock.send(buf, fs.gcount());
			} while (fs.gcount() > 0);
			sock.close();
			fs.close();
			
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			this->xresponse(ss);
			
			return *this;
		}
		
		// set address about data connection
		int& port() { return dport_; }
		int port() const { return dport_; }
		
		// get response messages from the server
		response_array& responses() { return res_; }
		const response_array& responses() const { return res_; }
		const response_type& response(size_t index) { return res_.at(index); }
		
	private:
		string_type host_;
		int port_;
		string_type daddr_;
		int dport_;
		response_array res_;
		
		template <class InputStream>
		void xresponse(InputStream& sin) {
			std::basic_stringstream<char_type> dest;
			string_type buf;
			while (std::getline(sin, buf)) {
				chomp(buf);
				int code;
				
				try {
					code = lexical_cast<int>(buf.substr(0, 3));
					if (!dest.str().empty()) dest << std::endl;
					dest << buf.substr(4);
				}
				catch (clx::bad_lexical_cast&) {
					if (!dest.str().empty()) dest << std::endl;
					dest << buf;
					continue;
				}
				
				if (buf[3] == ' ') {
					if (code >= 400) throw ftp_error(code, dest.str());
					response_type tmp(code, dest.str());
					res_.push_front(tmp);
					break;
				}
			}
		}
		
		template <class IOStream>
		basic_ftp& xcommand(IOStream& ss, const string_type& cmd) {
			ss << (cmd + "\r\n");
			this->xresponse(ss);
			return *this;
		}
		
		basic_ftp& xcommand(const string_type& cmd) {
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_iostream<char_type> ss(&sbuf);
			return this->xcommand(ss, cmd);
		}
		
		// for data connection
		socket_type xtransfer(int type, int mode) {
			string_type buf;
			
			if (type == ascii) buf = "TYPE A";
			else buf = "TYPE I";
			this->xcommand(buf);
			
			if (mode == active) return this->xactive(dport_);
			else return this->xpassive();
		}
		
		socket_type xactive(int port) {
			std::vector<string_type> v;
			split_if(localhost(*this), v, is_any_of("."));
			if (v.size() != 4) throw std::runtime_error("invalid IP addres");
			
			std::basic_stringstream<char_type> s;
			for (size_t i = 0; i < v.size(); ++i) s << v.at(i) << ",";
			s << port / 256 << "," << port % 256;
			
			this->xcommand("PORT " + s.str());
			acceptor_type serv(port);
			
			return serv.accept();
		}
		
		socket_type xpassive() {
			this->xcommand("PASV");
			
			// extract the IP address and port number
			string_type buf = res_.begin()->second;
			buf.erase(0, buf.find('(') + 1);
			buf.erase(buf.find(')'));
			std::vector<string_type> v;
			split_if(buf, v, is_any_of(", "));
			if (v.size() != 6) throw ftp_error(500, "invalid response of PASV");
			
			string_type ipaddr = v.at(0) + "." + v.at(1) + "." + v.at(2) + "." + v.at(3);
			int port = lexical_cast<int>(v.at(4)) * 256 + lexical_cast<int>(v.at(5));
			return socket_type(ipaddr, port);
		}
	};
	
	typedef basic_ftp<tcp::socket, tcp::acceptor> ftp;
}

#endif // CLX_FTP_H
