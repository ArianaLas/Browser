#ifndef _BROWSER_HPP_
#define _BROWSER_HPP_
#include <QMainWindow>
#include <QObject>
#include <QWebView>

class Browser : public QMainWindow {
	
	Q_OBJECT // if class Browser has own signals and slots

	public:
		Browser(QWidget *parent = 0); //0, bo pierwsze okno nie ma parenta
	
	private:
		QWebView *web_view;
		void initUI();
};

#endif
