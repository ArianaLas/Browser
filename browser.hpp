#ifndef _BROWSER_HPP_
#define _BROWSER_HPP_
#include <QMainWindow>
#include <QObject>
#include <QWebView>
#include <QLineEdit>
#include <QProgressBar>
#include <QStackedWidget>
#include <QAction>

class Browser : public QMainWindow {
	
	Q_OBJECT // if class Browser has own signals and slots

	public:
		Browser(QWidget *parent = 0); //0, because first window has no parent 
	
	private:
		QLineEdit *url_bar;
		QWebView *web_view;
		QProgressBar *progress_bar;
		QStackedWidget *stacked_widget;
		QAction *prev;
		QAction *next;
		QAction *stop;
		void initUI();
		
	private slots:
		void updateUrlBar(const QUrl &url);
		void urlRequested();
		void titleChange(const QString &title);
		void loadStarted();
		void loadFinished(bool ok);
			
};

#endif
