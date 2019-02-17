/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: code.cpp

  Object representing Falcon code-tree
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sun, 17 Feb 2019 13:31:53 +0000
  Touch : Sun, 17 Feb 2019 14:17:30 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/engine/code.h>
#include <algorithm>

namespace Falcon {

Code::Code(const std::string& source, int line) noexcept :
		m_source(source), m_line(line), m_data(new char[1000])
{
	std::copy(m_source.data(), m_source.data() + m_source.size(), m_data);
	m_data[m_source.size()] = 0;
}

Code::Code(const Code& other) noexcept:
		m_source(other.m_source), m_line(other.m_line), m_data(other.m_data)
{}

Code::Code(Code&& other) noexcept:
		m_source(std::move(other.m_source)),
		m_line(other.m_line), m_data(other.m_data)
{
	other.m_data = nullptr;
}

Code::~Code()
{
	delete[] m_data;
}

std::string Code::toString() const noexcept {
	return std::string(m_data);
}

}


/* end of code.cpp */

