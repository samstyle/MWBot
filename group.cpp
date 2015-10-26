#include "main.h"

int getGroupCount(QWebFrame* frm, int ally) {
	QWebElement elm = frm->findFirstElement(ally ? "div.group1" : "div.group2");
	QString txt = elm.toPlainText();
	int pos = txt.lastIndexOf("(");
	txt = txt.mid(pos).remove(")").remove("(");
	return txt.split("/").first().toInt();
}

QString getWeakest(QWebFrame* frm, int ally) {
	QString res;
	QList<QWebElement> grps = frm->findAllElements("td.group").toList();
	QWebElement grp = ally ? grps.first() : grps.last();
	QList<QWebElement> flst = grp.findAll("li.alive").toList();
	QWebElement fgt;
	int minhp = 0;
	int hp;
	foreach(fgt, flst) {
		hp = fgt.findFirst("span.life span.number span.fighter-hp").toPlainText().split("/").first().replace("k","*1000").toInt();
		if ((minhp == 0) || (hp < minhp)) {
			minhp = hp;
			res = fgt.findFirst("label[for]").attribute("for");
		}
	}
	return res;
}

void MWBotWin::groupFight() {
	QString target;
	int res = 0;
	int cnt;
	int step = 1;
	do {
		cnt = getGroupCount(frm,1);
		if (cnt == 0) {
			res = 2;		// lose
		} else {
			cnt = getGroupCount(frm,0);
			if (cnt == 0) {
				res = 1;	// win
			} else {
				target = getWeakest(frm,0);
				target.prepend("input.radio-attack#");
				clickElement(target);
				state.loading = 1;
				clickElement("div#fightAction span.f div.c");
				step++;
			}
		}
	} while (res == 0);
	state.loading = 1;
	waitLoading();
}
