/* ------------------------------------------------------------------------- */
/*
 *  win32/filesystem.h
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
 *  Last-modified: Fri 30 Jul 2010 22:21:23 JST
 */
/* ------------------------------------------------------------------------- */
#ifndef CLX_WIN32_FILESYSTEM_H
#define CLX_WIN32_FILESYSTEM_H

#if defined(__BORLANDC__)
#include <stdio.h>
#else
#include <cstdio>
#endif
#include <algorithm>
#include <string>
#include <windows.h>

namespace clx {
	namespace filesystem {
		/* ----------------------------------------------------------------- */
		//  delimiter_api
		/* ----------------------------------------------------------------- */
		inline char delimiter_api(const std::basic_string<char>& hint = std::basic_string<char>()) {
			return (hint.find('/') != std::basic_string<char>::npos) ? '/' : '\\';
		}
		
		/* ----------------------------------------------------------------- */
		//  leaf_api
		/* ----------------------------------------------------------------- */
		inline std::basic_string<char> leaf_api(const std::basic_string<char>& path) {
			const char delim = delimiter_api(path);
			std::basic_string<char>::size_type pos = path.find_last_of(delim);
			if (pos == std::basic_string<char>::npos) return path;
			else if (pos < path.size() - 1) std::basic_string<char>();
			return path.substr(pos + 1);
		}
		
		/* ----------------------------------------------------------------- */
		//  branch_api
		/* ----------------------------------------------------------------- */
		inline std::basic_string<char> branch_api(const std::basic_string<char>& path) {
			const char delim = delimiter_api(path);
			std::basic_string<char>::size_type pos = path.find_last_of(delim);
			if (pos == std::basic_string<char>::npos) return std::basic_string<char>();
			return path.substr(0, pos);
		}
		
		/* ----------------------------------------------------------------- */
		//  exists_api
		/* ----------------------------------------------------------------- */
		inline bool exists_api(const std::basic_string<char>& path) {
			FILE* fp = std::fopen(path.c_str(), "r");
			if (fp) {
				std::fclose(fp);
				return true;
			}
			return false;
		}
		
		/* ----------------------------------------------------------------- */
		//  is_directory_api
		/* ----------------------------------------------------------------- */
		inline bool is_directory_api(const std::basic_string<char>& path) {
			HANDLE hfile = ::CreateFile(path.c_str(), 0, 0, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hfile == INVALID_HANDLE_VALUE) return false;
			
			BY_HANDLE_FILE_INFORMATION info;
			::GetFileInformationByHandle(hfile, &info);
			::CloseHandle(hfile);
			
			if ((info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) return true;
			return false;
		}
		
		/* ----------------------------------------------------------------- */
		//  copy_api
		/* ----------------------------------------------------------------- */
		inline bool copy_api(const std::basic_string<char>& from, const std::basic_string<char>& to, bool fail_if_exists) {
			return ::CopyFileA(from.c_str(), to.c_str(), fail_if_exists) ? true : false;
		}
		
		/* ----------------------------------------------------------------- */
		//  list_api
		/* ----------------------------------------------------------------- */
		template <class OutIter>
		inline OutIter list_api(const std::basic_string<char>& dir, OutIter out) {
			const char delim = '\\';
			
			std::basic_string<char> query(dir);
			std::replace(query.begin(), query.end(), '/', delim);
			if (query.at(query.size() - 1) != '\\') query += delim;
			query += "*.*";
			
			WIN32_FIND_DATA w32fd;
			HANDLE hfind = ::FindFirstFile(query.c_str(), &w32fd);
			if (hfind != INVALID_HANDLE_VALUE) {
				do {
					std::basic_string<char> filename(w32fd.cFileName);
					if ((w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) filename += delim;
					*out = filename;
					++out;
				} while (::FindNextFile(hfind, &w32fd));
				::FindClose(hfind);
			}
			
			return out;
		}
	}
}

#endif // CLX_WIN32_FILESYSTEM_H
