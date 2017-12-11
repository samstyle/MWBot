#include "main.h"

// VERY NEW

namespace test {

mwItem getObject(QWebElement& elm) {
	mwItem itm;
	itm.name = elm.findFirst("img").attribute("title");
	QWebElement cnt = elm.findFirst("div.count");
	if (cnt.isNull()) {
		itm.count = 1;
	} else {
		itm.count = toNumber(cnt.toPlainText());
	}
	return itm;
}

// get items list from 'info alert' window
QList<mwItem> getPrize(QWebFrame* frm) {
	QList<mwItem> res;
	mwItem itm;
	QWebElement box = frm->findFirstElement("div.alert.infoalert div#alert-text div.objects");
	QWebElement elm = box.firstChild();
	while (eVisible(elm)) {
		itm = getObject(elm);
		if (itm.count > 0)
			res.append(itm);
		elm = elm.nextSibling();
	}
	return res;
}

QList<mwItem> getChestItems(QWebFrame* frm) {
	QWebElementCollection coll = frm->findAllElements("div.overtip.alert.1");
	QWebElement elm;
	QWebElement ch;
	QList<mwItem> res;
	mwItem itm;
	foreach(elm, coll) {
		if (eVisible(elm)) {
			elm = elm.findFirst("div.help div.data");
			ch = elm.firstChild();
			while (eVisible(ch)) {
				if (ch.tagName() == "span") {
					itm.name = ch.classes().join(":");
					itm.count = toNumber(ch.toPlainText());
					res.append(itm);
				} else if ((ch.tagName() == "div") && (ch.classes().contains("objects"))) {
					elm = ch;
					ch = elm.firstChild();
					while (eVisible(ch)) {
						if ((ch.tagName() == "span") && (ch.classes().contains("object-thumb"))) {
							itm = getObject(ch);
							if (itm.count > 0)
								res.append(itm);
						}
						ch = ch.nextSibling();
					}
					break;
				}
				ch = ch.nextSibling();
			}
		}
	}
	return res;
}

QList<mwItem> getDuelResult(QWebFrame* frm) {
	QList <mwItem> res;
	mwItem itm;
	QWebElement box = frm->findFirstElement("div#content table.data td.log ul.fight-log li.result");
	if (!eVisible(box)) return res;
	QWebElement ch = box.firstChild();
	QWebElement sp;
	while (eVisible(ch)) {
		if ((ch.tagName() == "div") && !ch.classes().contains("actions")) {
			sp = ch.findFirst("span");
			while (!sp.isNull()) {
				if (sp.classes().contains("object-thumb")) {
					itm = getObject(sp);
					res.append(itm);
				} else if (sp.classes().contains("icon-bike")) {
					// do nothing
				} else {
					itm.name = sp.classes().join(":");
					itm.count = toNumber(sp.toPlainText());
					res.append(itm);
				}
				sp = sp.nextSibling();
			}
		}
		ch = ch.nextSibling();
	}
	return res;
}

// return
//	-1 if no collection item found
//	% of collection completion
int getCollectionItem(QWebFrame* frm) {
	int res = -1;
	QWebElement elm = frm->findFirstElement("div.overtip.alert.alert-big");
	if (eVisible(elm)) {
		elm = elm.findFirst("div.help div.data p.holders");
		res = elm.toPlainText().split(":").last().remove("%").trimmed().toInt();
	}
	return res;
}

}

// NEW

FightBox MWBotWin::getDuelResult() {
	FightBox res;
	int mylife;
	int enlife;
	QWebElement elm;
	res.enemy = getStat("div.fighter2","td.fighter2-cell");
	click(ui.browser, "i.icon.icon-forward");				// forward button
	mylife = frm->findFirstElement("span#fighter1-life").toPlainText().split("/").first().trimmed().toInt();
	enlife = frm->findFirstElement("span#fighter2-life").toPlainText().split("/").first().trimmed().toInt();
	res.result = (mylife > 0) ? 1 : ((enlife > 0) ? 0 : 2);		// duel result (0,1,2 = lose,win,draw)
	res.items = getDuelResultMain();
	elm = frm->findFirstElement("div.backlink div.button div.c");
	if (!elm.isNull()) {
		click(ui.browser, elm);
	}
	res.items.append(getDuelResultExtra());
	checkChests(res);
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

	elm = frm->findFirstElement("td.log div.result div.button div.c");
	qDebug() << 1 << eVisible(elm);
	if (!eVisible(elm)) elm = frm->findFirstElement("td.log div.result span.button div.c");
	qDebug() << 2 << eVisible(elm);
	if (!eVisible(elm)) elm = frm->findFirstElement("td.log div.result div.backlink div.button div.c");
	qDebug() << 3 << eVisible(elm);
	if (eVisible(elm)) {
		click(ui.browser, elm);
		res.items.append(getDuelResultExtra());
	}
	checkChests(res);
	return res;
}

FightBox MWBotWin::getChestResult() {
	FightBox res;
	mwItem item;
	QWebElement elm,itm;
	QWebElementCollection elms = frm->findAllElements("div#alert-text span.object-thumb");
	res.result = resChest;
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
	checkChests(res);
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
				obj.count = elm.findFirst("span.count").toPlainText().remove("#").toInt();
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
				obj.count = 1;
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
			} else {			/* if (elm.parent().parent().attribute("id") == "alert-text") */
				obj.name = elm.classes().first();
				obj.count = elm.toPlainText().remove(",").toInt();
			//} else {
			//	obj.name.clear();
			}
			if (!obj.name.isEmpty()) {
				if (obj.count == 0) obj.count = 1;
				res.append(obj);
			}
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
	loadPage("alley");
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
	QString icon = "info.png";
	QString fightres;
	QString enname;
	QString nname = getItemIcon(res.enemy.type);
	if (res.enemy.level < 1) {
		enname = QString("%0").arg(res.enemy.name);
	} else {
		enname = QString("%0 [%1]").arg(res.enemy.name).arg(res.enemy.level);
	}
	if (res.result == resDraw) {		// draw
		tolog = QString("<img src=%0>&nbsp;<b>%1</b></font>").arg(nname).arg(enname);
		icon = "unknown.png";
	} else {
		switch (res.result) {
			case resLose:		// lose
				tolog = QString("<img src=%0>&nbsp;<b>%1</b>&nbsp;").arg(nname).arg(enname);
				icon = "no.png";
				break;
			case resWin:		// win
				tolog = QString("<img src=%0>&nbsp;<b>%1</b>&nbsp;").arg(nname).arg(enname);
				icon = "yes.png";
				break;
			case resChest:		// chest
				tolog = trUtf8("Открыт сундук <b>%0</b>&nbsp;").arg(res.enemy.name);
				icon = "chest.png";
				break;
			case resBonus:
				tolog = trUtf8("Получены предметы: ");
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
	log(tolog, icon);
}
