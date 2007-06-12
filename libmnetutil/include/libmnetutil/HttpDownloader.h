/*
  Copyright (C) 2005, 2004 Erik Eliasson, Johan Bilien

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _HTTPDOWNLOAD_H_
#define _HTTPDOWNLOAD_H_

#include <libmutil/MemObject.h>
#include <libmnetutil/StreamSocket.h>
#include <libmnetutil/TCPSocket.h>
#include <libmnetutil/Downloader.h>
#include <string>
#include <vector>
#include <map>

#define HTTP_METHOD_1_0 			"HTTP/1.0"

#define HTTP_RESPONSECODE_OK 			200
#define HTTP_RESPONSECODE_NOTFOUND 		404
#define HTTP_RESPONSECODE_MOVEDPERMANENTLY 	301
#define HTTP_RESPONSECODE_MOVEDTEMPORARILY 	302
#define HTTP_RESPONSECODE_SERVERERROR 		500

#define HTTP_HEADER_FROM 			"From"
#define HTTP_HEADER_USERAGENT 			"User-Agent"
#define HTTP_HEADER_SERVER 			"Server"
#define HTTP_HEADER_HOST 			"Host"
#define HTTP_HEADER_ACCEPTS 			"Accepts"
#define HTTP_HEADER_CONTENTTYPE 		"Content-Type"
#define HTTP_HEADER_CONTENTLENGTH 		"Content-Length"
#define HTTP_HEADER_LASTMODIFIED 		"Last-Modified"

#define HTTP_HEADER_CRLF			"\r\n"

using namespace std;

/**
 * This class is a very simple HTTP user agent that fetches web pages using the HTTP 1.0 (not 1.1) protocol.
 *
 * The implementation is, as mentioned, very simplified and has the following limitations:
 *  - The entire requested document is stored in memory, thus exhausting the available system memory if large files are fetched
 *  - The above limitation is however not relevant as only the first 32 KiB of the requested document are fetched
 *  - Cannot cope with multi-line headers
 *  - Cannot handle password protected resources
 *  - Only handles HTTP (not HTTPS)
 *
 * Usage:
 *  # Initialize the class instance by calling the constructor
 *  # Call either downloadToFile(string), downloadToString() or requestHeaders()
 *  # Work with the document using the functions getHeader(string), getBody() and getResponseCode()
 *
 * Constructors and descructors are defined in HEADER FILE!
 *
 * @author	Mikael Svensson
 */
class HttpDownloader : public Downloader {
	public:
		/**
		 * Connects to HTTP server but does not fetch file.
		 *
		 * @param	url	File/document to fetch.
		 */
		HttpDownloader(string url) : url (url), remotePort(80), respCode(-1), followRedirect(true), sock(NULL), internalSocketObject(false) {
			parseUrl();
			if (remotePort > 0 && remoteHostname != "") {
				sock = new TCPSocket(remoteHostname, remotePort);
				internalSocketObject = true;
			}
		}

		/**
		 * This constructor is useful when tunneling HTTP over other protocols than pure TCP.
		 *
		 * Since the constructor HttpDownload(string) opens up an new TCP connection directly it is not
		 * useful when encryption is needed (e.g. when using the TlsSrpSocket). In these cases
		 * a StreamSocket object can be created outside the HttpDownloader object and passed to this
		 * constructor for use by the download functions later on.
		 *
		 * @note	The StreamSocket sock will *not* be closed after usage, even though this is not an HTTP 1.1 compatible client.
		 * @param	url	File/document to fetch.
		 * @param	sock	Pre-existing socket to use for communicating with HTTP server.
		 */
		HttpDownloader(string url, StreamSocket * sock): url (url), remotePort(80), respCode(-1), followRedirect(true), sock(sock), internalSocketObject(false) {
			parseUrl();
		}

		/**
		 * The default constructor deallocates memory, if allocated.
		 */
		~HttpDownloader() {
			if (internalSocketObject)
				delete sock;
		}


		const char*	getChars();
		/**
		 * Fetch remote file and save as file on local computer.
		 */
		bool 	downloadToFile(string filename);

		/**
		 * Fetch remote file and return it as a single string
		 */
		string 	downloadToString();

		/**
		 * Fetch only HTTP headers for remote file.
		 *
		 * @return	True on success, false otherwise.
		 */
		bool 	downloadHeaders();

		/**
		 * Return value of HTTP header.
		 *
		 * @note	This function relies on that either downloadHeaders(), downloadToFile() or downloadToString() has been called prior.
		 * @param	header	Name of header to fetch.
		 */
		string 	getHeader(string header);

		/**
		 * Returns the response code.
		 *
		 * @note	This function relies on that either downloadHeaders(), downloadToFile() or downloadToString() has been called prior.
		 */
		int 	getResponseCode() const;

		/**
		 * Sets whether or not redirection responses should be followed automatically.
		 *
		 * To prevent circular redirections (loops) a maximum of three redirections will be followed.
		 */
		void 	setFollowRedirects(bool const val);
		bool 	getFollowRedirects() const;

		virtual std::string getMemObjectType() const {return "HttpDownloader";};

	private:
		// Variables
		string 	url;
		string 	remoteHostname;
		string 	remoteProtocol;
		string 	remoteFile;
		int 	remotePort;
		map<string, string> headers;
		int 	respCode;
		bool	followRedirect;
		StreamSocket * sock;
		bool 	internalSocketObject;

		// Functions
		int 	fetch(string request, ostream & bodyStream);

		/**
		 * Parse a line of the HTTP response and store the line as a key/value pair in the map of headers.
		 *
		 * Note that the header value is trimmed for white-space both from the right and left.
		 */
		void 	parseHeader(string line);

		/**
		 * Does not take into account the following things:
		 * - Header values might be split into multiple lines
		 * - Header lines can end with \\n instead of \\r\\n
		 */
		int 	parseHeaders(stringstream & headers);

		/**
		 * Used to parse the URL supplied in the constructor.
		 *
		 * The purpose is to pick out address, port and so on.
		 *
		 * Can probably not handle "Location: /index.php" but only "Location: http://full-server-name/index.php"
		 */
		void 	parseUrl();

		/**
		 * @deprecated	Use the very similar split() from libmutil/stringutils.h instead.
		 * @param	data		String to be split
		 * @param	token		Separator
		 * @param	res		The string vector where the strings found will be stored
		 * @param	maxChars	The maximum number of characters of data that will be scanned (-1 disables feature and entire string is scanned).
		 * @author	Erik Ehrlund
		 * @author	Mikael Svensson
		 */
		void	split(string data, string token, vector<string> &res, int maxChars = -1); // Copyright Erik Ehrlund

		/**
		 * This function removes any spaces, line feeds, carrige returns and tabs from the beginning and end of the specified string.
		 *
		 * @deprecated	Use the very similar trim() from libmutil/stringutils.h instead.
		 */
		string	trim(string s);
		string	buildRequestString(string method, string file);
};

#endif
