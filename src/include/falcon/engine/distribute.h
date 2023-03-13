/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: distribute.h

  Distribute an iterator over a sequence
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 06 Apr 2019 12:12:27 +0100
  Touch : Sat, 06 Apr 2019 12:12:27 +0100

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_DISTRIBUTE_H_
#define _FALCON_DISTRIBUTE_H_

namespace falcon {

template<typename _iterator, typename _tgt>
void distribute(_iterator&& iter, _tgt& tgt)
{
	tgt = *iter;
}

/**
 * Utility to distribute the values of an iterator on target arguments.
 *
 * Example:
 * @code
 * std::vector<std::variant<int, std::string>> array{1, "hello", 2, "world"};
 * std::string sh, sw;
 * int one, two;
 *
 * falcon::distribute(array.begin(), one, sh, two, sw);
 *
 * std::cout << sh << ": " << one << '\n'
 * 	         << sw << ": " << two << '\n';
 * @endcode
 */
template<typename _iterator, typename _tgt, typename ...Args>
void distribute(_iterator&& iter, _tgt& tgt, Args&... args)
{
	tgt = *iter;
	++iter;
	distribute(std::forward<_iterator>(iter), args...);
}

}

#endif /* _FALCON_DISTRIBUTE_H_ */

/* end of distribute.h */

