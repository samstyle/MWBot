#include "main.h"

QString MWBotWin::getWeakest(int ally) {
	QString res;
	res.clear();
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
	int work = 1;
	do {
		target = getWeakest(0);
		if (target.isEmpty()) {
			work = 0;
		} else {
			clickElement(QString("input.radio-attack#%0").arg(target));
			state.loading = 1;		// wait page reload
			clickElement("div#fightAction span.f div.c");
			waitLoading();
		}
	} while (work);
	// todo:wait "fight end" timer
	// push "back to..." button
}
