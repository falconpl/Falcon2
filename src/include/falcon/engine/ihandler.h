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



	using MsgHandlerCB = bool (*)(const String& message, Item& self, VMContext& ctx);

	class MsgHandler {
	public:
		MsgHandler() = default;
		MsgHandler(const MsgHandler& ) = default;
		MsgHandler(MsgHandler&& ) = default;
		~MsgHandler() = default;

		MsgHandler(MsgHandlerCB setp, MsgHandlerCB getp, MsgHandlerCB send):
			m_setp{setp}, m_getp{getp}, m_send{send}
			{}

		using Ptr = std::shared_ptr<MsgHandler>;

		const MsgHandlerCB& setp() const noexcept { return m_setp; }
		const MsgHandlerCB& getp() const noexcept { return m_setp; }
		const MsgHandlerCB& send() const noexcept { return m_setp; }

	private:
		MsgHandlerCB m_setp{nullptr};
		MsgHandlerCB m_getp{nullptr};
		MsgHandlerCB m_send{nullptr};
	};


	class Delegate {
	public:
		Delegate() = default;
		Delegate(const Delegate&) = default;
		Delegate(Delegate&&) = default;
		~Delegate() = default;


		Delegate(const Item& item, MsgHandler::Ptr msgh ):
			m_target(item), m_msgh(msgh)
		{}

		using Ptr = std::shared_ptr<Delegate>;

		const Item& target() const noexcept {return m_target;}
		const MsgHandler::Ptr& msgh() const noexcept {return m_msgh;}

	private:
		Item m_target;
		MsgHandler::Ptr m_msgh;
	};


	class MsgMapEntry {
	public:
		// We want the message callback pointer to be valid,
		// even if we don't have callbacks
		MsgMapEntry(): m_msgh(std::make_shared<MsgHandler>()) {}

		MsgMapEntry(const MsgMapEntry&) = default;
		MsgMapEntry(MsgMapEntry&&) = default;
		~MsgMapEntry() = default;

		MsgMapEntry(MsgHandler::Ptr msgh): m_msgh(msgh) {}
		MsgMapEntry(MsgHandler::Ptr msgh, Delegate::Ptr delegate): m_msgh(msgh), m_delegate(delegate) {}
		MsgMapEntry(MsgHandlerCB getp, MsgHandlerCB setp, MsgHandlerCB call):
			m_msgh(std::make_shared<MsgHandler>(getp, setp, call))
		{}
		MsgMapEntry(MsgHandlerCB getp, MsgHandlerCB setp, MsgHandlerCB call, Delegate::Ptr delegate):
					m_msgh(std::make_shared<MsgHandler>(getp, setp, call)),
					m_delegate(delegate)
		{}

		const MsgHandler::Ptr& msgh() const noexcept { return m_msgh;}
		const Delegate::Ptr& delegate() const noexcept { return m_msgh;}

	private:
		MsgHandler::Ptr	m_msgh;
		Delegate::Ptr m_delegate;

	};

	using MessageMap = std::unordered_map<String, MsgMapEntry>;

	void addMessage(const String& msg, MsgHandler::Ptr mh) noexcept;
	void addMessage(const String& msg, MsgHandlerCB getp,  MsgHandlerCB setp,  MsgHandlerCB send) noexcept
	{
		addMessage(msg, std::make_shared<MsgHandler>(getp, setp, send));
	}

	void addMessage(const String& msg, MsgHandler::Ptr mh, Delegate::Ptr delegate) noexcept;

	void addMessage(const String& msg, MsgHandlerCB getp,  MsgHandlerCB setp,  MsgHandlerCB send, Delegate::Ptr delegate) noexcept
	{
		addMessage(msg, std::make_shared<MsgHandler>(getp, setp, send), delegate);
	}

	bool delegate(const String& msg, Delegate::Ptr delegate) noexcept;
	bool clearDelegate(const String& msg) noexcept;
	bool delMessage(const String& msg) noexcept;

	bool hasMessage(const String& msg) const noexcept
	{
		MessageMap::const_iterator pos{m_messageMap.find(msg)};
		return pos != m_messageMap.end();
	}

	bool hasSend(const String& msg) const noexcept {
		MessageMap::const_iterator pos{m_messageMap.find(msg)};
		return pos != m_messageMap.end() && pos->second.msgh()->send() != nullptr;
	}

	bool hasGetp(const String& msg) const noexcept {
		MessageMap::const_iterator pos{m_messageMap.find(msg)};
		return pos != m_messageMap.end() && pos->second.msgh()->getp() != nullptr;
	}

	bool hasSetp(const String& msg) const noexcept {
		MessageMap::const_iterator pos{m_messageMap.find(msg)};
		return pos != m_messageMap.end() && pos->second.msgh()->setp() != nullptr;
	}

	bool hasDelegate(const String& msg) const noexcept {
		MessageMap::const_iterator pos{m_messageMap.find(msg)};
		return pos != m_messageMap.end() && pos->second.delegate().get() != nullptr;
	}

	//const MessageMap& messages() const noexcept {return m_messageMap;}

	void setDefaultHandler(MsgHandler::Ptr mh) noexcept {
		m_defaultHandler = mh;
	}

	void setDefaultHandler(MsgHandlerCB getp, MsgHandlerCB setp, MsgHandlerCB send) noexcept {
		m_defaultHandler = std::make_shared<MsgHandler>(getp, setp, send);
	}

	MsgHandler* getDefaultHandler() const noexcept {
		return m_defaultHandler.get();
	}

	void clearDefaultHandler() noexcept {
		m_defaultHandler.reset();
	}

	void setDefaultDelegate(Delegate::Ptr mh) noexcept;

	Delegate* getDefaultDelegate() const noexcept {
		return m_defaultDelegate.get();
	}

	void clearDefaultDelegate() noexcept {
		m_defaultDelegate.reset();
	}


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
	 * @param self The object receiving the message (might be different from VMcontext:self()).
	 * @param ctx The VMContext where the execution takes place
	 *
	 */
	void sendMessage(const String& msg, Item& self, VMContext& ctx) const noexcept;

	/**
	 * Gets a property from self a message to the handler.
	 *
	 * On success, the value of the property must be pushed on top of the data stack,
	 * or the context must be modified in order to access the property at a later time.
	 *
	 * On failure, the handle must raise the property exception in the context.
	 *
	 * @param msg The name of the message to be sent
	 * @param self The object receiving the message (might be different from VMcontext:self()).
	 * @param ctx The VMContext where the execution takes place
	 *
	 */
	void getProperty(const String& msg, Item& self, VMContext& ctx) const noexcept;

	/**
	 * Sets a property from self a message to the handler.
	 *
	 * On success, the value of the result of the operation (the value of the property)
	 * must be pushed on top of the data stack,
	 * or the context must be modified in order to access the property at a later time.
	 *
	 * On failure, the handle must raise the propert exception in the context.
	 *
	 * @param msg The name of the property (message) to be modified.
	 * @param self The object receiving the message (might be different from VMcontext:self()).
	 * @param value The value to be set as the property value.
	 * @param ctx The VMContext where the execution takes place
	 *
	 */
	void setProperty(const String& msg, Item& self, Item& value, VMContext& ctx) const noexcept;

protected:

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

	MsgHandler::Ptr m_defaultHandler;
	Delegate::Ptr m_defaultDelegate;

	bool checkCircularDelegate(const Delegate* delegate) const noexcept {
		return IHandler::checkCircularDelegate(delegate, this);
	}

	static bool checkCircularDelegate(const Delegate* delegate, const IHandler* origin);
};

}

#endif /* _FALCON_IHANDLER_H_ */

/* end of ihandler.h */
