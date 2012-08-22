#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QIcon>
#include <QUrl>
#include <QString>
#include <QWebHistory>
#include "browser.hpp"

Browser::Browser(QWidget *parent) {
	setWindowTitle("Browser");
	resize(800, 500);
	initUI();
}

void Browser::initUI() {
	QAction *bookmarks = new QAction(QIcon::fromTheme("user-bookmarks"), "&Bookmarks", 0);
	QAction *history = new QAction("&History", 0);
	QAction *help = new QAction(QIcon::fromTheme("help-about"), "&Help", 0);
	QAction *quit = new QAction(QIcon::fromTheme("window-close"), "&Quit", 0);
	bookmarks->setIconVisibleInMenu(true);
	help->setIconVisibleInMenu(true);
	quit->setIconVisibleInMenu(true);

	QMenuBar *menu_bar = menuBar();
	QMenu *menu = menu_bar->addMenu("File");
	menu->addAction(bookmarks);
	menu->addAction(history);
	menu->addAction(help);
	menu->addSeparator();
	menu->addAction(quit);

	next = new QAction(QIcon::fromTheme("go-next"), "&Next", 0);
	prev = new QAction(QIcon::fromTheme("go-previous"), "&Prev", 0);
	QAction *refresh = new QAction(QIcon::fromTheme("view-refresh"), "&Refresh", 0);
	stop = new QAction(QIcon::fromTheme("process-stop"), "&Stop", 0);
	prev->setIconVisibleInMenu(true);
	next->setIconVisibleInMenu(true);
	refresh->setIconVisibleInMenu(true);
	stop->setIconVisibleInMenu(true);

	QMenu *page = menu_bar->addMenu("Page");
	page->addAction(prev);
	page->addAction(next);
	page->addAction(refresh);
	page->addAction(stop);

	QToolBar *tool_bar = addToolBar("Main Toolbar");
	tool_bar->addAction(prev);
	tool_bar->addAction(next);
	tool_bar->addAction(refresh);
	tool_bar->addAction(stop);

	url_bar = new QLineEdit();
	web_view = new QWebView(this);
	web_view->load(QUrl("http://www.google.com"));
	setCentralWidget(web_view);

	connect(prev, SIGNAL(triggered()), web_view, SLOT(back()));
	connect(next, SIGNAL(triggered()), web_view, SLOT(forward()));
	connect(refresh, SIGNAL(triggered()), web_view, SLOT(reload()));
	connect(stop, SIGNAL(triggered()), web_view, SLOT(stop()));
	connect(web_view, SIGNAL(urlChanged(QUrl)), this, SLOT(updateUrlBar(QUrl)));
	connect(url_bar, SIGNAL(returnPressed()), this, SLOT(urlRequested()));
	connect(web_view, SIGNAL(titleChanged(QString)), this, SLOT(titleChange(QString)));

	QStatusBar *status_bar = statusBar();
	// FIXME: check this connect
	connect(web_view, SIGNAL(statusBarMessage(QString)), status_bar, SLOT(showMessage(QString)));

	stacked_widget = new QStackedWidget(this);
	tool_bar->addWidget(stacked_widget);

	progress_bar = new QProgressBar(this);
	stacked_widget->addWidget(url_bar);
	stacked_widget->addWidget(progress_bar);
	stacked_widget->setCurrentWidget(url_bar);
	connect(web_view, SIGNAL(loadStarted()), this, SLOT(loadStarted()));
	connect(web_view, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));

	connect(web_view, SIGNAL(loadProgress(int)), progress_bar, SLOT(setValue(int)));

}

void Browser::updateUrlBar(const QUrl &url) {
	url_bar->setText(url.toString()); //change from QUrl to QString
	url_bar->setCursorPosition(0);
}

void Browser::urlRequested() {
	QString url = url_bar->text();
	if (!url.startsWith("http://")) {
		url.push_front("http://");
	}
	web_view->load(QUrl(url));
}

void Browser::titleChange(const QString &title) {
	setWindowTitle(title + " - Browser");
}

void Browser::loadStarted() {
	stacked_widget->setCurrentWidget(progress_bar);
	stop->setDisabled(false);
		
}

void Browser::loadFinished(bool ok) {
	stacked_widget->setCurrentWidget(url_bar);
	stop->setDisabled(true);
	prev->setDisabled(!web_view->history()->canGoBack());
	next->setDisabled(!web_view->history()->canGoForward());
}
