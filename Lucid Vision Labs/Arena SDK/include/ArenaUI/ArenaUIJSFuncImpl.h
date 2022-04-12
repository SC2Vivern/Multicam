#pragma once
#include "include/cef_v8.h"
#include "ArenaUIJSFunction.h"

namespace ArenaUI
{
	class JSFuncImpl : public JSFunction
	{
	public:
		JSFuncImpl(CefRefPtr<CefBrowser> browser, std::string funcName);
		virtual ~JSFuncImpl();

		virtual void Execute(long long funcId, std::vector<std::string> arguments);

		virtual std::string GetFunctionName();

	private:
		CefRefPtr<CefBrowser> m_browser;
		std::string m_funcName;
	};
}
