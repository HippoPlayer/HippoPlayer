/*
 * mptFileIO.h
 * -----------
 * Purpose: A wrapper around std::fstream, enforcing usage of mpt::PathString.
 * Notes  : You should only ever use these wrappers instead of plain std::fstream classes.
 * Authors: OpenMPT Devs
 * The OpenMPT source code is released under the BSD license. Read LICENSE for more details.
 */

#pragma once

#include "BuildSettings.h"

#if defined(MPT_ENABLE_FILEIO)

#include "../common/mptString.h"
#include "../common/mptPathString.h"
#include "../common/mptIO.h"

#if defined(MPT_FSTREAM_NO_WCHAR)
#if MPT_GCC_AT_LEAST(9,1,0)
#include <filesystem>
#endif // MPT_GCC_AT_LEAST(9,1,0)
#endif // MPT_FSTREAM_NO_WCHAR
#include <fstream>
#include <ios>
#include <ostream>
#include <streambuf>
#include <utility>

#if MPT_COMPILER_MSVC
#include <cstdio>
#endif // !MPT_COMPILER_MSVC

#ifdef MODPLUG_TRACKER
#if MPT_OS_WINDOWS
#include <windows.h>
#endif // MPT_OS_WINDOWS
#endif // MODPLUG_TRACKER

#endif // MPT_ENABLE_FILEIO


OPENMPT_NAMESPACE_BEGIN


#if defined(MPT_ENABLE_FILEIO)


// Sets the NTFS compression attribute on the file or directory.
// Requires read and write permissions for already opened files.
// Returns true if the attribute has been set.
// In almost all cases, the return value should be ignored because most filesystems other than NTFS do not support compression.
#ifdef MODPLUG_TRACKER
#if MPT_OS_WINDOWS
bool SetFilesystemCompression(HANDLE hFile);
bool SetFilesystemCompression(int fd);
bool SetFilesystemCompression(const mpt::PathString &filename);
#endif // MPT_OS_WINDOWS
#endif // MODPLUG_TRACKER


namespace mpt
{

namespace detail
{

template<typename Tbase>
inline void fstream_open(Tbase & base, const mpt::PathString & filename, std::ios_base::openmode mode)
{
	#if defined(MPT_FSTREAM_NO_WCHAR)
		#if MPT_GCC_AT_LEAST(9,1,0)
			base.open(static_cast<std::filesystem::path>(filename.AsNative()), mode);
		#else // !MPT_GCC_AT_LEAST(9,1,0)
			// Warning: MinGW with GCC earlier than 9.1 detected. Standard library does neither provide std::fstream wchar_t overloads nor std::filesystem with wchar_t support. Unicode filename support is thus unavailable.
			base.open(mpt::ToCharset(mpt::Charset::Locale, filename.AsNative()).c_str(), mode);
		#endif // MPT_GCC_AT_LEAST(9,1,0)
	#else // !MPT_FSTREAM_NO_WCHAR
		base.open(filename.AsNativePrefixed().c_str(), mode);
	#endif // MPT_FSTREAM_NO_WCHAR
}

} // namespace detail

class SafeOutputFile;

// We cannot rely on implicit conversion of mpt::PathString to std::filesystem::path when constructing std::fstream
// because of broken overload implementation in GCC libstdc++ 8, 9, 10.
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=95642
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=90704

class fstream
	: public std::fstream
{
private:
	typedef std::fstream Tbase;
public:
	friend SafeOutputFile;
public:
	fstream() {}
	fstream(const mpt::PathString & filename, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
	{
		detail::fstream_open<Tbase>(*this, filename, mode);
	}
	void open(const mpt::PathString & filename, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out)
	{
		detail::fstream_open<Tbase>(*this, filename, mode);
	}
	void open(const char * filename, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out) = delete;
	void open(const std::string & filename, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out) = delete;
#if MPT_OS_WINDOWS
	void open(const wchar_t * filename, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out) = delete;
	void open(const std::wstring & filename, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out) = delete;
#endif
};

class ifstream
	: public std::ifstream
{
private:
	typedef std::ifstream Tbase;
public:
	friend SafeOutputFile;
public:
	ifstream() {}
	ifstream(const mpt::PathString & filename, std::ios_base::openmode mode = std::ios_base::in)
	{
		detail::fstream_open<Tbase>(*this, filename, mode);
	}
	void open(const mpt::PathString & filename, std::ios_base::openmode mode = std::ios_base::in)
	{
		detail::fstream_open<Tbase>(*this, filename, mode);
	}
	void open(const char * filename, std::ios_base::openmode mode = std::ios_base::in) = delete;
	void open(const std::string & filename, std::ios_base::openmode mode = std::ios_base::in) = delete;
#if MPT_OS_WINDOWS
	void open(const wchar_t * filename, std::ios_base::openmode mode = std::ios_base::in) = delete;
	void open(const std::wstring & filename, std::ios_base::openmode mode = std::ios_base::in) = delete;
#endif
};

class ofstream
	: public std::ofstream
{
private:
	typedef std::ofstream Tbase;
public:
	friend SafeOutputFile;
public:
	ofstream() {}
	ofstream(const mpt::PathString & filename, std::ios_base::openmode mode = std::ios_base::out)
	{
		detail::fstream_open<Tbase>(*this, filename, mode);
	}
#if MPT_COMPILER_MSVC
protected:
	ofstream(FILE * file)
		: std::ofstream(file)
	{
	}

#endif // MPT_COMPILER_MSVC
public:
	void open(const mpt::PathString & filename, std::ios_base::openmode mode = std::ios_base::out)
	{
		detail::fstream_open<Tbase>(*this, filename, mode);
	}
	void open(const char * filename, std::ios_base::openmode mode = std::ios_base::out) = delete;
	void open(const std::string & filename, std::ios_base::openmode mode = std::ios_base::out) = delete;
#if MPT_OS_WINDOWS
	void open(const wchar_t * filename, std::ios_base::openmode mode = std::ios_base::out) = delete;
	void open(const std::wstring & filename, std::ios_base::openmode mode = std::ios_base::out) = delete;
#endif
};

enum class FlushMode
{
	None   = 0,  // no explicit flushes at all
	Single = 1,  // explicitly flush higher-leverl API layers
	Full   = 2,  // explicitly flush *all* layers, up to and including disk write caches
};

inline FlushMode FlushModeFromBool(bool flush)
{
	return flush ? FlushMode::Full : FlushMode::None;
}

#ifdef MODPLUG_TRACKER

class SafeOutputFile
{
private:
	FlushMode m_FlushMode;
#if MPT_COMPILER_MSVC
	std::FILE *m_f = nullptr;
#endif // MPT_COMPILER_MSVC
	mpt::ofstream m_s;
#if MPT_COMPILER_MSVC
	static mpt::tstring convert_mode(std::ios_base::openmode mode, FlushMode flushMode);
	std::FILE * internal_fopen(const mpt::PathString &filename, std::ios_base::openmode mode, FlushMode flushMode);
#endif // MPT_COMPILER_MSVC
public:
	SafeOutputFile() = delete;
	explicit SafeOutputFile(const mpt::PathString &filename, std::ios_base::openmode mode = std::ios_base::out, FlushMode flushMode = FlushMode::Full)
		: m_FlushMode(flushMode)
#if MPT_COMPILER_MSVC
		, m_s(internal_fopen(filename, mode | std::ios_base::out, flushMode))
#else // !MPT_COMPILER_MSVC
		, m_s(filename, mode)
#endif // MPT_COMPILER_MSVC
	{
		if(!stream().is_open())
		{
			stream().setstate(mpt::ofstream::failbit);
		}
	}
	mpt::ofstream& stream()
	{
		return m_s;
	}
	operator mpt::ofstream& ()
	{
		return stream();
	}
	const mpt::ofstream& stream() const
	{
		return m_s;
	}
	operator const mpt::ofstream& () const
	{
		return stream();
	}
	operator bool() const
	{
		return stream() ? true : false;
	}
	bool operator!() const
	{
		return stream().operator!();
	}
	~SafeOutputFile() noexcept(false);
};

#endif // MODPLUG_TRACKER



#ifdef MODPLUG_TRACKER

// LazyFileRef is a simple reference to an on-disk file by the means of a
// filename which allows easy assignment of the whole file contents to and from
// byte buffers.
class LazyFileRef {
private:
	const mpt::PathString m_Filename;
public:
	LazyFileRef(const mpt::PathString &filename)
		: m_Filename(filename)
	{
		return;
	}
public:
	LazyFileRef & operator = (const std::vector<std::byte> &data);
	LazyFileRef & operator = (const std::vector<char> &data);
	LazyFileRef & operator = (const std::string &data);
	operator std::vector<std::byte> () const;
	operator std::vector<char> () const;
	operator std::string () const;
};

#endif // MODPLUG_TRACKER


} // namespace mpt


class InputFile
{
private:
	mpt::PathString m_Filename;
	mpt::ifstream m_File;
	bool m_IsValid;
	bool m_IsCached;
	std::vector<std::byte> m_Cache;
public:
	static bool DefaultToLargeAddressSpaceUsage();
public:
	InputFile(const mpt::PathString &filename, bool allowWholeFileCaching = DefaultToLargeAddressSpaceUsage());
	~InputFile();
	bool IsValid() const;
	bool IsCached() const;
	mpt::PathString GetFilename() const;
	std::istream* GetStream();
	mpt::const_byte_span GetCache();
private:
	bool Open(const mpt::PathString &filename, bool allowWholeFileCaching = DefaultToLargeAddressSpaceUsage());
};


#endif // MPT_ENABLE_FILEIO



OPENMPT_NAMESPACE_END

