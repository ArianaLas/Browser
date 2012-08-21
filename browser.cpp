#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QIcon>
#include <QWebView>
#include <QUrl>
#include <QString>
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
	menu->addSeparator();
	menu->addAction(help);
	menu->addAction(quit);

	QAction *next = new QAction(QIcon::fromTheme("go-next"), "&Next", 0);
	QAction *prev = new QAction(QIcon::fromTheme("go-previous"), "&Prev", 0);
	QAction *refresh = new QAction(QIcon::fromTheme("view-refresh"), "&Refresh", 0);
	QAction *stop = new QAction(QIcon::fromTheme("process-stop"), "&Stop", 0);
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

	QWebView *web_view = new QWebView(this);
	web_view->load(QUrl("http://www.google.com"));

	setCentralWidget(web_view);

	statusBar();
}
