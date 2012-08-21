#ifndef _BROWSER_HPP_
#define _BROWSER_HPP_
#include <QMainWindow>

class Browser : public QMainWindow {
	public:
		Browser(QWidget *parent = 0); //0, bo pierwsze okno nie ma parenta
	
	private:
		void initUI();
};

#endif
