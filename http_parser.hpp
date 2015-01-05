/* Copyright Joyent, Inc. and other Node contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once


#define HTTP_PARSER_VERSION_MAJOR 1
#define HTTP_PARSER_VERSION_MINOR 0

#include <sys/types.h>
#if defined(_WIN32) && !defined(__MINGW32__)
typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

typedef unsigned int size_t;
typedef int ssize_t;
#else
#include <stdint.h>
#endif

#include <cstdint>

#include <functional>

/* Compile with -DHTTP_PARSER_STRICT=1 to parse URLs and hostnames
 * strictly according to the RFCs
 */
#ifndef HTTP_PARSER_STRICT
# define HTTP_PARSER_STRICT 0
#endif

/* Maximium header size allowed */
#define HTTP_MAX_HEADER_SIZE (80*1024)

/* Map for errno-related constants
 *
 * The provided argument should be a macro that takes 2 arguments.
 */
#define HTTP_ERRNO_MAP(XX)                                           \
  /* No error */                                                     \
  XX(OK, "success")                                                  \
                                                                     \
  /* Callback-related errors */                                      \
  XX(CB_message_begin, "the on_message_begin callback failed")       \
  XX(CB_path, "the on_path callback failed")                         \
  XX(CB_query_string, "the on_query_string callback failed")         \
  XX(CB_url, "the on_url callback failed")                           \
  XX(CB_fragment, "the on_fragment callback failed")                 \
  XX(CB_header_field, "the on_header_field callback failed")         \
  XX(CB_header_value, "the on_header_value callback failed")         \
  XX(CB_headers_complete, "the on_headers_complete callback failed") \
  XX(CB_body, "the on_body callback failed")                         \
  XX(CB_message_complete, "the on_message_complete callback failed") \
  XX(CB_reason, "the on_reason callback failed")                     \
  XX(CB_chunk_header, "the on_chunk_header callback failed")         \
  XX(CB_chunk_complete, "the on_chunk_complete callback failed")     \
                                                                     \
  /* Parsing-related errors */                                       \
  XX(INVALID_EOF_STATE, "stream ended at an unexpected time")        \
  XX(HEADER_OVERFLOW,                                                \
     "too many header bytes seen; overflow detected")                \
  XX(CLOSED_CONNECTION,                                              \
     "data received after completed connection: close message")      \
  XX(INVALID_VERSION, "invalid HTTP version")                        \
  XX(INVALID_STATUS, "invalid HTTP status code")                     \
  XX(INVALID_METHOD, "invalid HTTP method")                          \
  XX(INVALID_URL, "invalid URL")                                     \
  XX(INVALID_HOST, "invalid host")                                   \
  XX(INVALID_PORT, "invalid port")                                   \
  XX(INVALID_PATH, "invalid path")                                   \
  XX(INVALID_QUERY_STRING, "invalid query string")                   \
  XX(INVALID_FRAGMENT, "invalid fragment")                           \
  XX(LF_EXPECTED, "LF character expected")                           \
  XX(INVALID_HEADER_TOKEN, "invalid character in header")            \
  XX(INVALID_CONTENT_LENGTH,                                         \
     "invalid character in content-length header")                   \
  XX(HUGE_CONTENT_LENGTH,                                            \
     "content-length header too large")                              \
  XX(INVALID_CHUNK_SIZE,                                             \
     "invalid character in chunk size header")                       \
  XX(HUGE_CHUNK_SIZE,                                                \
     "chunk header size too large")                                  \
  XX(INVALID_CONSTANT, "invalid constant string")                    \
  XX(INVALID_INTERNAL_STATE, "encountered unexpected internal state")\
  XX(STRICT, "strict mode assertion failed")                         \
  XX(PAUSED, "parser is paused")                                     \
  XX(UNKNOWN, "an unknown error occurred")


/* Define HPE_* values for each errno value above */
#define HTTP_ERRNO_GEN(n, s) HPE_##n,
enum http_errno_enum {
  HTTP_ERRNO_MAP(HTTP_ERRNO_GEN)
};
#undef HTTP_ERRNO_GEN


enum http_parser_url_fields
  { UF_SCHEMA           = 0
  , UF_HOST             = 1
  , UF_PORT             = 2
  , UF_PATH             = 3
  , UF_QUERY            = 4
  , UF_FRAGMENT         = 5
  , UF_USERINFO         = 6
  , UF_MAX              = 7
};


/* Result structure for http_parser_parse_url().
 *
 * Callers should index into field_data[] with UF_* values iff field_set
 * has the relevant (1 << UF_*) bit set. As a courtesy to clients (and
 * because we probably have padding left over), we convert any port to
 * a uint16_t.
 */
struct http_parser_url {
  uint16_t field_set;           /* Bitmask of (1 << UF_*) values */
  uint16_t port;                /* Converted UF_PORT string */

  struct {
    uint16_t off;               /* Offset into buffer in which field starts */
    uint16_t len;               /* Length of run in buffer */
  } field_data[UF_MAX];
};

/* Parse a URL; return nonzero on failure */
int http_parser_parse_url(const char *buf, size_t buflen, int is_connect, struct http_parser_url *u);

class http_parser
{

public:
	/* Request Methods */
	enum http_method
	{ HTTP_DELETE    = 0
	, HTTP_GET
	, HTTP_HEAD
	, HTTP_POST
	, HTTP_PUT
	/* pathological */
	, HTTP_CONNECT
	, HTTP_OPTIONS
	, HTTP_TRACE
	/* webdav */
	, HTTP_COPY
	, HTTP_LOCK
	, HTTP_MKCOL
	, HTTP_MOVE
	, HTTP_PROPFIND
	, HTTP_PROPPATCH
	, HTTP_UNLOCK
	/* subversion */
	, HTTP_REPORT
	, HTTP_MKACTIVITY
	, HTTP_CHECKOUT
	, HTTP_MERGE
	/* upnp */
	, HTTP_MSEARCH
	, HTTP_NOTIFY
	, HTTP_SUBSCRIBE
	, HTTP_UNSUBSCRIBE
	/* RFC-5789 */
	, HTTP_PATCH
	};


	enum http_parser_type { HTTP_REQUEST, HTTP_RESPONSE, HTTP_BOTH };


	/* Flag values for http_parser.flags field */
	enum flags
	{ F_CHUNKED               = 1 << 0
	, F_TRAILING              = 1 << 3
	, F_UPGRADE               = 1 << 4
	, F_SKIPBODY              = 1 << 5
	};

	struct http_errno
	{
		http_errno(int e) : m_errno((http_errno_enum)e){}
		http_errno(http_errno_enum e) : m_errno((http_errno_enum)e){}

		/* Return a string name of the given error */
		const char* name();
		/* Return a string description of the given error */
		const char* description();

	private:
		http_errno_enum m_errno;
	};


	/*
	 * Callbacks should return non-zero to indicate an error. The parser will
	 *
	 * then halt execution.
	 *
	 * The one exception is on_headers_complete. In a HTTP_RESPONSE parser
	 * returning '1' from on_headers_complete will tell the parser that it
	 * should not expect a body. This is used when receiving a response to a
	 * HEAD request which may contain 'Content-Length' or 'Transfer-Encoding:
	 * chunked' headers that indicate the presence of a body.
	 *
	 * http_data_cb does not return data chunks. It will be call arbitrarally
	 * many times for each string. E.G. you might get 10 callbacks for "on_path"
	 * each providing just a few characters more data.
	 */
	typedef std::function<int(http_parser&, const char *at, size_t length)> http_data_cb;

	typedef std::function<int(http_parser&)> http_cb;

	struct parser_settings {
		http_cb      on_message_begin;
		http_data_cb on_url;
		http_data_cb on_header_field;
		http_data_cb on_header_value;
		http_data_cb on_headers_complete;
		http_data_cb on_body;
		http_cb      on_message_complete;
		http_data_cb on_reason;
		/* When on_chunk_header is called, the current chunk length is stored
		* in parser->content_length.
		*/
		http_cb      on_chunk_header;
		http_cb      on_chunk_complete;
	};


public:

	http_parser(enum http_parser_type t);

public:

	std::size_t execute(const parser_settings& _settings, const char *data, size_t len);

	/* Pause or un-pause the parser; a nonzero value pauses */
	void pause(int paused);

public:

	/* Returns a string version of the HTTP method. */
	static const char * method_str (enum http_method m);

private:

	unsigned char type : 2;     /* enum http_parser_type */
	unsigned char flags : 6;    /* F_* values from 'flags' enum; semi-public */
	unsigned char state;        /* enum state from http_parser.c */
	unsigned char header_state; /* enum header_state from http_parser.c */
	unsigned char index;        /* index into current matcher */

	uint32_t nread;          /* # bytes read in various scenarios */
	int64_t m_content_length;  /* # bytes in body (0 if no Content-Length header) */

	unsigned short m_http_major;
	unsigned short m_http_minor;
	unsigned short m_status_code; /* responses only */
	unsigned char m_method;       /* requests only */
	unsigned char m_http_errno : 7;

	/* 1 = Upgrade header was present and the parser has exited because of that.
	* 0 = No upgrade header present.
	* Should be checked when http_parser_execute() returns in addition to
	* error checking.
	*/
	char m_upgrade : 1;

public:
	/* Get an http_errno value from an http_parser */
 	inline http_errno get_errno(){return http_errno(m_http_errno);}

 	inline bool has_upgrade(){return m_upgrade;}

 	inline unsigned short http_major(){return m_http_major;}
 	inline unsigned short http_minor(){return m_http_minor;}

 	inline unsigned short set_status_code(unsigned short _status_code){m_status_code = _status_code;}

 	inline unsigned char request_method(){return m_method;}

 	inline int64_t content_length(){return m_content_length;}
};

