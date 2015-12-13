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
		res.items.append(getDuelResultExtra());
	}
	return res;
}

FightBox MWBotWin::getGroupResult() {

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
				obj.count = elm.findFirst("span.count").toPlainText().remove("#").toInt();
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

}

QList<mwItem> MWBotWin::getGroupResultExtra() {

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
	QString enname = QString("%0 [%1]").arg(res.enemy.name).arg(res.enemy.level);
	QString tolog;
	QString icn;
	QString fightres;
	QString nname;
	nname = getItemIcon(res.enemy.type);
	if (res.result == 2) {
		tolog = QString("<font style=background-color:#e0e020><img src=%0>&nbsp;%1</font>").arg(nname).arg(enname);
	} else {
		if (res.result == 1) {
			tolog = QString("<font style=background-color:#20e020><img src=%0>&nbsp;").arg(nname);
		} else {
			tolog = QString("<font style=background-color:#e02020><img src=%0>&nbsp;").arg(nname);
		}
		tolog.append(enname).append("</font>&nbsp;");
		foreach(mwItem obj,res.items) {
			fightres.append("<font style=background-color:#d0d0d0>");
			icn = getItemIcon(obj.name);
			if (icn.isEmpty()) {
				icn = ":/images/unknown.png";
			}
			fightres.append(QString("<img width=16 height=16 src='%0' title='%1'>").arg(icn).arg(obj.name));
			fightres.append(QString("&nbsp;%0</font>&nbsp;").arg(obj.count));
		}
		tolog.append(fightres);
	}
	log(tolog);
	return res.result;
}

/*
FightBox MWBotWin::getResult() {
	FightBox res;
	QWebElement elm;
	mwItem obj;
	QWebElementCollection coll;
	int mylife = frm->findFirstElement("span#fighter1-life").toPlainText().split("/").first().trimmed().toInt();
	int enlife = frm->findFirstElement("span#fighter2-life").toPlainText().split("/").first().trimmed().toInt();
	res.result = (mylife > 0) ? 1 : ((enlife > 0) ? 0 : 2);

	obj.name = trUtf8("деньги");
	obj.count = 0;

	coll=frm->findAllElements("li.result span.tugriki");

	foreach(elm,coll) {
		if (elm.attribute("title").isNull()) {
			if (res.result == 1) obj.count += elm.toPlainText().trimmed().toInt();
		} else {
			obj.count += elm.attribute("title").split(":").last().trimmed().toInt();
		}
	}
	if (obj.count != 0) res.items.append(obj);

	if (res.result == 1) {			// check all only if win

		obj.name = trUtf8("опыт");
		obj.count = frm->findFirstElement("li.result span.expa").attribute("title").split(":").last().trimmed().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("нефть");
		obj.count = frm->findFirstElement("li.result span.neft").attribute("title").split(":").last().trimmed().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("пули");
		obj.count = frm->findFirstElement("li.result span.bullet").toPlainText().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("звездочки");
		obj.count = frm->findFirstElement("li.result span.sparkles").toPlainText().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("снежинки");
		obj.count = frm->findFirstElement("li.result span.snowflake").toPlainText().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("жетоны");
		obj.count = frm->findFirstElement("li.result span.badge").toPlainText().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("мобила");
		obj.count = frm->findFirstElement("li.result span.mobila").isNull() ? 0 : 1;
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("подписи");
		obj.count = frm->findFirstElement("li.result span.party_signature").toPlainText().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("зубы");
		obj.count = frm->findFirstElement("li.result span.tooth-white").toPlainText().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("золотые зубы");
		obj.count = frm->findFirstElement("li.result span.tooth-golden").toPlainText().toInt();
		if (obj.count != 0) res.items.append(obj);

		coll = frm->findAllElements("li.result span.object-thumb");
		//int idx;
		foreach(elm, coll) {
			obj.count = elm.findFirst("span.count").toPlainText().toInt();
			if (obj.count != 0) {
				obj.name = elm.findFirst("img").attribute("alt");
				res.items.append(obj);
			}
		}
	}
	elm = frm->findFirstElement("div.backlink div.button div.c");
	if (elm.isNull()) return res;

	clickElement("div.backlink div.button div.c");

	obj.name = trUtf8("руда");
	obj.count = frm->findFirstElement("div#alert-text span.ruda").toPlainText().remove("\"").toInt();
	if (obj.count != 0) res.items.append(obj);

	obj.name = trUtf8("деньги");
	obj.count = frm->findFirstElement("div#alert-text span.tugriki").toPlainText().remove("\"").toInt();
	if (obj.count != 0) res.items.append(obj);

	coll = frm->findAllElements("div#alert-text span.object-thumb");
	foreach(elm,coll) {
		obj.name = elm.findFirst("img").attribute("alt");
		obj.count = elm.findFirst("span.count").toPlainText().remove("#").toInt();
		if (obj.count == 0) obj.count = 1;
		res.items.append(obj);
	}
	return res;
}
*/
