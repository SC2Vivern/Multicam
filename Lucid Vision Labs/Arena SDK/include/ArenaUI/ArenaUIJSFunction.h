#pragma once
#include <string>
#include <vector>
namespace ArenaUI
{
	class JSFunction
	{
	public:
		///
		// Notify the associated JavaScript function callback
		///
		virtual void Execute(long long funcId, std::vector<std::string> arguments) = 0;

		virtual std::string GetFunctionName() = 0;
	};

	class JSFuncRegistration
	{
	public:
		JSFuncRegistration(){};
		///
		// Executed when a new query is received. |query_id| uniquely identifies the
		// query for the life span of the router. Return true to handle the query
		// or false to propagate the query to other registered handlers, if any. If
		// no handlers return true from this method then the query will be
		// automatically canceled with an error code of -1 delivered to the
		// JavaScript onFailure callback. If this method returns true then a
		// Callback method must be executed either in this method or asynchronously
		// to complete the query.
		///
		virtual bool OnRegister(
			long long funcId,
			std::vector<std::string> arguments,
			JSFunction* pCallbackFunction) = 0;
	};
}