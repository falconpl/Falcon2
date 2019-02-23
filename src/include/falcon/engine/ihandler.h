/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: ihandler.h

  Base item handler
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 23 Feb 2019 04:27:44 +0000
  Touch : Sat, 23 Feb 2019 07:17:16 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#ifndef _FALCON_IHANDLER_H_
#define _FALCON_IHANDLER_H_

#include <string>
#include <unordered_map>

#include <falcon/setup.h>
#include <falcon/types.h>

namespace Falcon {
class Item;
class VMContext;

/**
 * Item Handler
 *
 * This is the base class for all the item handlers, that is, the classes
 * of items seen as native by the engine.
 */
class FALCON_API_ IHandler
{
public:
	using e_type = enum {
		type_undefined,
		type_nil,
		type_bool,
		type_int,
		type_double,
		type_numeric,
		type_string,
		type_symbol,
		type_buffer,
		type_code,
		type_meta,
		type_array,
		type_dict,
		type_concept,
		type_user
	};

	const std::string& name() const {return m_name;}
	const e_type type() const {return m_type;}

	virtual void create(Item& target, void* params=nullptr) const=0;
	virtual void destroy(Item& target) const
	{
		// By default, do nothing.
	}

	virtual bool toBool(const Item& target) const
	{
		return false;
	}

	virtual int64 toInteger(const Item& target) const
	{
		return 0L;
	}

	virtual double toDouble(const Item& target) const
	{
		return 0.0;
	}

	virtual std::string toString(const Item& target) const
	{
		return "Undefined";
	}

	using MessageHandler = void *(const std::string& msg, VMContext& ctx, int nparams);
	using MessageMap = std::unordered_map<std::string, MessageHandler>;

	void addMessage(const std::string& msg, MessageHandler mh);
	void delMessage(const std::string& msg);
	void sendMessage(const std::string& msg, VMContext& ctx, int nparams);
	const MessageMap& messages() const {return m_messageMap;}

	virtual bool getProperty(Item& self, const std::string& property, Item& target) const {
		return false;
	}

	virtual bool setProperty(Item& self, const std::string& property, Item& source) const {
		return false;
	}


protected:
	IHandler(const std::string& name="Undefined", e_type type=type_undefined):
		m_name(name),
		m_type(type)
	{}
	virtual ~IHandler() = default;

private:
	std::string m_name;
	e_type m_type;
	using MessageMap = std::unordered_map<std::string, MessageHandler>;
	MessageMap m_messageMap;
};

}

#endif /* _FALCON_IHANDLER_H_ */

/* end of ihandler.h */
