/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: singleton.h

  Utility class to abstract a singleton
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 23 Feb 2019 04:55:25 +0000
  Touch : Sat, 23 Feb 2019 07:15:20 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_SINGLETON_H_
#define _FALCON_SINGLETON_H_

namespace Falcon {

/**
 * An utility class used for well known type handlers.
 *
 * As WTH are used as pointers, this class behaves as a smart pointer
 * to an instance of the given type.
 */
template <typename T>
class Singleton
{
public:
	T* get() {
		static T inst;
		return &inst;
	}

	const T* operator->() const {return const_cast<Singleton<T>*>(this)->get();}
	const T& operator*() const {return *(const_cast<Singleton<T>*>(this)->get());}

    /**
     * Mutable accessors.
     *
     * Singletons in the Falcon engine and modules are meant to be
     * read-only type handlers, opaque and unavailable after creation.
     *
     * To allow the inner part of the engine and the module initializers to
     * configure the newly created instance, we use this structure, so to
     * make the write operations explicitly localised.
     */
    struct Singleton_Mutable {
    	Singleton_Mutable(Singleton<T>* s): m_s(s) {}
        T* operator->() {return m_s->get();}
        T& operator*() {return *m_s->get();}
    private:
        Singleton<T>* m_s;
    } mtbl;


	Singleton(): mtbl(this){}

	~Singleton() = default;

	Singleton(Singleton const&) = delete;
	Singleton(Singleton&&) = delete;
	Singleton& operator=(Singleton const&) = delete;
	Singleton& operator=(Singleton &&) = delete;
};


}

#endif /* _FALCON_SINGLETON_H_ */

/* end of singleton.h */
