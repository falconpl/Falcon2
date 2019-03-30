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

#include <memory>
#include <unordered_map>

#include <falcon/setup.h>
#include <falcon/string.h>
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

	const String& name() const noexcept {return m_name;}
	const e_type type() const noexcept {return m_type;}

	virtual void create(Item& target, void* params=nullptr) const noexcept =0;
	virtual void destroy(Item& target) const noexcept
	{
		// By default, do nothing.
	}

	virtual bool toBool(const Item& target) const noexcept
	{
		return false;
	}

	virtual int64 toInteger(const Item& target) const noexcept
	{
		return 0L;
	}

	virtual double toDouble(const Item& target) const noexcept
	{
		return 0.0;
	}

	virtual String toString(const Item& target) const noexcept
	{
		return "Undefined";
	}

	using MsgHandlerCB = bool *(Item& self, const String& message, VMContext& ctx);

	struct MsgHandler {
	public:
		MsgHandlerCB setp;
		MsgHandlerCB getp;
		MsgHandlerCB send;
	};

	using MsgHandlerPtr = std::shared_ptr<MsgHandler>;

	struct Delegate {
	public:
		Item target;
		MsgHandlerPtr msgh;
	};
	using DelegatePtr = std::shared_ptr<Delegate>;

	struct MessageMapEntry {
		DelegatePtr delegate;
		MsgHandlerPtr msgh;

		MessageMapEntry() = default;
		MessageMapEntry(const MessageMapEntry&) = default;
		MessageMapEntry(MessageMapEntry&&) = default;


	};

	using MessageMap = std::unordered_map<String, MsgHandler>;

	void addMessage(const String& msg, MsgHandler mh) noexcept;
	bool delMessage(const String& msg) noexcept;
	bool hasMessage(const String& msg) noexcept;
	/**
	 * Send a message to the handler.
	 *
	 * The Handler will prepare the context so to instruct the VM to
	 * execute the necessary code.
	 *
	 * The handler might also perform the required operation immediately and
	 * leave the return value on the VM stack.
	 *
	 * @param msg The name of the message to be sent
	 * @param ctx The VMContext where the execution takes place
	 *
	 */
	bool sendMessage(const String& msg, VMContext& ctx) noexcept;
	const MessageMap& messages() const noexcept {return m_messageMap;}

	void setDefaultHandler(MsgHandler mh) noexcept {
		m_defaultHandler = mh;
	}

	MsgHandler getDefaultHandler() const noexcept {
		return m_defaultHandler;
	}

	void clearDefaultHandler() noexcept {
		m_defaultHandler = propertyDispatcher;
	}

protected:
	static bool propertyDispatcher(Item& target, const String& msg, VMContext& ctx);

	IHandler(const String& name="Undefined", e_type type=type_undefined):
		m_name(name),
		m_type(type)
	{}
	virtual ~IHandler() = default;

private:
	String m_name;
	e_type m_type;
	using MessageMap = std::unordered_map<String, MsgHandler>;
	MessageMap m_messageMap;

	MsgHandler m_defaultHandler{propertyDispatcher};
};

}

#endif /* _FALCON_IHANDLER_H_ */

/* end of ihandler.h */
