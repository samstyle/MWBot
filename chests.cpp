#include "main.h"

QWebElement MWBotWin::getItemElement(int id) {
	loadPage("player");
	QWebElementCollection elms = frm->findAllElements("table.inventary td.equipment-cell div.object-thumb");
	QWebElement elm,itm,res;
	foreach(elm, elms) {
		itm = elm.findFirst("img");
		if (itm.attribute("data-st").toInt() == id) {
			res = elm;
		}
	}
	return res;
}

int MWBotWin::getItem(int id) {
	QWebElement elm = getItemElement(id);
	int res = 0;
	if (elm.isNull()) return res;
	QWebElement itm = elm.findFirst("div.count");
	if (itm.isNull()) {
		res = 1;
	} else {
		res = itm.toPlainText().remove("#").toInt();
	}
	return res;
}

void MWBotWin::doChest(int id) {
	QWebElement elm = getItemElement(id);
	if (elm.isNull()) return;
	QWebElement itm = elm.findFirst("div.action span");
//	qDebug() << itm.toPlainText();
	clickElement(itm);
	FightBox res = getChestResult();
	logResult(res);
}

// 5573 : shaurma box
// 5574 : shaurma key

void MWBotWin::openChests() {
	if (!(opt.chest.keyOil || opt.chest.keyRat || opt.chest.keyElect)) return;		// no keys
	setBusy(true);
// get keys count if undef
	if (opt.chest.keyOil < 0)
		opt.chest.keyOil = getItem(4020);	// 4020 : oil key
	if (opt.chest.keyRat < 0)
		opt.chest.keyRat = getItem(3347);	// 3347 : rat key
	if (opt.chest.keyElect < 0)
		opt.chest.keyElect = getItem(5723);	// 5723 : election key
// open oil chests
	if (opt.chest.keyOil) {
		if (getItem(4023)) {			// 4023 : blue oil chest
			doChest(4023);
			opt.chest.keyOil--;
		} else if (getItem(4022)) {		// 4022 : green oil chest
			doChest(4022);
			opt.chest.keyOil--;
		}
	} else if (opt.chest.keyRat) {
		if (getItem(3348)) {			// 3348 : small rat chest
			doChest(3348);
			opt.chest.keyRat--;
		}
	} else if (getItem(5574) && getItem(5573)) {	// 5573/5574 : shaurma
		doChest(5573);
	}
	setBusy(false);
}
