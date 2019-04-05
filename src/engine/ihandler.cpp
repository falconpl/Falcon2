/*****************************************************************************
  FALCON2 - The Falcon Programming Language
  FILE: ihandler.cpp

  Base Item Handler
  -------------------------------------------------------------------
  Author: Giancarlo Niccolai
  Begin : Sat, 16 Mar 2019 15:09:25 +0000
  Touch : Sat, 16 Mar 2019 15:09:25 +0000

  -------------------------------------------------------------------
  (C) Copyright 2019 The Falcon Programming Language
  Released under Apache 2.0 License.
******************************************************************************/

#include <falcon/engine/ihandler.h>
#include <falcon/engine/vmcontext.h>
#include <falcon/logger.h>

#define LOCAL_CAT	LOG_CAT("M+ENG")

namespace Falcon {

void IHandler::addMessage(const String& msg, MsgHandler::Ptr mh) noexcept
{
	LOG_TRC << LOCAL_CAT << this << "->" << "addMessage(" << msg << ", " << mh.get() << ")";

	m_messageMap.insert(std::make_pair(msg, MsgHandler(m_msgId++, mh)));
}

void IHandler::addMessage(const String& msg, MsgHandler::Ptr mh, Delegate::Ptr delegate) noexcept
{
	LOG_TRC << LOCAL_CAT << this << "->" << "addMessage(" << msg << ", " << mh.get() << ',' << delegate.get() << ")";
	m_messageMap.insert(std::make_pair(msg, MsgMapEntry(m_msgId++, mh, delegate)));
}

bool IHandler::delMessage(const String& msg) noexcept
{
	LOG_TRC << LOCAL_CAT << this << "->" << "delMessage(" << msg << ")";
	return m_messageMap.erase(msg) != 0;
}

bool IHandler::delegate(const String& msg, Delegate::Ptr delegate) noexcept
{
	LOG_TRC << LOCAL_CAT << this << "->" << "delegate(" << msg << ", " << delegate.get() << ")";
	auto iterator = m_messageMap.find(msg);
	if(iterator == m_messageMap.end()) {
		return false;
	}
	iterator->second.delegate(delegate);
	return true;
}

bool IHandler::clearDelegate(const String& msg) noexcept
{
	LOG_TRC << LOCAL_CAT << this << "->" << "clearDelegate(" << msg << ")";
	auto iterator = m_messageMap.find(msg);
	if(iterator == m_messageMap.end()) {
		return false;
	}
	iterator->second.clearDelegate();
	return true;
}

void IHandler::setDefaultDelegate(Delegate::Ptr delegate) noexcept
{
	LOG_TRC << LOCAL_CAT << this << "->" << "setDefaultDelegate(" << delegate.get() << ")";
	m_defaultDelegate = delegate;
}


void IHandler::sendMessage(const String& msg, Item& self, VMContext& ctx) const noexcept
{
	LOG_TRC << LOCAL_CAT << this << "->" << "sendMessage(" << msg << ", " << &self << "," << &ctx << ")";

	auto iterator = m_messageMap.find(msg);
	bool result = iterator != m_messageMap.end();
	if(result) {
		auto entry = iterator->second;

		template<typename cbtype>
		auto callAndReturn = [&](cbtype cb, const String& msg, Item& self, VMContext& ctx) {
			Item retval;
			if(cb(msg, entry.id(), ctx.self(), retval)){
				ctx.returnFrame(retval);
				return true;
			}
			return false;
		};

		result = std::visit(sendVisitor {
				[](std::monostate) { return false; },
				[&](const DeepAccessor& cb) { return cb(msg, entry.id(), ctx.self(), ctx); },
				[&](const DeepFunctor::Ptr& cb) { return cb(msg, entry.id(), ctx.self(), ctx); },
				[&](const Accessor& cb) { return callAndReturn(cb, msg, entry.id(), ctx.self(), ctx);},
				[&](const Functor::Ptr& cb) {return callAndReturn(*cb, msg, entry.id(), ctx.self(), ctx);},

			},
			entry.msgh()->sender());
	}

	if (!result) {
		ctx->raiseInvalidMessage(msg);
	}
}

void IHandler::getProperty(const String& msg, Item& self, VMContext& ctx) const noexcept
{
	LOG_TRC << LOCAL_CAT << this << "->" << "getProperty(" << msg << ", " << &self << "," << &ctx << ")";

	auto iterator = m_messageMap.find(msg);
	bool result = iterator != m_messageMap.end();
	if(result) {
		auto entry = iterator->second;

		// helper for non-deep get accessors
		template<typename cbtype>
		auto getProp = [&](cbtype cb, const String& msg, Item& self, VMContext& ctx) {
			ctx->pushNil();
			if(cb(msg, entry.id(), ctx.self(), ctx->topData())){
				return true;
			}
			cxt->popData();
			return false;
		};

		// switch selecting what to do
		result = std::visit(sendVisitor {
				[](std::monostate) { return false; },
				[&](const DeepAccessor& cb) { return cb(msg, entry.id(), ctx.self(), ctx); },
				[&](const DeepFunctor::Ptr& cb) { return cb(msg, entry.id(), ctx.self(), ctx); },
				[&](const Accessor& cb) { return getProp(cb, msg, entry.id(), ctx.self(), ctx);},
				[&](const Functor::Ptr& cb) {return getProp(*cb, msg, entry.id(), ctx.self(), ctx);},

			},
			entry.msgh()->getter());
	}

	if (!result) {
		ctx->raiseInvalidMessage(msg);
	}
}

void IHandler::setProperty(const String& msg, Item& self, Item& value, VMContext& ctx) const noexcept
{
	LOG_TRC << LOCAL_CAT << this << "->" << "setProperty(" << msg << ", " << &self << "," << &value << "," << &ctx << ")";

	auto iterator = m_messageMap.find(msg);
	bool result = iterator != m_messageMap.end();
	if(result) {
		auto entry = iterator->second;

		// helper for non-deep set accessors
		template<typename cbtype>
		auto setProp = [&](cbtype cb, const String& msg, Item& self, VMContext& ctx) {
			ctx->pushNil();
			if(cb(msg, entry.id(), ctx.self(), value, ctx->topData())){
				return true;
			}
			ctx->popData();
			return false;
		};

		// switch over the property type.
		result = std::visit(sendVisitor {
				[](std::monostate) { return false; },
				[&](const DeepAccessor& cb) { return cb(msg, entry.id(), ctx.self(), value, ctx); },
				[&](const DeepFunctor::Ptr& cb) { return cb(msg, entry.id(), ctx.self(), value, ctx); },
				[&](const Accessor& cb) { return setProp(cb, msg, entry.id(), ctx.self(), value, ctx);},
				[&](const Functor::Ptr& cb) {return setProp(*cb, msg, entry.id(), ctx.self(), value, ctx);},

			},
			entry.msgh()->getter());
	}

	if (!result) {
		ctx->raiseInvalidMessage(msg);
	}
}

}

/* end of ihandler.cpp */
