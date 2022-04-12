
#pragma once

namespace ArenaUI
{
	enum MessageLoopType
	{
		Automatic, //Users do not need to manage the message loop at all, handled on internal ArenaUI thread
		Managed,   //Users need to call RunMessageLoop() from their own main UI thread
		Manual	 //Users need to call DoMessageLoopWork() from OnIdle() in their own message handling loop
	};
	//Set to true before calling Windows APIs like TrackPopupMenu that enter a modal message loop. Set to false after exiting the modal message loop.
	void ARENAUI_API ArenaUISetOSModalLoop(bool osModalLoop);

	void ARENAUI_API RegisterQuery(JSQuery* pQuery);

	void ARENAUI_API RegisterJSFunctionHandler(JSFuncRegistration* pFuncReg);

	void ARENAUI_API UnregisterJSFunctionHandlers();

	void ARENAUI_API Shutdown();

	// This class is exported from the ArenaUI.dll
	class ARENAUI_API ArenaUI
	{
	public:
		ArenaUI();
		~ArenaUI();

		int Initialize(HINSTANCE hInstance, MessageLoopType type = Automatic);

		void RunMessageLoop();
		void DoMessageLoopWork();

	private:
		void* m_pImpl;
	};
} // namespace ArenaUI
