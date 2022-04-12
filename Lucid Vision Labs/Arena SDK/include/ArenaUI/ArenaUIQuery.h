#pragma once
#include <string>

namespace ArenaUI
{

	class UikCallback
	{
	public:
		///
		// Notify the associated JavaScript onSuccess callback that the query has
		// completed successfully with the specified |response|.
		///
		virtual void Success(const std::string& response) = 0;

		///
		// Notify the associated JavaScript onFailure callback that the query has
		// failed with the specified |error_code| and |error_message|.
		///
		virtual void Failure(int error_code, const std::string& error_message) = 0;
	};

	class JSQuery
	{
	public:
		JSQuery(){};
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
		virtual bool OnQuery(
			long long query_id,
			const std::string& request,
			UikCallback& callback) = 0;

		///
		// Executed when a query has been canceled either explicitly using the
		// JavaScript cancel function or implicitly due to browser destruction,
		// navigation or renderer process termination. It will only be called for
		// the single handler that returned true from OnQuery for the same
		// |query_id|. No references to the associated Callback object should be
		// kept after this method is called, nor should any Callback methods be
		// executed.
		///
		virtual void OnQueryCanceled(
			long long query_id) = 0;
	};
}