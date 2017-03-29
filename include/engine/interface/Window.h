#pragma once

class Engine;
class WindowManager;

/*
* Interface for getting information about the window.
*/
class Window
{
	friend class Engine;

	public:
		/*
		* Gets the height of the window
		*/
		static int getWindowHeight();

		/*
		* Gets the width of the window
		*/
		static int getWindowWidth();

	protected:
		/*
		* Initiates the interface with the window manager. 
		*/
		static void init(WindowManager *windowManager);

	private:
		static WindowManager *_windowManager;
};