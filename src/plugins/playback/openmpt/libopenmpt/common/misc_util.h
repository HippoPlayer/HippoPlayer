/*
 * misc_util.h
 * -----------
 * Purpose: Various useful utility functions.
 * Notes  : (currently none)
 * Authors: OpenMPT Devs
 * The OpenMPT source code is released under the BSD license. Read LICENSE for more details.
 */


#pragma once

#include "BuildSettings.h"

#include "mptAssert.h"
#include "mptBaseMacros.h"
#include "mptBaseTypes.h"
#include "mptBaseUtils.h"
#include "mptString.h"

// old
#include "mptBaseUtils.h"
#include "mptSpan.h"
#include "mptMemory.h"
#include "mptExceptionText.h"
#include "mptStringFormat.h"
#include "mptStringParse.h"
#include "mptOS.h"
#include "mptTime.h"
#include "mptLibrary.h"

#include <stdexcept>
#include <optional>
#include <vector>

#include <cstdlib>

#include <stdlib.h>


OPENMPT_NAMESPACE_BEGIN


namespace Util
{

	// Insert a range of items [insStart,  insEnd], and possibly shift item fix to the left.
	template<typename T>
	void InsertItem(const T insStart, const T insEnd, T &fix)
	{
		MPT_ASSERT(insEnd >= insStart);
		if(fix >= insStart)
		{
			fix += (insEnd - insStart + 1);
		}
	}

	// Insert a range of items [insStart,  insEnd], and possibly shift items in range [fixStart, fixEnd] to the right.
	template<typename T>
	void InsertRange(const T insStart, const T insEnd, T &fixStart, T &fixEnd)
	{
		MPT_ASSERT(insEnd >= insStart);
		const T insLength = insEnd - insStart + 1;
		if(fixStart >= insEnd)
		{
			fixStart += insLength;
		}
		if(fixEnd >= insEnd)
		{
			fixEnd += insLength;
		}
	}

	// Delete a range of items [delStart,  delEnd], and possibly shift item fix to the left.
	template<typename T>
	void DeleteItem(const T delStart, const T delEnd, T &fix)
	{
		MPT_ASSERT(delEnd >= delStart);
		if(fix > delEnd)
		{
			fix -= (delEnd - delStart + 1);
		}
	}

	// Delete a range of items [delStart,  delEnd], and possibly shift items in range [fixStart, fixEnd] to the left.
	template<typename T>
	void DeleteRange(const T delStart, const T delEnd, T &fixStart, T &fixEnd)
	{
		MPT_ASSERT(delEnd >= delStart);
		const T delLength = delEnd - delStart + 1;
		if(delStart < fixStart  && delEnd < fixStart)
		{
			// cut part is before loop start
			fixStart -= delLength;
			fixEnd -= delLength;
		} else if(delStart < fixStart  && delEnd < fixEnd)
		{
			// cut part is partly before loop start
			fixStart = delStart;
			fixEnd -= delLength;
		} else if(delStart >= fixStart && delEnd < fixEnd)
		{
			// cut part is in the loop
			fixEnd -= delLength;
		} else if(delStart >= fixStart && delStart < fixEnd && delEnd > fixEnd)
		{
			// cut part is partly before loop end
			fixEnd = delStart;
		}
	}

} // namespace Util



namespace Util
{

	template<typename T, std::size_t n>
	class fixed_size_queue
	{
	private:
		T buffer[n+1];
		std::size_t read_position;
		std::size_t write_position;
	public:
		fixed_size_queue() : read_position(0), write_position(0)
		{
			return;
		}
		void clear()
		{
			read_position = 0;
			write_position = 0;
		}
		std::size_t read_size() const
		{
			if ( write_position > read_position )
			{
				return write_position - read_position;
			} else if ( write_position < read_position )
			{
				return write_position - read_position + n + 1;
			} else
			{
				return 0;
			}
		}
		std::size_t write_size() const
		{
			if ( write_position > read_position )
			{
				return read_position - write_position + n;
			} else if ( write_position < read_position )
			{
				return read_position - write_position - 1;
			} else
			{
				return n;
			}
		}
		bool push( const T & v )
		{
			if ( !write_size() )
			{
				return false;
			}
			buffer[write_position] = v;
			write_position = ( write_position + 1 ) % ( n + 1 );
			return true;
		}
		bool pop() {
			if ( !read_size() )
			{
				return false;
			}
			read_position = ( read_position + 1 ) % ( n + 1 );
			return true;
		}
		T peek() {
			if ( !read_size() )
			{
				return T();
			}
			return buffer[read_position];
		}
		const T * peek_p()
		{
			if ( !read_size() )
			{
				return nullptr;
			}
			return &(buffer[read_position]);
		}
		const T * peek_next_p()
		{
			if ( read_size() < 2 )
			{
				return nullptr;
			}
			return &(buffer[(read_position+1)%(n+1)]);
		}
	};

} // namespace Util


namespace Util
{

std::vector<std::byte> HexToBin(const mpt::ustring &src);
mpt::ustring BinToHex(mpt::const_byte_span src);
template <typename T> inline mpt::ustring BinToHex(mpt::span<T> src) { return Util::BinToHex(mpt::byte_cast<mpt::const_byte_span>(src)); }

class base64_parse_error : public std::runtime_error
{
public:
	base64_parse_error()
		: std::runtime_error("invalid Base64 encoding")
	{
	}
};

std::vector<std::byte> Base64ToBin(const mpt::ustring &src);
mpt::ustring BinToBase64(mpt::const_byte_span src);
template <typename T> inline mpt::ustring BinToBase64(mpt::span<T> src) { return Util::BinToBase64(mpt::byte_cast<mpt::const_byte_span>(src)); }

std::vector<std::byte> Base64urlToBin(const mpt::ustring &src);
mpt::ustring BinToBase64url(mpt::const_byte_span src);
template <typename T> inline mpt::ustring BinToBase64url(mpt::span<T> src) { return Util::BinToBase64url(mpt::byte_cast<mpt::const_byte_span>(src)); }

} // namespace Util

namespace Util
{

template <typename T>
class heap_value
{
private:
	std::unique_ptr<T> m_value{};
public:
	template <typename ... Targs>
	heap_value(Targs && ... args)
		: m_value(std::make_unique<T>(std::forward<Targs>(args) ...))
	{
		return;
	}
	const T & value() const
	{
		return *m_value;
	}
	T & value()
	{
		return *m_value;
	}
};

} // namespace Util

#if defined(MODPLUG_TRACKER) || (defined(LIBOPENMPT_BUILD) && defined(LIBOPENMPT_BUILD_TEST))

namespace mpt
{

// Wrapper around std::getenv.
std::optional<mpt::ustring> getenv(const mpt::ustring &env_var);

} // namespace mpt

#endif // MODPLUG_TRACKER || (LIBOPENMPT_BUILD && LIBOPENMPT_BUILD_TEST)


#if MPT_OS_WINDOWS

template <typename Tstring, typename Tbuf, typename Tsize>
Tstring ParseMaybeNullTerminatedStringFromBufferWithSizeInBytes(const Tbuf *buf, Tsize sizeBytes)
{
	// REG_SZ may contain a single NUL terminator, multiple NUL terminators, or no NUL terminator at all
	return Tstring(reinterpret_cast<const typename Tstring::value_type*>(buf), reinterpret_cast<const typename Tstring::value_type*>(buf) + (sizeBytes / sizeof(typename Tstring::value_type))).c_str();
}


#endif // MPT_OS_WINDOWS


OPENMPT_NAMESPACE_END
