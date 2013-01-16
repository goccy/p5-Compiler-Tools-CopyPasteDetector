/* ------------------------------------------------------------------------- */
/*
 *  officex.h
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
 *  Last-modified: Tue 18 Aug 2009 23:10:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_OFFICE_H
#define CLX_OFFICE_H

#include "config.h"
#include <cstring>
#include <string>
#include <istream>
#include <fstream>

namespace clx {
	/* --------------------------------------------------------------------- */
	//  is_ole2
	/* --------------------------------------------------------------------- */
	inline bool is_ole2(std::basic_istream<char>& in) {
		static const unsigned char sig[8] = { 0xd0, 0xcf, 0x11, 0xe0, 0xa1, 0xb1, 0x1a, 0xe1 };
		
		unsigned char magic[8];
		in.read(reinterpret_cast<char*>(magic), 8);
		if (in.fail() || in.gcount() < 8) return false;
		if (std::memcmp(magic, sig, 8) != 0) return false;
		
		return true;
	}
	
	/* --------------------------------------------------------------------- */
	//  is_ole2
	/* --------------------------------------------------------------------- */
	inline bool is_ole2(const char* path) {
		std::basic_ifstream<char> fs(path, std::ios::in | std::ios::binary);
		if (!fs.is_open()) return false;
		return clx::is_ole2(fs);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_ole2
	/* --------------------------------------------------------------------- */
	inline bool is_ole2(const std::basic_string<char>& path) {
		return clx::is_ole2(path.c_str());
	}
	
	/* --------------------------------------------------------------------- */
	//  is_doc
	/* --------------------------------------------------------------------- */
	inline bool is_doc(std::basic_istream<char>& in) {
		static const unsigned short wIdent = 0xa5ec;
		
		if (!is_ole2(in)) return false;
		
		in.seekg(512);
		unsigned short magic;
		in.read(reinterpret_cast<char*>(&magic), 2);
		if (in.fail() || in.gcount() < 2) return false;
		if (magic != wIdent) return false;
		
		return true;
	}
	
	/* --------------------------------------------------------------------- */
	//  is_doc
	/* --------------------------------------------------------------------- */
	inline bool is_doc(const char* path) {
		std::basic_ifstream<char> fs(path, std::ios::in | std::ios::binary);
		if (!fs.is_open()) return false;
		return clx::is_doc(fs);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_doc
	/* --------------------------------------------------------------------- */
	inline bool is_doc(const std::basic_string<char>& path) {
		return clx::is_doc(path.c_str());
	}
	
	/* --------------------------------------------------------------------- */
	//  is_xls
	/* --------------------------------------------------------------------- */
	inline bool is_xls(std::basic_istream<char>& in) {
		static const unsigned short sig[] = { // "Workbook"
			0x57, 0x6f, 0x72, 0x6b, 0x62, 0x6f, 0x6f, 0x6b };
		static const unsigned short old[] = { // "Book"
			0x42, 0x6f, 0x6f, 0x6b };
		
		if (!is_ole2(in)) return false;
		
		in.seekg(30);
		unsigned short bits = 0;
		in.read(reinterpret_cast<char*>(&bits), 2);
		if (in.fail() || in.gcount() < 2) return false;
		
		in.seekg(48);
		size_t pos = 0;
		in.read(reinterpret_cast<char*>(&pos), 4);
		if (in.fail() || in.gcount() < 4) return false;
		in.seekg(512 + pos * (1 << bits) + 128);
		
		unsigned short magic[64];
		in.read(reinterpret_cast<char*>(magic), 128);
		if (in.fail() || in.gcount() < 128) return false;
		if (std::memcmp(magic, sig, sizeof(sig)) != 0 &&
			std::memcmp(magic, old, sizeof(old)) != 0) {
			return false;
		}
		
		return true;
	}
	
	/* --------------------------------------------------------------------- */
	//  is_xls
	/* --------------------------------------------------------------------- */
	inline bool is_xls(const char* path) {
		std::basic_ifstream<char> fs(path, std::ios::in | std::ios::binary);
		if (!fs.is_open()) return false;
		return clx::is_xls(fs);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_xls
	/* --------------------------------------------------------------------- */
	inline bool is_xls(const std::basic_string<char>& path) {
		return clx::is_xls(path.c_str());
	}
	
	/* --------------------------------------------------------------------- */
	//  is_ppt
	/* --------------------------------------------------------------------- */
	inline bool is_ppt(std::basic_istream<char>& in) {
		static const unsigned short usr[] = { // "Current User"
			0x43, 0x75, 0x72, 0x72, 0x65, 0x6e, 0x74, 0x20,
			0x55, 0x73, 0x65, 0x72 };
		static const unsigned short sig[] = { // "PowerPoint Document"
			0x50, 0x6f, 0x77, 0x65, 0x72, 0x50, 0x6f, 0x69, 0x6e, 0x74, 0x20,
			0x44, 0x6f, 0x63, 0x75, 0x6d, 0x65, 0x6e, 0x74 };
		static const unsigned short pic[] = { // "Pictures"
			0x50, 0x69, 0x63, 0x74, 0x75, 0x72, 0x65, 0x73 };
		
		if (!is_ole2(in)) return false;
		
		in.seekg(30);
		unsigned short bits = 0;
		in.read(reinterpret_cast<char*>(&bits), 2);
		if (in.fail() || in.gcount() < 2) return false;
		
		in.seekg(48);
		size_t pos = 0;
		in.read(reinterpret_cast<char*>(&pos), 4);
		if (in.fail() || in.gcount() < 4) return false;
		in.seekg(512 + pos * (1 << bits) + 128);
		
		unsigned short magic[64];
		in.read(reinterpret_cast<char*>(magic), 128);
		if (in.fail() || in.gcount() < 128) return false;
		if (std::memcmp(magic, pic, sizeof(pic)) == 0) {
			in.read(reinterpret_cast<char*>(magic), 128);
			if (in.fail() || in.gcount() < 128) return false;
		}
		
		if (std::memcmp(magic, sig, sizeof(sig)) != 0 &&
			std::memcmp(magic, usr, sizeof(usr)) != 0) {
			return false;
		}
		
		return true;
	}
	
	/* --------------------------------------------------------------------- */
	//  is_ppt
	/* --------------------------------------------------------------------- */
	inline bool is_ppt(const char* path) {
		std::basic_ifstream<char> fs(path, std::ios::in | std::ios::binary);
		if (!fs.is_open()) return false;
		return clx::is_ppt(fs);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_ppt
	/* --------------------------------------------------------------------- */
	inline bool is_ppt(const std::basic_string<char>& path) {
		return clx::is_ppt(path.c_str());
	}
}

#endif // CLX_OFFICE_H
