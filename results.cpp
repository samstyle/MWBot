#include "main.h"

// NEW

FightBox MWBotWin::getDuelResult() {
	FightBox res;
	int mylife;
	int enlife;
	QWebElement elm;
	res.enemy = getStat("div.fighter2","td.fighter2-cell");
	clickElement("i.icon.icon-forward");				// forward button
	mylife = frm->findFirstElement("span#fighter1-life").toPlainText().split("/").first().trimmed().toInt();
	enlife = frm->findFirstElement("span#fighter2-life").toPlainText().split("/").first().trimmed().toInt();
	res.result = (mylife > 0) ? 1 : ((enlife > 0) ? 0 : 2);		// duel result (0,1,2 = lose,win,draw)
	res.items = getDuelResultMain();
	elm = frm->findFirstElement("div.backlink div.button div.c");
	if (!elm.isNull()) {
		clickElement(elm);
	}
	res.items.append(getDuelResultExtra());
	return res;
}

FightBox MWBotWin::getGroupResult() {
	FightBox res;
	QWebElement elm;
	QString str;
	elm = frm->findFirstElement("form#fightGroupForm h3 div.group2");
	str = elm.toPlainText();
	res.enemy.name = str.split("(").first().trimmed();
	res.enemy.type = elm.findFirst("i").attribute("class");
	res.enemy.level = -1;
	res.result = str.contains("(0/") ? 1 : 0;
	res.items = getGroupResultMain();
	elm = frm->findFirstElement("div.result div.button div.c");
	if (!elm.isNull()) {
		clickElement(elm);
	}
	res.items.append(getDuelResultExtra());
	return res;
}

FightBox MWBotWin::getChestResult() {
	FightBox res;
	mwItem item;
	QWebElement elm,itm;
	QWebElementCollection elms = frm->findAllElements("div#alert-text span.object-thumb");
	res.result = 3;		// chest
	res.enemy.name = frm->findFirstElement("div#alert-text b").toPlainText();
	foreach(elm, elms) {
		itm = elm.findFirst("img");
		item.name = itm.attribute("title");
		item.icn = getItemIcon(item.name);
		itm = elm.findFirst("div.count");
		if (itm.isNull()) {
			item.count = 1;
		} else {
			item.count = itm.toPlainText().remove("#").toInt();
		}
		res.items.append(item);
	}
	return res;
}

QList<mwItem> MWBotWin::getDuelResultMain() {
	QList<mwItem> res;
	mwItem obj;
	QList<QWebElement> lst = frm->findAllElements("li.result div span").toList();
	QWebElement elm;
	foreach(elm, lst) {
		obj.count = 0;
		if (!elm.classes().contains("count") && (elm.classes().size() > 0)) {
			if (elm.classes().contains("object-thumb")) {
				obj.name = elm.findFirst("img").attribute("alt");
				obj.count = elm.findFirst("span.count").toPlainText().toInt();
			} else {
				obj.name = elm.classes().first();
				obj.count = elm.toPlainText().remove(",").toInt();
			}
			if (obj.count == 0)
				obj.count = 1;
			res.append(obj);
		}
	}
	return res;
}

QList<mwItem> MWBotWin::getDuelResultExtra() {
	QList<mwItem> res;
	mwItem obj;
	QList<QWebElement> lst = frm->findAllElements("div#alert-text span").toList();
	QWebElement elm;
	foreach(elm, lst) {
		obj.count = 0;
		if (!elm.classes().contains("count") && (elm.classes().size() > 0)) {
			if (elm.classes().contains("object-thumb")) {
				obj.name = elm.findFirst("img").attribute("alt");
				obj.count = elm.findFirst("div.count").toPlainText().remove("#").toInt();
				if (obj.count == 0) {
					obj.count = elm.findFirst("span.count").toPlainText().remove("#").toInt();
				}
			} else {
				obj.name = elm.classes().first();
				obj.count = elm.toPlainText().remove("\"").remove(",").toInt();
			}
			if (obj.count == 0)
				obj.count = 0;
			res.append(obj);
		}
	}
	return res;
}

QList<mwItem> MWBotWin::getGroupResultMain() {
	QList<mwItem> res;
	mwItem obj;
	QList<QWebElement> lst = frm->findAllElements("li div.result div span").toList();
	QWebElement elm;
	foreach(elm, lst) {
		obj.count = 0;
		if (!elm.classes().contains("count") && (elm.classes().size() > 0)) {
			if (elm.classes().contains("object-thumb")) {
				obj.name = elm.findFirst("img").attribute("alt");
				obj.count = elm.findFirst("span.count").toPlainText().toInt();
			} else {
				obj.name = elm.classes().first();
				obj.count = elm.toPlainText().remove(",").toInt();
			}
			if (obj.count == 0)
				obj.count = 1;
			res.append(obj);
		}
	}
	return res;
}

// OLD

// get atack result
bool MWBotWin::atkResult() {
	int res = 0;
	res = fightResult();
	if (res < 0) {
		return false;
	}
	loadPage("alley/");
	getAtackTimer();
	return true;
}

int MWBotWin::fightResult() {
	FightBox res = getDuelResult();
	logResult(res);
	return res.result;
}

void MWBotWin::logResult(FightBox res) {
	QString tolog;
	QString icn;
	QString fightres;
	QString enname;
	QString nname = getItemIcon(res.enemy.type);
	if (res.enemy.level < 1) {
		enname = QString("%0").arg(res.enemy.name);
	} else {
		enname = QString("%0 [%1]").arg(res.enemy.name).arg(res.enemy.level);
	}
	if (res.result == 2) {		// draw
		tolog = QString("<img src=:/images/unknown.png>&nbsp;<img src=%0>&nbsp;<b>%1</b></font>").arg(nname).arg(enname);
	} else {
		switch (res.result) {
			case 0:		// lose
				tolog = QString("<img src=:/images/stop.png>&nbsp;<img src=%0>&nbsp;<b>%1</b>&nbsp;").arg(nname).arg(enname);
				break;
			case 1:		// win
				tolog = QString("<img src=:/images/yes.png>&nbsp;<img src=%0>&nbsp;<b>%1</b>&nbsp;").arg(nname).arg(enname);
				break;
			case 3:		// chest
				tolog = trUtf8("Открыт сундук <b>%0</b>&nbsp;").arg(res.enemy.name);
				break;
		}
		fightres.append("<b>");
		foreach(mwItem obj,res.items) {
			if ((obj.name == "tugriki") || (res.result != 0)) {
				icn = getItemIcon(obj.name);
				if (icn.isEmpty()) {
					icn = ":/images/unknown.png";
				}
				fightres.append(QString("<span style=background-color:#d0d0d0;>\
						&nbsp;%0&nbsp;<img width=16 height=16 src='%1' title='%2'>&nbsp;\
						</span>&nbsp;").arg(obj.count).arg(icn).arg(obj.name));
			}
		}
		fightres.append("</b>");
		tolog.append(fightres);
	}
	log(tolog);
}
