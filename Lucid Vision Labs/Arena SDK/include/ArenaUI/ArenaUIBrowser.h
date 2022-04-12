#pragma once
#include <string>

namespace ArenaUI
{
	class ARENAUI_API BrowserHandler
	{
	public:
		BrowserHandler(ARENAUI_HWND hWnd, std::string url);
		virtual ~BrowserHandler();

		ARENAUI_HWND GetBrowserHwnd();

		void Reload();

		void Back();

		void Forward();

		void LoadUrl(std::string url);

		std::string GetUrl();

		//Close the browser, this will return true when its ok to close the window that hosts this browser, otherwise
		// it will return false and the WM_CLOSE message should be ignored.  This if returs false it will resend the
		// WM_CLOSE message therefore allowing user to call this fucntion again until it returns true and its ok to
		//follow through with the closing of the host window.
		bool CloseBrowser();

	private:
		void* m_pImpl;
	};
} // namespace ArenaUI
