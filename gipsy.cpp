#include "main.h"

void MWBotWin::gipsy() {
	setBusy(true);
	clickElement("a.square-middlestreet",0);
	clickElement("a[href='/camp/gypsy/'] div.c",0);
	QWebElement elm = frm->findFirstElement("div.log span.amulet-reward");
	int gold = elm.toPlainText().toInt();
	while ((gold > 24) && (~flag & FL_STOP)) {
		clickElement("div#startGameButton0 div.c",0);
		clickElement("div.history div.button div.c",0);
		ui.browser->reload();
		waitLoading(1000);
		elm = frm->findFirstElement("div.log span.amulet-reward");
		if (elm.isNull()) break;
		gold = elm.toPlainText().toInt();
	}
	setBusy(false);
}
