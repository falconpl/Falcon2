/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: string.h

  Specialized Falcon String
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 30 Mar 2019 10:10:26 +0000
  Touch : Sat, 30 Mar 2019 10:10:26 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_STRING_H_
#define _FALCON_STRING_H_

#include <string>
namespace Falcon {

/**
 * Specialized String for Falcon.
 *
 * Currently, it's a std::string. It is meant to implement transparently some
 * special features of Falcon strings, while providing full compatibility with
 * std::string.
 *
 * It automatically cast-converts from and to std::string, and provides a
 * str() method to explicitly access the underlying string.
 */
class String: public std::string {
	String()=default;
	String(const String&) = default;
	String(String&&) = default;
	~String() = default;

	String(const char* data):
		std::string(data)
	{}
	String(const std::string& data):
		std::string(data)
	{}

	const std::string str() const noexcept { return static_cast<std::string>(*this); }
	std::string str() noexcept { return static_cast<std::string>(*this); }
	const operator std::string() const noexcept { return str(); }
	operator std::string() noexcept { return str(); }

	String& operator =(const String& other) {str() = other.str(); return *this; }
	bool operator <(const String& other) const noexcept {return str() < other.str(); }

};

}

#endif /* _FALCON_STRING_H_ */

/* end of string.h */

