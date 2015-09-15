#include "main.h"

void MWBotWin::gipsy() {
	setBusy(true);
	clickElement("a.square-middlestreet");
	clickElement("a[href='/camp/'] div.c");
	clickElement("a[href='/camp/gypsy/'] div.c");
	QWebElement elm = frm->findFirstElement("div.log span.amulet-reward");
	int gold = elm.toPlainText().toInt();
	while ((gold > (goldType-1)) && (~flag & FL_STOP)) {
		switch(goldType) {
			case 750: clickElement("div#startGameButton1 div.c"); break;
//			case 150: clickElement("div#startGameButton2 div.c",0); break;
			default: clickElement("div#startGameButton0 div.c"); break;
		}
		clickElement("div.history div.button div.c");
		ui.browser->reload();
		waitLoading();
		elm = frm->findFirstElement("div.log span.amulet-reward");
		if (elm.isNull()) break;
		gold = elm.toPlainText().toInt();
	}
	setBusy(false);
}
