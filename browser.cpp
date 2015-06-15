/*
 * This file is part of Browser.
 *
 * Browser is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Browser is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Browser.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
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
	help->setShortcut(QKeySequence::HelpContents);
	QAction *quit = new QAction(QIcon::fromTheme("window-close"), "&Quit", 0);
	quit->setShortcut(QKeySequence::Quit);
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

	connect(quit, SIGNAL(triggered()), this, SLOT(quit()));

	next = new QAction(QIcon::fromTheme("go-next"), "&Next", 0);
	next->setShortcut(QKeySequence::Forward);
	prev = new QAction(QIcon::fromTheme("go-previous"), "&Prev", 0);
	prev->setShortcut(QKeySequence::Back);
	QAction *refresh = new QAction(QIcon::fromTheme("view-refresh"), "&Refresh", 0);
	refresh->setShortcut(QKeySequence::Refresh);
	stop_action = new QAction(QIcon::fromTheme("process-stop"), "&Stop", 0);
	prev->setIconVisibleInMenu(true);
	next->setIconVisibleInMenu(true);
	refresh->setIconVisibleInMenu(true);
	stop_action->setIconVisibleInMenu(true);
	QMenu *page = menu_bar->addMenu("Page");
	page->addAction(prev);
	page->addAction(next);
	page->addAction(refresh);
	page->addAction(stop_action);
	QToolBar *tool_bar = addToolBar("Main Toolbar");
	tool_bar->addAction(prev);
	tool_bar->addAction(next);
	tool_bar->addAction(refresh);
	tool_bar->addAction(stop_action);

	url_bar = new QLineEdit();

	stacked_main = new QStackedWidget(this);
	setCentralWidget(stacked_main);
	connect(prev, SIGNAL(triggered()), this, SLOT(back()));
	connect(next, SIGNAL(triggered()), this, SLOT(forward()));
	connect(stop_action, SIGNAL(triggered()), this, SLOT(stop()));
	connect(refresh, SIGNAL(triggered()), this, SLOT(reload()));
	connect(url_bar, SIGNAL(returnPressed()), this, SLOT(urlRequested()));

	status_bar = statusBar();

	stacked_widget = new QStackedWidget(this);
	tool_bar->addWidget(stacked_widget);

	progress_bar = new QProgressBar(this);
	stacked_widget->addWidget(url_bar);
	stacked_widget->addWidget(progress_bar);
	stacked_widget->setCurrentWidget(url_bar);
	//setWindowIcon(web_view->icon());

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
	connect(tab_bar, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequested(int)));
	// FIXME: It's impossible to edit url_bar when a page is loading
	// (on another tab)

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

void Browser::titleChanged(const QString &title) {
	setWindowTitle(title + " - Browser");
}

void Browser::loadStarted() {
	stacked_widget->setCurrentWidget(progress_bar);
	stop_action->setDisabled(false);
	status_bar->showMessage(url_bar->text());
}

void Browser::loadFinished(bool ok) {
	stacked_widget->setCurrentWidget(url_bar);
	stop_action->setDisabled(true);
	prev->setDisabled(!getCurrentWebView()->history()->canGoBack());
	next->setDisabled(!getCurrentWebView()->history()->canGoForward());
	status_bar->clearMessage();
}

void Browser::iconChanged() {
	setWindowIcon(getCurrentWebView()->page()->mainFrame()->icon());
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
		getCurrentWebView()->findText("");
	} else if (event->key() == Qt::Key_F3 && !tool_bar_bottom->isHidden()) {
		find();	
	}
}

void Browser::find() {
	if (
		getCurrentWebView()->findText(
			phrase->text(), QWebPage::FindWrapsAroundDocument)
	) {
		status_bar->showMessage("Found: \"" + phrase->text() + "\"");
	} else {
		status_bar->showMessage("Not found: \"" + phrase->text() + "\"");
	}
}

void Browser::newTab() {
	QWebView* web_view = new QWebView();
	// addTab() returns index
	tab_bar->setCurrentIndex(
		tab_bar->addTab(QIcon::fromTheme("window-new"), "New tab")
	);
	stacked_main->setCurrentIndex(stacked_main->addWidget(web_view));
	url_bar->clear();
	connect(web_view, SIGNAL(urlChanged(QUrl)), this, SLOT(updateUrlBar(QUrl)));
	connect(web_view, SIGNAL(titleChanged(QString)), this, SLOT(titleChanged(QString)));
	connect(web_view, SIGNAL(loadStarted()), this, SLOT(loadStarted()));
	connect(web_view, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));
	connect(web_view, SIGNAL(loadProgress(int)), progress_bar, SLOT(setValue(int)));
	connect(web_view, SIGNAL(iconChanged()), this, SLOT(iconChanged()));

	QWebPage *web_page = web_view->page();
	connect(
		web_page,
		SIGNAL(linkHovered(QString, QString, QString)),
		this,
		SLOT(linkHovered(QString, QString, QString))
	);
}

void Browser::setCurrentPage(int index) {
	stacked_main->setCurrentIndex(index);
	titleChanged(getCurrentWebView()->title());
	updateUrlBar(getCurrentWebView()->url().toString());
	prev->setDisabled(!getCurrentWebView()->history()->canGoBack());
	next->setDisabled(!getCurrentWebView()->history()->canGoForward());
}

QWebView* Browser::getCurrentWebView() {
	return dynamic_cast<QWebView*>(stacked_main->currentWidget()); // conversely to polymorphysm
}

void Browser::back() {
	getCurrentWebView()->back();
}

void Browser::forward() {
	getCurrentWebView()->forward();
}

void Browser::stop() {
	getCurrentWebView()->stop();
}

void Browser::reload() {
	getCurrentWebView()->reload();
}

void Browser::quit() {
	QApplication::quit();
}

void Browser::tabCloseRequested(int index) {
	// Last tab must persist
	// TODO: Hide tab when count() == 1
	if (tab_bar->count() > 1) {
		tab_bar->removeTab(index);
		stacked_main->setCurrentIndex(index);
		QWidget* widget = stacked_main->currentWidget();
		stacked_main->removeWidget(widget);
		delete widget;
	}
}
