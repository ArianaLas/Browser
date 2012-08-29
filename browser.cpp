#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QIcon>
#include <QUrl>
#include <QString>
#include <QWebFrame>
#include <QWebHistory>
#include <QLabel>
#include <QTabWidget>
#include <QDockWidget>
#include "browser.hpp"

Browser::Browser(QWidget *parent) {
	setWindowTitle("Browser");
	resize(800, 500);
	initUI();
}

void Browser::initUI() {
	QAction *new_tab = new QAction(QIcon::fromTheme("window-new"), "&New Tab", 0);
	new_tab->setShortcut(QKeySequence::AddTab);
	QAction *bookmarks = new QAction(QIcon::fromTheme("user-bookmarks"), "&Bookmarks", 0);
	QAction *history = new QAction("&History", 0);
	QAction *help = new QAction(QIcon::fromTheme("help-about"), "&Help", 0);
	QAction *quit = new QAction(QIcon::fromTheme("window-close"), "&Quit", 0);
	new_tab->setIconVisibleInMenu(true);
	bookmarks->setIconVisibleInMenu(true);
	help->setIconVisibleInMenu(true);
	quit->setIconVisibleInMenu(true);
	QMenuBar *menu_bar = menuBar();
	QMenu *menu = menu_bar->addMenu("File");
	menu->addAction(new_tab);
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

	stacked_main = new QStackedWidget(this);
	setCentralWidget(stacked_main);
/*
	connect(prev, SIGNAL(triggered()), web_view, SLOT(back()));
	connect(next, SIGNAL(triggered()), web_view, SLOT(forward()));
	connect(refresh, SIGNAL(triggered()), web_view, SLOT(reload()));
	connect(stop, SIGNAL(triggered()), web_view, SLOT(stop()));
	connect(web_view, SIGNAL(urlChanged(QUrl)), this, SLOT(updateUrlBar(QUrl)));
	connect(web_view, SIGNAL(titleChanged(QString)), this, SLOT(titleChange(QString)));
	*/
	connect(url_bar, SIGNAL(returnPressed()), this, SLOT(urlRequested()));


	status_bar = statusBar();

	stacked_widget = new QStackedWidget(this);
	tool_bar->addWidget(stacked_widget);

	progress_bar = new QProgressBar(this);
	stacked_widget->addWidget(url_bar);
	stacked_widget->addWidget(progress_bar);
	stacked_widget->setCurrentWidget(url_bar);
	//connect(web_view, SIGNAL(loadStarted()), this, SLOT(loadStarted()));
	//connect(web_view, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
	//connect(web_view, SIGNAL(loadProgress(int)), progress_bar, SLOT(setValue(int)));
	//connect(web_view, SIGNAL(iconChanged()), this, SLOT(iconChanged()));
	//setWindowIcon(web_view->icon());

	//QWebPage *web_page = web_view->page();
	//connect(web_page, SIGNAL(linkHovered(QString, QString, QString)), this, SLOT(linkHovered(QString, QString, QString)));

	tool_bar_bottom = new QToolBar(this);
	addToolBar(Qt::BottomToolBarArea, tool_bar_bottom);
	QLabel *label = new QLabel("Find: ");
	phrase = new QLineEdit();
	tool_bar_bottom->addWidget(label);
	tool_bar_bottom->addWidget(phrase);
	tool_bar_bottom->hide();

	connect(phrase, SIGNAL(returnPressed()), this, SLOT(find()));
	
	tab_bar = new QTabBar();
	tab_bar->setTabsClosable(true);

	QDockWidget *tab_dock = new QDockWidget(this);
	tab_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
	tab_dock->setTitleBarWidget(new QWidget());
	tab_dock->setWidget(tab_bar);
	addDockWidget(Qt::TopDockWidgetArea, tab_dock);
	newTab();
	connect(new_tab, SIGNAL(triggered()), this, SLOT(newTab()));

	connect(tab_bar, SIGNAL(currentChanged(int)), this, SLOT(setCurrentPage(int)));

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
	getCurrentWebView()->load(QUrl(url));
}

void Browser::titleChange(const QString &title) {
	setWindowTitle(title + " - Browser");
}

void Browser::loadStarted() {
	stacked_widget->setCurrentWidget(progress_bar);
	stop->setDisabled(false);
	status_bar->showMessage(url_bar->text());

}

void Browser::loadFinished(bool ok) {
	stacked_widget->setCurrentWidget(url_bar);
	stop->setDisabled(true);
	prev->setDisabled(!web_view->history()->canGoBack());
	next->setDisabled(!web_view->history()->canGoForward());
	status_bar->clearMessage();
}

void Browser::iconChanged() {
	setWindowIcon(web_view->page()->mainFrame()->icon());
}

void Browser::linkHovered(const QString &url, const QString &title, const QString &content) {
	if (title == "") {
		status_bar->showMessage(url);
	} else {
		status_bar->showMessage(title);
	}
}

void Browser::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Slash) {
		tool_bar_bottom->show();
		phrase->setFocus(Qt::ShortcutFocusReason);
	} else if (event->key() == Qt::Key_Escape) {
		tool_bar_bottom->hide();
		status_bar->clearMessage();
		web_view->findText("");
	} else if (event->key() == Qt::Key_F3 && !tool_bar_bottom->isHidden()) {
		find();	
	}
}

void Browser::find() {
	if (web_view->findText(phrase->text(), QWebPage::FindWrapsAroundDocument)) {
		status_bar->showMessage("Found: \"" + phrase->text() + "\"");
	} else {
		status_bar->showMessage("Not found: \"" + phrase->text() + "\"");
	}
}

void Browser::newTab() {
	tab_bar->setCurrentIndex(tab_bar->addTab(QIcon::fromTheme("window-new"), "New tab")); // addTab() returns index
	stacked_main->setCurrentIndex(stacked_main->addWidget(new QWebView));
	url_bar->clear();
}

void Browser::setCurrentPage(int index) {
	stacked_main->setCurrentIndex(index);
	updateUrlBar(getCurrentWebView()->url().toString());
}

QWebView* Browser::getCurrentWebView() {
	return dynamic_cast<QWebView*>(stacked_main->currentWidget()); // conversely to polymorphysm
}
