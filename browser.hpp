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

#ifndef _BROWSER_HPP_
#define _BROWSER_HPP_
#include <QMainWindow>
#include <QObject>
#include <QWebView>
#include <QLineEdit>
#include <QProgressBar>
#include <QStackedWidget>
#include <QAction>
#include <QKeyEvent>
#include <QToolBar>
#include <QTabBar>

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
		QStatusBar *status_bar;
		QToolBar *tool_bar_bottom;
		QLineEdit *phrase;
		QStackedWidget *stacked_main;
		QTabBar *tab_bar;
		void initUI();
		QWebView* getCurrentWebView();

	protected:
		void keyPressEvent(QKeyEvent *event);
		
	private slots:
		void updateUrlBar(const QUrl &url);
		void urlRequested();
		void titleChange(const QString &title);
		void loadStarted();
		void loadFinished(bool ok);
		void iconChanged();
		void linkHovered(const QString &url, const QString &title, const QString &content);
		void find();
		void newTab();
		void setCurrentPage(int index);
		
			
};

#endif
