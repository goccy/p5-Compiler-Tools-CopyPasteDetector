/* ------------------------------------------------------------------------- */
/*
 *  image.h
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
 *  Last-modified: Wed 19 Aug 2009 01:43:00 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CXL_IMAGE_H
#define CLX_IMAGE_H

#include "config.h"
#include <cstring>
#include <string>
#include <istream>
#include <fstream>

namespace clx {
	/* --------------------------------------------------------------------- */
	//  bmptype
	/* --------------------------------------------------------------------- */
	namespace bmptype {
		enum { both = 0, os2, win };
	}
	
	/* --------------------------------------------------------------------- */
	//  is_bmp
	/* --------------------------------------------------------------------- */
	inline bool is_bmp(std::basic_istream<char>& in, int which = bmptype::both) {
		static const char bm[2] = { 0x42, 0x4d }; // BM
		
		unsigned char magic[2];
		in.read(reinterpret_cast<char*>(magic), 2);
		if (in.fail() || in.gcount() < 2) return false;
		if (std::memcmp(magic, bm, 2) != 0) return false;
		
		if (which == bmptype::os2) {
			size_t size = 0;
			in.read(reinterpret_cast<char*>(&size), 4);
			if (in.fail() || in.gcount() < 4) return false;
			if (size != 12) return false;
		}
		else if (which == bmptype::win) {
			size_t size = 0;
			in.read(reinterpret_cast<char*>(&size), 4);
			if (in.fail() || in.gcount() < 4) return false;
			if (size < 40) return false;
		}
		
		return true;
	}
	
	/* --------------------------------------------------------------------- */
	//  is_bmp
	/* --------------------------------------------------------------------- */
	inline bool is_bmp(const char* path, int which = bmptype::both) {
		std::basic_ifstream<char> fs(path, std::ios::in | std::ios::binary);
		if (!fs.is_open()) return false;
		return clx::is_bmp(fs, which);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_bmp
	/* --------------------------------------------------------------------- */
	inline bool is_bmp(const std::basic_string<char>& path, int which = bmptype::both) {
		return clx::is_bmp(path.c_str(), which);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_png
	/* --------------------------------------------------------------------- */
	inline bool is_png(std::basic_istream<char>& in) {
		static const unsigned char sig[8] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };
		static const unsigned char ihdr[4] = { 0x49, 0x48, 0x44, 0x52 };
		//static const unsigned char idat[4] = { 0x49, 0x44, 0x41, 0x54 };
		static const unsigned char iend[4] = { 0x49, 0x45, 0x4e, 0x44 };
		
		unsigned char magic[8];
		in.read(reinterpret_cast<char*>(magic), 8);
		if (in.fail() || in.gcount() < 8) return false;
		if (std::memcmp(magic, sig, 8) != 0) return false;
		
		in.seekg(4, std::ios_base::cur);
		in.read(reinterpret_cast<char*>(magic), 4);
		if (in.fail() || in.gcount() < 4) return false;
		if (std::memcmp(magic, ihdr, 4) != 0) return false;
		
		in.seekg(-8, std::ios_base::end); // IEND + CRC
		in.read(reinterpret_cast<char*>(magic), 4);
		if (in.fail() || in.gcount() < 4) return false;
		if (std::memcmp(magic, iend, 4) != 0) return false;
		
		return true;
	}
	
	/* --------------------------------------------------------------------- */
	//  is_png
	/* --------------------------------------------------------------------- */
	inline bool is_png(const char* path) {
		std::basic_ifstream<char> fs(path, std::ios::in | std::ios::binary);
		if (!fs.is_open()) return false;
		return clx::is_png(fs);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_png
	/* --------------------------------------------------------------------- */
	inline bool is_png(const std::basic_string<char>& path) {
		return clx::is_png(path.c_str());
	}
	
	/* --------------------------------------------------------------------- */
	//  is_gif
	/* --------------------------------------------------------------------- */
	inline bool is_gif(std::basic_istream<char>& in) {
		static const unsigned char sig[3] = { 0x47, 0x49, 0x46 }; // GIF
		static const unsigned char v87[3] = { 0x38, 0x37, 0x61 }; // 87a
		static const unsigned char v89[3] = { 0x38, 0x39, 0x61 }; // 89a
		static const unsigned char end = 0x3b;
		
		unsigned char magic[3];
		in.read(reinterpret_cast<char*>(magic), 3);
		if (in.fail() || in.gcount() < 3) return false;
		if (std::memcmp(magic, sig, 3) != 0) return false;
		
		in.read(reinterpret_cast<char*>(magic), 3);
		if (in.fail() || in.gcount() < 3) return false;
		if (std::memcmp(magic, v87, 3) != 0 &&
			std::memcmp(magic, v89, 3) != 0) {
			return false;
		}
		
		unsigned char c;
		in.seekg(-1, std::ios_base::end);
		in.read(reinterpret_cast<char*>(&c), 1);
		if (in.fail() || in.gcount() < 1 || c != end) return false;
		
		return true;
	}
	
	/* --------------------------------------------------------------------- */
	//  is_gif
	/* --------------------------------------------------------------------- */
	inline bool is_gif(const char* path) {
		std::basic_ifstream<char> fs(path, std::ios::in | std::ios::binary);
		if (!fs.is_open()) return false;
		return clx::is_gif(fs);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_gif
	/* --------------------------------------------------------------------- */
	inline bool is_gif(const std::basic_string<char>& path) {
		return clx::is_gif(path.c_str());
	}
	
	/* --------------------------------------------------------------------- */
	//  is_jpg
	/* --------------------------------------------------------------------- */
	inline bool is_jpg(std::basic_istream<char>& in) {
		static const unsigned char soi[2] = { 0xff, 0xd8 };
		static const unsigned char eoi[2] = { 0xff, 0xd9 };
		
		unsigned char magic[2];
		in.read(reinterpret_cast<char*>(magic), 2);
		if (in.fail() || in.gcount() < 2) return false;
		if (std::memcmp(magic, soi, 2) != 0) return false;
		
		in.seekg(-2, std::ios_base::end);
		in.read(reinterpret_cast<char*>(magic), 2);
		if (in.fail() || in.gcount() < 2) return false;
		if (std::memcmp(magic, eoi, 2) != 0) return false;
		
		return true;
	}
	
	/* --------------------------------------------------------------------- */
	//  is_jpg
	/* --------------------------------------------------------------------- */
	inline bool is_jpg(const char* path) {
		std::basic_ifstream<char> fs(path, std::ios::in | std::ios::binary);
		if (!fs.is_open()) return false;
		return clx::is_jpg(fs);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_jpg
	/* --------------------------------------------------------------------- */
	inline bool is_jpg(const std::basic_string<char>& path) {
		return clx::is_jpg(path.c_str());
	}
	
	/* --------------------------------------------------------------------- */
	//  is_wmf
	/* --------------------------------------------------------------------- */
	inline bool is_wmf(std::basic_istream<char>& in, bool apm = true) {
		static const unsigned char sig[4] = { 0xd7, 0xcd, 0xc6, 0x9a };
		
		unsigned char magic[4];
		in.read(reinterpret_cast<char*>(magic), 4);
		if (in.fail() || in.gcount() < 4) return false;
		if (apm) {
			if (std::memcmp(magic, sig, 4) != 0) return false;
			else in.seekg(22); // Placeable WMF header size
		}
		else {
			if (std::memcmp(magic, sig, 4) == 0) in.seekg(22);
			else in.seekg(0);
		}
		
		// Standard Windows metafile header
		unsigned short type = 0;
		in.read(reinterpret_cast<char*>(&type), 2);
		if (in.fail() || in.gcount() < 2) return false;
		if (type != 0 && type != 1) return false;
		
		unsigned short size = 0;
		in.read(reinterpret_cast<char*>(&size), 2);
		if (in.fail() || in.gcount() < 2) return false;
		if (size != 9) return false;
		
		unsigned short params = 0;
		in.seekg(12, std::ios_base::cur);
		in.read(reinterpret_cast<char*>(&params), 2);
		if (in.fail() || in.gcount() < 2) return false;
		if (params != 0) return false;
		
		return true;
	}
	
	/* --------------------------------------------------------------------- */
	//  is_wmf
	/* --------------------------------------------------------------------- */
	inline bool is_wmf(const char* path, bool apm = true) {
		std::basic_ifstream<char> fs(path, std::ios::in | std::ios::binary);
		if (!fs.is_open()) return false;
		return clx::is_wmf(fs, apm);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_wmf
	/* --------------------------------------------------------------------- */
	inline bool is_wmf(const std::basic_string<char>& path, bool apm = true) {
		return clx::is_wmf(path.c_str(), apm);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_emf
	/* --------------------------------------------------------------------- */
	inline bool is_emf(std::basic_istream<char>& in) {
		static const unsigned char sig[4] = { 0x20, 0x45, 0x4d, 0x46 };
		
		size_t type = 0;
		in.read(reinterpret_cast<char*>(&type), 4);
		if (in.fail() || in.gcount() < 4) return false;
		if (type != 0x01) return false;
		
		unsigned char magic[4];
		in.seekg(36, std::ios_base::cur);
		in.read(reinterpret_cast<char*>(magic), 4);
		if (in.fail() || in.gcount() < 4) return false;
		if (std::memcmp(magic, sig, 4) != 0) return false;
		
		return true;
	}
	
	/* --------------------------------------------------------------------- */
	//  is_emf
	/* --------------------------------------------------------------------- */
	inline bool is_emf(const char* path) {
		std::basic_ifstream<char> fs(path, std::ios::in | std::ios::binary);
		if (!fs.is_open()) return false;
		return clx::is_emf(fs);
	}
	
	/* --------------------------------------------------------------------- */
	//  is_emf
	/* --------------------------------------------------------------------- */
	inline bool is_emf(const std::basic_string<char>& path) {
		return clx::is_emf(path.c_str());
	}
}

#endif // CLX_IMAGE_H
