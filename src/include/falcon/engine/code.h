/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: code.h

  Object representing Falcon code-tree
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sun, 17 Feb 2019 14:04:12 +0000
  Touch : Sun, 17 Feb 2019 14:17:30 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_CODE_H_
#define _FALCON_CODE_H_

#include <string>

namespace Falcon {
class Code {
public:
	Code(const std::string& source="", int m_line=0) noexcept;
	Code(const Code& other) noexcept;
	Code(Code&& other) noexcept;
	Code& operator=(const Code& other) = default;
	~Code();

	std::string toString() const noexcept;
private:
	std::string m_source;
	int m_line;
	char *m_data; // TODO
};
}

#endif /* _FALCON_CODE_H_ */

/* end of code.h */

