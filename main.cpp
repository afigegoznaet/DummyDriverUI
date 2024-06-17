/*
  ==============================================================================

	This file contains the basic startup code for a JUCE application.

  ==============================================================================
*/

#include <JuceHeader.h>
#include <MainWindow.hpp>

//==============================================================================
class ExactonApp : public JUCEApplication {
public:
	//==============================================================================
	ExactonApp() {}

	const String getApplicationName() override {
		return ProjectInfo::projectName;
	}
	const String getApplicationVersion() override {
		return ProjectInfo::versionString;
	}
	bool moreThanOneInstanceAllowed() override { return true; }

	//==============================================================================
	void initialise(const String &) override {
		// This method is where you should put your application's initialisation
		// code..

		mainDlg.reset(new MainDialog(getApplicationName()));
		mainDlg->setResizable(false, false);
		mainDlg->setSize(620, 360);
		// mainDlg->setResizeLimits(Invariants::min_width,
		// Invariants::min_height, 						 Invariants::max_width,
		// Invariants::max_height);
	}

	void shutdown() override {
		// Add your application's shutdown code here..

		mainDlg = nullptr; // (deletes our window)
	}

	//==============================================================================
	void systemRequestedQuit() override {
		// This is called when the app is being asked to quit: you can ignore
		// this request and let the app carry on running, or call quit() to
		// allow the app to close.
		quit();
	}

	void anotherInstanceStarted(const String &) override {
		// When another instance of the app is launched while this one is
		// running, this method is invoked, and the commandLine parameter tells
		// you what the other instance's command-line arguments were.
	}

	//==============================================================================
	/*
		This class implements the desktop window that contains an instance of
		our MainComponent class.
	*/
	class MainDialog : public DocumentWindow {
	public:
		MainDialog(String name)
			: DocumentWindow(
				name,
				Desktop::getInstance().getDefaultLookAndFeel().findColour(
					ResizableWindow::backgroundColourId),
				DocumentWindow::allButtons) {
			setUsingNativeTitleBar(true);
			setContentOwned(new MainWindow(*this), true);

			// #if JUCE_IOS || JUCE_ANDROID
			centreWithSize(getWidth(), getHeight());
#ifdef _WIN32
			setFullScreen(false);
#endif
			// #else
			setResizable(false, false);
			//
			// #endif

			setVisible(true);
		}

		void closeButtonPressed() override {
			// This is called when the user tries to close this window. Here,
			// we'll just ask the app to quit when this happens, but you can
			// change this to do whatever you need.
			//setVisible(false);
			JUCEApplication::getInstance()->systemRequestedQuit();
		}

		/* Note: Be careful if you override any DocumentWindow methods - the
		   base class uses a lot of them, so by overriding you might break its
		   functionality. It's best to do all your work in your content
		   component instead, but if you really have to override any
		   DocumentWindow methods, make sure your subclass also calls the
		   superclass's method.
		*/
		void minimiseButtonPressed() override {}
		void maximiseButtonPressed() override {}

	private:
		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainDialog)
	};

private:
	std::unique_ptr<MainDialog> mainDlg;
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION(ExactonApp)
