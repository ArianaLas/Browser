#ifndef _BROWSER_HPP_
#define _BROWSER_HPP_
#include <QMainWindow>
#include <QObject>
#include <QWebView>
#include <QLineEdit>

class Browser : public QMainWindow {
	
	Q_OBJECT // if class Browser has own signals and slots

	public:
		Browser(QWidget *parent = 0); //0, because first window has no parent 
	
	private:
		QLineEdit *url_bar;
		QWebView *web_view;
		void initUI();
		
	private slots:
		void updateUrlBar(const QUrl &url);
		void urlRequested();
		void titleChange(const QString &title);
			
};

#endif
