#include "main.h"

void MWBotWin::gipsy() {
	setBusy(true);
	click(ui.browser, "a.square-middlestreet");
	click(ui.browser, "a[href='/camp/'] div.c");
	click(ui.browser, "a[href='/camp/gypsy/'] div.c");
	QWebElement elm = frm->findFirstElement("div.log span.amulet-reward");
	int gold = elm.toPlainText().toInt();
	while ((gold > (goldType-1)) && (~flag & FL_STOP)) {
		switch(goldType) {
			case 750: click(ui.browser, "div#startGameButton1 div.c"); break;
//			case 150: click(ui.browser, "div#startGameButton2 div.c",0); break;
			default: click(ui.browser, "div#startGameButton0 div.c"); break;
		}
		click(ui.browser, "div.history div.button div.c");
		ui.browser->reload();
		waitLoading(ui.browser);
		elm = frm->findFirstElement("div.log span.amulet-reward");
		if (elm.isNull()) break;
		gold = elm.toPlainText().toInt();
	}
	setBusy(false);
}
