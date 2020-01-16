#ifndef FILTERINPUT_HPP_INCLUDED
#define FILTERINPUT_HPP_INCLUDED

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/config.hpp>                    
#include <boost/iostreams/categories.hpp>
#include <boost/iostreams/checked_operations.hpp>
#include <boost/iostreams/detail/ios.hpp>          // openmode, streamsize.
#include <boost/iostreams/read.hpp>                // check_eof 
#include <boost/iostreams/pipeline.hpp>
#include <boost/iostreams/write.hpp>
#include <fstream>
#include <iostream>

#include "toolkit_errors.h"

namespace io = boost::iostreams;

using namespace io;

template< typename Ch,
          typename Alloc = std::allocator<Ch> >
class csv_filter {
	private:
		typedef typename std::basic_string<Ch>::traits_type  string_traits;

		bool columns_section;
		bool data_section;

		enum flag_type {
			f_read      = 1,
			f_write     = f_read << 1,
			f_suppress  = f_write << 1
		};

	public:
		typedef Ch                                          char_type;
		typedef char_traits<char_type>                      traits_type;
		typedef std::basic_string<Ch,string_traits,Alloc>   string_type;

		struct category : dual_use, filter_tag, multichar_tag {};

		csv_filter(): columns_section(false), data_section(false), pos_(string_type::npos), flags_(0) {}
		virtual ~csv_filter() {}

		template<typename Source>
		std::streamsize read(Source& src, char_type* s, std::streamsize n) {
			using namespace std;
			BOOST_ASSERT(!(flags_ & f_write));
			flags_ |= f_read;

			std::streamsize result = 0;
			if (!cur_line_.empty() && (result = read_line(s, n)) == n)
				return n;

			typename traits_type::int_type status = traits_type::good();
			while (result < n && !traits_type::is_eof(status)) {
				// Call next_line() to retrieve a line of filtered text, and
				// read_line() to copy it into buffer s.
				if (traits_type::would_block(status = next_line(src)))
					return result;

				result += read_line(s + result, n - result);
			}

			return detail::check_eof(result);
		}
		template<typename Sink>
		std::streamsize write(Sink& snk, const char_type* s, std::streamsize n){
			using namespace std;
			BOOST_ASSERT(!(flags_ & f_read));
			flags_ |= f_write;

			// Handle unfinished business.
			if (pos_ != string_type::npos && !write_line(snk))
				return 0;

			const char_type *cur = s, *next;
			while (true) {
				// Search for the next full line in [cur, s + n), filter it
				// and write it to snk.
				typename string_type::size_type rest = n - (cur - s);
				if ((next = traits_type::find(cur, rest, traits_type::newline()))) {
					cur_line_.append(cur, next - cur);
					cur = next + 1;

					if (!write_line(snk))
						return static_cast<std::streamsize>(cur - s);
				} else {
					cur_line_.append(cur, rest);
					return n;
				}
			}
		}
		template<typename Sink>
		void close(Sink& snk, BOOST_IOS::openmode which) {
			if ((flags_ & f_read) && which == BOOST_IOS::in)
				close_impl();

			if ((flags_ & f_write) && which == BOOST_IOS::out) {
				try {
					if (!cur_line_.empty())
						write_line(snk);
				} catch (...) {
				try {
					close_impl();
				} catch (...) { }
					throw;
				}
				close_impl();
			}
		}
	private:
		std::streamsize read_line(char_type* s, std::streamsize n) {
			using namespace std;
			std::streamsize result = (std::min) (n, static_cast<std::streamsize>(cur_line_.size()));
			traits_type::copy(s, cur_line_.data(), result);
			cur_line_.erase(0, result);
			return result;
		}

		bool valid_csv_line(const string_type& line) {
			if (line.find_first_of("#") == 0 ) {
				return false;
			}
			if (line.starts_with("COLUMNS:")) {
				columns_section = true;
				return false;
			}
			if (line.starts_with("DATA:")) {
				data_section = true;
				return false;
			}
			if (line.starts_with("DATA_FROM:")) {
				throw RBDLToolkitError("RBDL Toolkit does not support DATA_FROM statements in Animation files! Please put your DATA direktly into the file!\n");
			}
			return true;
		}

		// Attempts to retrieve a line of text from the given source; returns
		// an int_type as a good/eof/would_block status code.
		template<typename Source>
		typename traits_type::int_type next_line(Source& src) {
			using namespace std;
			typename traits_type::int_type c;
			bool done = false;
			while (!done) {
				while ( traits_type::is_good(c = io::get(src)) && c != traits_type::newline() ) {
					if (c != traits_type::to_char_type('\r') && c != traits_type::to_char_type('\t'))
						cur_line_ += traits_type::to_int_type(c);
				}
				while (columns_section && !data_section && valid_csv_line(cur_line_)) {
					string_type buffer = "";
					while ( traits_type::is_good(c = io::get(src)) && c != traits_type::newline() ) {
						if (c != traits_type::to_char_type('\r') && c != traits_type::to_char_type('\t'))
							buffer += traits_type::to_int_type(c);
					}
					if (valid_csv_line(buffer)) {
						cur_line_.append(buffer);
					}
				}
				done = valid_csv_line(cur_line_);
				if (!done) {
					cur_line_.erase();
				}
			}
			if (!traits_type::would_block(c)) {
				if (c == traits_type::newline() && (flags_ & f_suppress) == 0)
					cur_line_ += c;
			}
			return c; // status indicator.
		}

		//Filters the current line and attemps to write it to the given sink.
		// Returns true for success.
		template<typename Sink> bool write_line(Sink& snk) {
			if (!valid_csv_line(cur_line_))
				return 0;
			string_type line = cur_line_;
			if ((flags_ & f_suppress) == 0)
				line += traits_type::newline();
				std::streamsize amt = static_cast<std::streamsize>(line.size());
				bool result = io::write_if(snk, line.data(), amt) == amt;
			if (result)
				clear();
			return result;
		}

		void close_impl() {
			clear();
			flags_ &= f_suppress;
		}

		void clear() {
			cur_line_.erase();
			pos_ = string_type::npos;
		}

		string_type                      cur_line_;
		typename string_type::size_type  pos_;
		int                              flags_;
};
BOOST_IOSTREAMS_PIPABLE(csv_filter, 2)

#endif 

