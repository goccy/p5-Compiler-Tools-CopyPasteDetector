/* ------------------------------------------------------------------------- */
/*
 *  http.h
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
 *  Last-modified: Tue 18 Aug 2009 11:09:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_HTTP_H
#define CLX_HTTP_H

#include "config.h"
#include <string>
#include <map>
#include <istream>
#include <sstream>
#include "tcp.h"
#include "sockbuf.h"
#include "lexical_cast.h"
#include "split.h"
#include "strip.h"
#include "format.h"
#include "tokenizer.h"

namespace clx {
	/* --------------------------------------------------------------------- */
	//  basic_http
	/* --------------------------------------------------------------------- */
	template <class Socket>
	class basic_http : public Socket {
	public:
		typedef Socket socket_type;
		typedef char char_type;
		typedef std::basic_string<char_type> string_type;
		typedef std::map<string_type, string_type> head_type;
		typedef typename head_type::iterator head_iterator;
		typedef typename head_type::const_iterator const_head_iterator;
		
		basic_http() :
			socket_type(), host_(), port_(0), ver_(1.1),
			code_(0), message_(), head_(), body_() {}
		
		basic_http(const basic_http& cp) :
			socket_type(cp), host_(cp.host_), port_(cp.port_), ver_(cp.ver_),
			code_(cp.code_), message_(cp.message_), head_(cp.head_), body_(cp.body_) {}
		
		explicit basic_http(const socket_type& cp) :
			socket_type(cp), host_(), port_(0), ver_(1.1),
			code_(0), message_(), head_(), body_() {
			host_ = cp.address().ipaddr();
			port_ = cp.address().port();
		}
		
		explicit basic_http(const string_type& host, int port = 80) :
			socket_type(), host_(host), port_(port), ver_(1.1),
			code_(0), message_(), head_(), body_() {
			this->start();
		}
		
		explicit basic_http(const char_type* host, int port = 80) :
			socket_type(), host_(host), port_(port), ver_(1.1),
			code_(0), message_(), head_(), body_() {
			this->start();
		}
		
		virtual ~basic_http() throw() { this->finish(); }
		
		basic_http& start() {
			this->connect(host_, port_);
			return *this;
		}
		
		basic_http& start(const string_type& host, int port = 80) {
			host_ = host;
			port_ = port;
			return this->start();
		}
		
		basic_http& start(const char_type* host, int port = 80) {
			string_type tmp(host);
			return this->start(tmp, port);
		}
		
		void finish() { this->close(); }
		
		void reset() {
			code_ = 0;
			message_.clear();
			head_.clear();
			body_.clear();
		}
		
		basic_http& request(const string_type& query) {
			this->start();
			size_t n = this->send(query);
			if (n != query.size()) throw clx::socket_error("send");
			return *this;
		}
		
		basic_http& request(const char_type* query) {
			string_type tmp(query);
			return this->request(tmp);
		}
		
		bool response(bool full, double timeout = -1.0) {
			this->reset();
			
			if (timeout >= 0.0) {
				fd_set rfds;
				FD_ZERO(&rfds);
				FD_SET(this->socket(), &rfds);
				
				struct timeval tv;
				tv.tv_sec = static_cast<int>(timeout);
				tv.tv_usec = static_cast<int>((timeout - tv.tv_sec) * 1e+06);
				int n = ::select(FD_SETSIZE, &rfds, NULL, NULL, &tv);
				if (n < 0) throw clx::socket_error("select");
				else if (n == 0) return false; // timeout
			}
			
			basic_sockbuf<socket_type, char_type> sbuf(*this);
			std::basic_istream<char_type> ss(&sbuf);
			if (!this->response_status(ss)) return false;
			if (!this->response_head(ss)) return false;
			if (!full) return true;
			
			if (head_.find("Transfer-Encoding") != head_.end() &&
				head_["Transfer-Encoding"] == "chunked") {
				return this->response_chunked(ss);
			}
			else if (head_.find("Content-Length") != head_.end()) {
				int n = lexical_cast<int>(head_["Content-Length"]);
				return this->response_data(ss, n);
			}
			else return this->response_body(ss);
		}
		
		bool response(double timeout = -1.0) {
			return this->response(true, timeout);
		}
		
		bool head(const string_type& path, const head_type& head = head_type(),
			double timeout = -1.0) {
			string_type query = this->make_query("HEAD", path, string_type(), head);
			this->request(query);
			bool res = this->response(false, timeout);
			this->finish();
			return res;
		}
		
		bool head(const char_type* path, const head_type& head = head_type(),
			double timeout = -1.0) {
			string_type tmp(path);
			return this->head(tmp, head, timeout);
		}
		
		bool get(const string_type& path, const head_type& head = head_type(),
			double timeout = -1.0) {
			string_type query = this->make_query("GET", path, string_type(), head);
			this->request(query);
			bool res = this->response(timeout);
			this->finish();
			return res;
		}
		
		bool get(const char_type* path, const head_type& head = head_type(),
			double timeout = -1.0) {
			string_type tmp(path);
			return this->get(tmp, head, timeout);
		}
		
		bool post(const string_type& path, const string_type& data,
			const head_type& head = head_type(), double timeout = -1.0) {
			string_type query = this->make_query("POST", path, data, head);
			this->request(query);
			bool res = this->response(timeout);
			this->finish();
			return res;
		}
		
		bool post(const char_type* path, const char_type* data,
			const head_type& head = head_type(), double timeout = -1.0) {
			string_type tmp_path(path);
			string_type tmp_data(data);
			return this->post(tmp_path, tmp_data, head, timeout);
		}
		
		// accessor
		double& version() { return ver_; }
		double version() const { return ver_; }
		
		int code() const { return code_; }
		const string_type& message() const { return message_; }
		const head_type& head() const { return head_; }
		head_type& head() { return head_; }
		const string_type& body() const { return body_; }
		
	private:
		string_type host_;
		int port_;
		double ver_;
		
		// response fields
		int code_;
		string_type message_;
		head_type head_;
		string_type body_;
		
		// make query string to send the destination server
		string_type make_query(const string_type& method, const string_type& path,
			const string_type& data, const head_type& head) {
			std::basic_stringstream<char_type> query;
			
			query << basic_format<char_type>("%s %s HTTP/%.1f\r\n") % method % path % this->version();
			query << "Host: " << host_ << "\r\n";
			
			for (const_head_iterator pos = head.begin(); pos != head.end(); ++pos) {
				query << pos->first << ": " << pos->second << "\r\n";
			}
			
			if (!data.empty()) {
				query << "Content-Length: " << data.size() << "\r\n";
				query << "\r\n";
				query << data;
			}
			else query << "\r\n";
			
			return query.str();
		}
		
		// get response from the destination server
		template <class InputStream>
		bool response_status(InputStream& sin) {
			string_type line;
			if (!std::getline(sin, line)) return false;
			std::vector<string_type> v;
			split(line, v);
			if (v.size() < 3) return false;
			code_ = lexical_cast<int>(v.at(1));
			message_ = v.at(2);
			return true;
		}
		
		template <class InputStream>
		bool response_head(InputStream& sin) {
			format_separator<char_type> sep("%s: %s");
			basic_tokenizer<format_separator<char_type>, string_type> tok(sep);
			
			string_type line;
			while (std::getline(sin, line)) {
				chomp(line);
				if (line.empty()) break;
				tok.assign(line);
				if (tok.size() != 2) continue;
				head_[tok.at(0)] = tok.at(1);
				
				line.clear();
				tok.reset();
			}
			return true;
		}
		
		template <class InputStream>
		bool response_data(InputStream& sin, int n) {
			if (n == 0) return true;
			
			std::vector<char_type> v(n);
			sin.read(reinterpret_cast<char_type*>(&v.at(0)), n);
			if (sin.gcount() != n) throw std::runtime_error("unmatch data size");
			body_.insert(body_.end(), v.begin(), v.end());
			return true;
		}
		
		template <class InputStream>
		bool response_body(InputStream& sin) {
			std::basic_stringstream<char_type> tmp;
			string_type line;
			while (std::getline(sin, line)) {
				chomp(line);
				tmp << line << std::endl;
				line.clear();
			}
			body_ = tmp.str();
			return true;
		}
		
		template <class InputStream>
		bool response_chunked(InputStream& sin) {
			while (1) {
				string_type line;
				while (line.empty()) {
					if (!std::getline(sin, line)) throw clx::socket_error("read");
					chomp(line);
				}
				int n = clx::lexical_cast<int>(line, std::ios::hex);
				if (n == 0) break;
				this->response_data(sin, n);
			}
			return true;
		}
	};
	
	typedef basic_http<tcp::socket> http;
}

#endif // CLX_HTTP_H
