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

void MWBotWin::doChest(int id, int dbg) {
	QWebElement elm = getItemElement(id);
	if (elm.isNull()) return;
	QWebElement itm = elm.findFirst("div.action span");
	qDebug() << itm.toPlainText();
	click(ui.browser, itm);
	FightBox res = getChestResult();
	if (res.items.isEmpty()) {
		pause(1.0);
		waitLoading(ui.browser);
		res = getChestResult();
	}
	logResult(res);
}

// 5573 : shaurma box
// 5574 : shaurma key

struct keyChestItem {
	int chestId;
	int keyId;
	QString chestName;
	QString keyName;
};

const keyChestItem kcTab[] = {
	{4021, 4020, QDialog::trUtf8("малая шкатулка партии"), QDialog::trUtf8("ключ партии")},
	{4022, 4020, QDialog::trUtf8("средняя шкатулка партии"), QDialog::trUtf8("ключ партии")},
	{4023, 4020, QDialog::trUtf8("большая шкатулка партии"), QDialog::trUtf8("ключ партии")},

	{3348, 3347, QDialog::trUtf8("малый ларец"), QDialog::trUtf8("ключ от шахтерского ларца")},
	{3349, 3347, QDialog::trUtf8("средний ларец"), QDialog::trUtf8("ключ от шахтерского ларца")},
	{3350, 3347, QDialog::trUtf8("большой ларец"), QDialog::trUtf8("ключ от шахтерского ларца")},

	{5573, 5574, "почтовый ящик", "ключ от почтового ящика"},

	{-1, -1, "", ""}
};

void MWBotWin::checkChests(FightBox res) {
	int idx;
	mwItem itm;
//	qDebug() << 1 << res.items.size();
	foreach(itm, res.items) {
		idx = 0;
		while (kcTab[idx].chestId > 0) {
			if (itm.name.toLower() == kcTab[idx].chestName) {
				qDebug() << "chest" << itm.name;
				opt.chest.need = 1;
			} else if (itm.name.toLower() == kcTab[idx].keyName) {
				qDebug() << "key" << itm.name;
				opt.chest.need = 1;
			}
			idx++;
		}
	}
}

void MWBotWin::openChests(int dbg) {
	int cid,kid;
	int ccn,kcn;
	int idx = 0;
	setBusy(true);
	while (kcTab[idx].chestId > 0) {
		cid = kcTab[idx].chestId;
		kid = kcTab[idx].keyId;
		ccn = getItem(cid);
		kcn = getItem(kid);
		if (dbg) {
			qDebug() << "chest" << cid << ccn << "key" << kid << kcn;
		}
		if ((ccn > 0) && (kcn > 0)) {
			doChest(cid, dbg);
		}
		idx++;
	}
	opt.chest.need = 0;
	setBusy(false);
}

/*
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
*/
