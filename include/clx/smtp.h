/* ------------------------------------------------------------------------- */
/*
 *  smtp.h
 *
 *  Copyright (c) 2004 - 2010, clown. All rights reserved.
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
 *  Last-modified: Mon 15 Mar 2010 23:45:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_SMTP_H
#define CLX_SMTP_H

#include "config.h"
#include <iostream>
#include <string>
#include <deque>
#include <vector>
#include <list>
#include <stdexcept>
#include "tcp.h"
#include "base64.h"
#include "md5.h"
#include "hmac.h"
#include "lexical_cast.h"
#include "strip.h"

namespace clx {
	namespace detail {
		/* ----------------------------------------------------------------- */
		/*
		 *  sendmail
		 *
		 *  The sendmail class is the Functor class for the mail method
		 *  of basic_smtp.
		 */
		/* ----------------------------------------------------------------- */
		class sendmail {
		public:
			typedef char char_type;
			typedef std::basic_string<char_type> string_type;
			
			sendmail() : message_() {}
			explicit sendmail(const string_type& message) : message_(message) {}
			
			virtual ~sendmail() {}
			
			template <class OutputStream>
			void operator()(OutputStream& sout) {
				sout << message_;
			}
			
		private:
			string_type message_;
		};
	}
	
	/* --------------------------------------------------------------------- */
	//  smtp_error
	/* --------------------------------------------------------------------- */
	class smtp_error : public std::runtime_error {
	public:
		typedef char char_type;
		typedef std::string string_type;
		
		explicit smtp_error(int code, const string_type& what_arg) :
			std::runtime_error(what_arg), code_(code) {}
		
		virtual ~smtp_error() throw() {}
		
		int code() const { return code_; }
		
	private:
		int code_;
	};
	
	/* --------------------------------------------------------------------- */
	//  smtp_auth
	/* --------------------------------------------------------------------- */
	namespace smtp_auth {
		enum { none = 0x01, plain = 0x02, login = 0x04, cram_md5 = 0x08, best = 0x0f };
	}
	
	/* --------------------------------------------------------------------- */
	//  basic_smtp
	/* --------------------------------------------------------------------- */
	template <class Socket>
	class basic_smtp : public Socket {
	public:
		typedef Socket socket_type;
		typedef char char_type;
		typedef std::basic_string<char_type> string_type;
		typedef std::pair<int, string_type> response_type;
		typedef std::deque<response_type> response_array;
		
		basic_smtp() :
			socket_type(), host_(), port_(0), auth_(0), res_(),
			sbuf_(sockbuf_type::nbuf) {}
		
		basic_smtp(const basic_smtp& cp) :
			socket_type(cp), host_(cp.host_), port_(cp.port_),
			auth_(0), res_(cp.res_),
			sbuf_(sockbuf_type::nbuf) {}
		
		explicit basic_smtp(const string_type& host, int port = 25) :
			socket_type(), host_(host), port_(port),
			auth_(0), res_(),
			sbuf_(sockbuf_type::nbuf) {
			this->start();
		}
		
		explicit basic_smtp(const char_type* host, int port = 25) :
			socket_type(), host_(host), port_(port),
			auth_(0), res_(),
			sbuf_(sockbuf_type::nbuf) {
			this->start();
		}
		
		virtual ~basic_smtp() throw() { this->finish(); }
		
		basic_smtp& start() {
			if (!this->is_open()) this->connect(host_, port_);
			sbuf_.socket(*this);
			std::basic_iostream<char_type> ss(&sbuf_);
			this->response(ss);
			return *this;
		}
		
		basic_smtp& start(const string_type& host, int port = 25) {
			host_ = host;
			port_ = port;
			return this->start();
		}
		
		basic_smtp& start(const char_type* host, int port = 25) {
			string_type tmp(host);
			return this->start(tmp, port);
		}
		
		void finish() {
			if (this->is_open()) {
				this->command("QUIT");
				this->close();
			}
		}
		
		basic_smtp& login(const string_type& id) {
			this->command("EHLO " + id + " " + host_);
			for (size_t i = 0; i < res_.size(); ++i) {
				if (res_[i].first == 250 && res_[i].second.find("AUTH") != string_type::npos) {
					if (res_[i].second.find("PLAIN") != string_type::npos) auth_ |= smtp_auth::plain;
					if (res_[i].second.find("LOGIN") != string_type::npos) auth_ |= smtp_auth::login;
					if (res_[i].second.find("CRAM-MD5") != string_type::npos) auth_ |= smtp_auth::cram_md5;
				}
			}
			return *this;
		}
		
		basic_smtp& login(const char_type* id) {
			string_type tmp(id);
			return this->login(tmp);
		}
		
		basic_smtp& login(const string_type& id, const string_type& pass, int type = smtp_auth::best) {
			this->login(id);
			
			if ((type & auth_ & smtp_auth::cram_md5) != 0) return this->auth_cram_md5(id, pass);
			else if ((type & auth_ & smtp_auth::plain) != 0) return this->auth_plain(id, pass);
			else if ((type & auth_ & smtp_auth::login) != 0) return this->auth_login(id, pass);
			else if ((type & smtp_auth::none) != 0) return *this;
			
			throw smtp_error(502, "SMTP-AUTH not supported");
			return *this;
		}
		
		basic_smtp& login(const char_type* id, const char_type* pass, int type = smtp_auth::best) {
			string_type tmp_id(id);
			string_type tmp_pass(pass);
			return this->login(tmp_id, tmp_pass, type);
		}
		
#if !defined(__BORLANDC__)
		template <class MailingList, class Functor>
		basic_smtp& mail(const string_type& from, const MailingList& to, Functor f) {
			return this->exec_mail(from, to, f);
		}
		
		template <class Functor>
		basic_smtp& mail(const string_type& from, const string_type& to, Functor f) {
			std::list<string_type> tmp;
			tmp.push_back(to);
			return this->exec_mail(from, tmp, f);
		}
#endif
		
		template <class MailingList>
		basic_smtp& mail(const string_type& from, const MailingList& to, const string_type& message) {
			detail::sendmail f(message);
			return this->exec_mail(from, to, f);
		}
		
		basic_smtp& mail(const string_type& from, const string_type& to, const string_type& message) {
			std::list<string_type> tmp;
			tmp.push_back(to);
			detail::sendmail f(message);
			return this->exec_mail(from, tmp, f);
		}
		
		// get response messages from the server
		response_array& responses() { return res_; }
		const response_array& responses() const { return res_; }
		const response_type& response(size_t index) { return res_.at(index); }
		
	private:
		typedef clx::basic_sockbuf<socket_type, char_type> sockbuf_type;
		
		string_type host_;
		int port_;
		int auth_;
		response_array res_;
		sockbuf_type sbuf_;
		
		template <class InputStream>
		void response(InputStream& sin) {
			std::basic_stringstream<char_type> dest;
			string_type buf;
			while (std::getline(sin, buf)) {
				clx::chomp(buf);
				int code;
				
				try {
					code = clx::lexical_cast<int>(buf.substr(0, 3));
					if (!dest.str().empty()) dest << std::endl;
					dest << buf.substr(4);
				}
				catch (clx::bad_lexical_cast&) {
					throw std::runtime_error("invalid response: " + buf);
				}
				
				if (buf[3] == ' ') {
					if (code >= 400) throw clx::smtp_error(code, dest.str());
					response_type tmp(code, dest.str());
					res_.push_front(tmp);
					break;
				}
			}
		}
		
		template <class IOStream>
		basic_smtp& command(IOStream& ss, const string_type& cmd) {
			ss << (cmd + "\r\n");
			this->response(ss);
			return *this;
		}
		
		basic_smtp& command(const string_type& cmd) {
			std::basic_iostream<char_type> ss(&sbuf_);
			return this->command(ss, cmd);
		}
		
		basic_smtp& auth_plain(const string_type& id, const string_type& pass) {
			std::basic_iostream<char_type> ss(&sbuf_);
			
			std::size_t len = id.size() * 2 + pass.size() + 2;
			std::vector<char> buffer(len, 0);
			std::memcpy(reinterpret_cast<char*>(&buffer.at(0)), id.c_str(), id.size());
			std::memcpy(reinterpret_cast<char*>(&buffer.at(id.size() + 1)), id.c_str(), id.size());
			std::memcpy(reinterpret_cast<char*>(&buffer.at((id.size() + 1) * 2)), pass.c_str(), pass.size());
			
			std::basic_string<char_type> cmd("AUTH PLAIN ");
			clx::base64::encode(buffer.begin(), buffer.end(), std::inserter(cmd, cmd.end()));
			this->command(ss, cmd);
			
			return *this;
		}
		
		basic_smtp& auth_login(const string_type& id, const string_type& pass) {
			std::basic_iostream<char_type> ss(&sbuf_);
			
			this->command(ss, string_type("AUTH LOGIN"));
			this->command(ss, clx::base64::encode(id));
			this->command(ss, clx::base64::encode(pass));
			
			return *this;
		}
		
		basic_smtp& auth_cram_md5(const string_type& id, const string_type& pass) {
			std::basic_iostream<char_type> ss(&sbuf_);
			
			this->command(ss, string_type("AUTH CRAM-MD5"));
			
			string_type buf = base64::decode(this->response(0).second);
			buf = id + " " + hmac<md5>(pass, buf).to_string();
			this->command(ss, base64::encode(buf));
			
			return *this;
		}
		
		template <class MailingList, class Functor>
		basic_smtp& exec_mail(const string_type& from, const MailingList& to, Functor f) {
			std::basic_iostream<char_type> ss(&sbuf_);
			string_type buf;
			
			this->command(ss, "MAIL FROM:<" + from + ">");
			for (typename MailingList::const_iterator pos = to.begin(); pos != to.end(); ++pos) {
				this->command(ss, "RCPT TO:<" + *pos + ">");
			}
			
			this->command(ss, "DATA");
			f(ss);
			
			this->response(ss);
			return *this;
		}
	};
	
	typedef clx::basic_smtp<clx::tcp::socket> smtp;
}

#endif // CLX_SMTP_H
