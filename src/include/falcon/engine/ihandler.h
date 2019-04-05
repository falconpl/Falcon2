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
#include <variant>
#include <vector>

#include <falcon/setup.h>
#include <falcon/string.h>
#include <falcon/types.h>
#include <falcon/item.h>

namespace Falcon {
class VMContext;

class TypeInfo
{
	virtual ~TypeInfo() {}
	using Ptr = std::shared_ptr<TypeInfo>;

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
	virtual bool toBool(const Item& target) const noexcept =0;
	virtual int64 toInteger(const Item& target) const noexcept =0;
	virtual double toDouble(const Item& target) const noexcept =0;
	virtual String toString(const Item& target) const noexcept =0;

private:
	String m_name;
	e_type m_type;
};

/**
 * Item Handler
 *
 * This is the base class for all the item handlers, that is, the classes
 * of items seen as native by the engine.
 *
 * They are comprising type information, static and unalterable,
 * and a message table that is never changed after creation.
 *
 */
class FALCON_API_ IHandler
{
public:

	/** The basic type information of this handler.
	 *
	 * Type information, or type behaviour, are the same across a wide
	 * range of item handlers. For example, two items having the same
	 * underlying string type could have different delegates,
	 * accessors or garbage collection profiles.
	 *
	 * Hence, the type information associated with different handler might
	 * be the same.
	 *
	 * Type Information are registered with the engine, and they are valid as long
	 * as the engine exits. Once created, they cannot be destroyed or modified.
	 * Hence, it's safe to access them by direct pointer.
	 *
	 */
	TypeInfo* typeInfo() const noexcept { return m_typeInfo;}

	/** Simplified accessor.
	 * Use this if you can access your property or call your method directly, and
	 * return a value to the virtual machine after your C++ call.
	 *
	 */
	using GeterCB = bool (*)(const String& message, int id, const Item& self, Item& value);
	using SetterCB = bool (*)(const String& message, int id, const Item& self, const Item& value);
	using SenderCB = bool (*)(const String& message, int id, const Item& self, Item& retval,
			const Item::sequence& paramters);

	/** Full message handler callback.
	 *
	 * A full message handler allows for direct manipulation of the VM context status.
	 * Use this if you need call other functions from within your handler.
	 */
	using DeepAccessor = bool (*)(const String& message, int id, VMContext& ctx);

	/** Stateful simple accessor.
	 *
	 * Use this when you can simply access your property or call your method, but you need
	 * to keep extra data of any nature, including varying states.
	 */
	class SetterFunctor {
	public:
		using Ptr = std::shared_ptr<SetterFunctor>;
		virtual ~SetterFunctor() {}
		virtual bool operator()(const String& message, int id, const Item& value) = 0;
	};

	class GetterFunctor {
	public:
		using Ptr = std::shared_ptr<GetterFunctor>;
		virtual ~GetterFunctor() {}
		virtual bool operator()(const String& message, int id, const Item& value) = 0;
	};

	/** Stateful deep accessor.
	 *
	 * A full message handler allows for direct manipulation of the VM context status,
	 * with the ability to keep extra status.
	 *
	 * Use this if you need call other functions from within your handler, and possibly
	 * use other data or status.
	 */
	class DeepFunctor {
	public:
		using Ptr = std::shared_ptr<DeepFunctor>;
		virtual ~DeepFunctor() {}
		virtual bool operator()(const String& message, int id, VMContext& ctx) = 0;
	};

	using Setter = std::variant<std::monostate, SetterCB, SetterFunctor::Ptr, DeepAccessor, DeepFunctor::Ptr>;
	using Getter = std::variant<std::monostate, GetterCB, GetterFunctor::Ptr, DeepAccessor, DeepFunctor::Ptr>;
	using Sender = std::variant<std::monostate, SenderCB, SenderFunctor::Ptr, DeepAccessor, DeepFunctor::Ptr>;

	/** A message handler is a triplet of receivers.
	 *
	 * The message handler holds the behavior for setting a property,
	 * getting a property and receiving a message directed to that property.
	 *
	 *
	 */
	class MsgHandler {
	public:
		MsgHandler() = default;
		MsgHandler(const MsgHandler& ) = default;
		MsgHandler(MsgHandler&& ) = default;
		~MsgHandler() = default;

		MsgHandler(Getter getter, Setter setter, Sender sender):
			m_setter{setter}, m_getter{getter}, m_sender{sender}
			{}

		using Ptr = std::shared_ptr<MsgHandler>;

		const Setter& setter() const noexcept { return m_setter; }
		const Getter& getter() const noexcept { return m_getter; }
		const Sender& sender() const noexcept { return m_sender; }

	private:
		Setter m_setter;
		Getter m_getter;
		Sender m_sender;
	};


	void addMessage(const String& msg, MsgHandler::Ptr mh) noexcept;
	void addMessage(const String& msg, Receiver getp,  Receiver setp,  Receiver send) noexcept
	{
		addMessage(msg, std::make_shared<MsgHandler>(getp, setp, send));
	}

	bool delegate(const String& msg, Item& delegate) noexcept;
	bool clearDelegate(const String& msg) noexcept;
	bool delMessage(const String& msg) noexcept;

	bool hasMessage(const String& msg) const noexcept
	{
		MessageMap::const_iterator pos{m_messageMap.find(msg)};
		return pos != m_messageMap.end();
	}

	bool hasSender(const String& msg) const noexcept {
		MessageMap::const_iterator pos{m_messageMap.find(msg)};
		return pos != m_messageMap.end()
				&& ! std::holds_alternative<std::monostate>(pos->second.msgh()->sender());
	}

	bool hasGetter(const String& msg) const noexcept {
		MessageMap::const_iterator pos{m_messageMap.find(msg)};
		return pos != m_messageMap.end()
				&& ! std::holds_alternative<std::monostate>(pos->second.msgh()->getter());
	}

	bool hasSetter(const String& msg) const noexcept {
		MessageMap::const_iterator pos{m_messageMap.find(msg)};
		return pos != m_messageMap.end()
				&& ! std::holds_alternative<std::monostate>(pos->second.msgh()->setter());
	}

	bool hasDelegate(const String& msg) const noexcept {
		MessageMap::const_iterator pos{m_messageMap.find(msg)};
		return pos != m_messageMap.end() && ! pos->second.delegate().isNil();
	}

	//const MessageMap& messages() const noexcept {return m_messageMap;}

	void setDefaultHandler(MsgHandler::Ptr mh) noexcept {
		m_defaultHandler = mh;
	}

	void setDefaultHandler(Receiver getp, Receiver setp, Receiver send) noexcept {
		m_defaultHandler = std::make_shared<MsgHandler>(getp, setp, send);
	}

	MsgHandler* getDefaultHandler() const noexcept {
		return m_defaultHandler.get();
	}

	void clearDefaultHandler() noexcept {
		m_defaultHandler.reset();
	}

	// will throw on circular reference
	void setDefaultDelegate(const Item& delegate) {
		checkCircularDelegate(*this, delegate.concept());
		m_defaultDelegate = delegate;
	}

	const Item& getDefaultDelegate() const noexcept {
		return m_defaultDelegate;
	}

	void clearDefaultDelegate() noexcept {
		m_defaultDelegate.setNil();
	}

	// will throw on circular reference
	void addBase(const Item& base)  {
		checkCircularDelegate(*this, base.concept());
		m_bases.push_back(base);
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
	IHandler(TypeInfo *type):
		m_typeInfo(type)
	{}
	virtual ~IHandler() = default;

private:

	class MsgMapEntry {
	public:
		// We want the message callback pointer to be valid,
		// even if we don't have callbacks
		MsgMapEntry(): m_msgh(std::make_shared<MsgHandler>()) {}

		MsgMapEntry(const MsgMapEntry&) = default;
		MsgMapEntry(MsgMapEntry&&) = default;
		~MsgMapEntry() = default;

		MsgMapEntry(MsgHandler::Ptr msgh): m_msgh(msgh) {}
		MsgMapEntry(MsgHandler::Ptr msgh, const Item& delegate): m_msgh(msgh), m_delegate(delegate) {}
		MsgMapEntry(Receiver getp, Receiver setp, Receiver call):
			m_msgh(std::make_shared<MsgHandler>(getp, setp, call))
		{}
		MsgMapEntry(Receiver getp, Receiver setp, Receiver call, const Item& delegate):
					m_msgh(std::make_shared<MsgHandler>(getp, setp, call)),
					m_delegate(delegate)
		{}

		const MsgHandler::Ptr& msgh() const noexcept { return m_msgh;}
		const Item& delegate() const noexcept {return m_delegate;}
		void delegate(Item delegate) const noexcept {m_delegate = delegate;}
		void clearDelegate() noexcept {m_delegate.setNil();}

		int id() const noexcept {return m_id;}
	private:
		int m_id{0};
		MsgHandler::Ptr	m_msgh;
		Item m_delegate;

	};

	int m_msgId{0};
	TypeInfo* m_typeInfo;

	using MessageMap = std::unordered_map<String, MsgMapEntry>;
	MessageMap m_messageMap;

	MsgHandler::Ptr m_defaultHandler;
	Item m_defaultDelegate;

	std::vector<Item> m_bases;
	static bool checkCircularDelegate(const IHandler& origin, const IHandler& target);
	void accessor(const String& msg, Item& self, VMContext& ctx, bool isSetter) const noexcept;
};

}

#endif /* _FALCON_IHANDLER_H_ */

/* end of ihandler.h */
