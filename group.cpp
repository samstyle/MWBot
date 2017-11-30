#include "main.h"

QList<grpChar> grpGetGroup(QWebFrame*, QWebElement& grp) {
	QList<grpChar> list;
	grpChar cha;
	QWebElement echa = grp.findFirst("ul.list-users li");
	QWebElement etmp;
	QStringList tsl;
	while (eVisible(echa)) {
		cha.me = echa.classes().contains("me") ? 1 : 0;
		cha.alive = echa.classes().contains("alive") ? 1 : 0;
		cha.name = echa.findFirst("span.user").toPlainText();
		etmp = echa.findFirst("input.radio-attack");
		cha.id = etmp.attribute("value").toInt();
		etmp = echa.findFirst("span.life span.number span.fighter_hp");
		tsl = etmp.toPlainText().split("/");
		cha.hp = toLarge(tsl.first());
		cha.maxhp = toLarge(tsl.last());
		list.append(cha);
		echa = echa.nextSibling();
	}
	return list;
}

QList<grpChar> grpGetAllies(QWebFrame* frm) {
	QWebElementCollection grps = frm->findAllElements("table.data td.group");
	QWebElement grp = grps.first();
	return grpGetGroup(frm, grp);
}

QList<grpChar> grpGetEnemies(QWebFrame* frm) {
	QWebElementCollection grps = frm->findAllElements("table.data td.group");
	QWebElement grp = grps.last();
	return grpGetGroup(frm, grp);
}

int getGroupCount(QWebFrame* frm, int ally) {
	QList<grpChar> grp = ally ? grpGetAllies(frm) : grpGetEnemies(frm);
	int res = 0;
	foreach(grpChar cha, grp) {
		if (cha.alive)
			res++;
	}
	return res;
}

int getWeakest(QWebFrame* frm, int ally) {
	QList<grpChar> grp = ally ? grpGetAllies(frm) : grpGetEnemies(frm);
	int hp = 0;
	int id = 0;
	foreach(grpChar cha, grp) {
		if (cha.alive && cha.id && ((hp == 0) || (cha.hp < hp))) {
			id = cha.id;
			hp = cha.hp;
		}
	}
	return id;
}

int getMyHp(QWebFrame* frm) {
	QWebElement elm = frm->findFirstElement("td.group li.me span.fighter_hp");
	QStringList lst = elm.toPlainText().replace("k","e3").replace("M","e6").replace(",",".").split("/");
	int hp = lst.first().toDouble();
	int maxhp = lst.last().toDouble();
	return (hp * 100 / maxhp);
}

int getSumHp(QWebFrame* frm, int ally) {
	QList<grpChar> grp = ally ? grpGetAllies(frm) : grpGetEnemies(frm);
	int res = 0;
	foreach (grpChar cha, grp) {
		if (cha.alive)
			res += cha.hp;
	}
	return res;
}

QString searchFightSlot(QWebFrame* frm, QStringList nList) {
	QString nam;
	QString name;
	QString res;
	QWebElement elm;
	res.clear();
	foreach(name, nList) {
		nam = QString("div.fight-slots li.filled input[type=radio][rel=\"%0\"]").arg(name);
		elm = frm->findFirstElement(nam);
		if (!elm.isNull()) {
			res = elm.attribute("id");
			break;
		}
	}
	return res;
}

void MWBotWin::groupFight() {
	QString target;
	int res = 0;
	int cnt;
	int ecnt;
	int rnd;
	int id;
	int useCheese = 0;
	if (opt.group.cheese) {
		int enemyHp = getSumHp(frm,0);
		int allyHp = getSumHp(frm,1);
		useCheese = (enemyHp > allyHp*2) ? 1 : 0;
	}
	int useHeal = opt.group.heal ? 1 : 0;
	int useBomb = opt.group.bomb ? 1 : 0;
	do {
		cnt = getGroupCount(frm, 1);		// alive allies
		ecnt = getGroupCount(frm, 0);		// alive enemies
		if (cnt && ecnt) {			// both groups is active
			if (getMyHp(frm)) {		// player is alive
				target.clear();
				rnd = rand() % 101;	// 0..100
				cnt = getMyHp(frm);
				if (useHeal && (cnt < 30)) {
					target = searchFightSlot(frm, opt.group.healList);
					if (target.isEmpty()) {
						useHeal = 0;
					} else {
						target.prepend("div.fight-slots li.filled input#");
					}
				} else if (useCheese) {
					target = searchFightSlot(frm, opt.group.cheeseList);
					if (!target.isEmpty()) {
						target.prepend("div.fight-slots li.filled input#");
					}
					useCheese = 0;
				} else if (useBomb && (rnd < opt.group.bombPrc) && (ecnt > 1)) {
					target = searchFightSlot(frm, opt.group.bombList);
					if (target.isEmpty()) {
						useBomb = 0;
					} else {
						target.prepend("div.fight-slots li.filled input#");
					}
				} else {
					id = getWeakest(frm,0);
					target = QString("input.radio-attack#attack-%0").arg(id);
				}
				if (!target.isEmpty()) {
					qDebug() << target;
					click(ui.browser, target);
				}
				qDebug() << QTime::currentTime() << "click";
				click(ui.browser, "div#fightAction span.f div.c");
				qDebug() << QTime::currentTime();
			} else {
				waitReload(ui.browser);	// player is dead: wait for reload
			}
		} else if (ecnt) {		// alive enemies
			res = 2;		// lose
		} else if (cnt) {		// alive allies
			res = 1;		// win
		} else {			// both is dead
			res = 3;		// draw
		}
	} while (res == 0);
	waitReload(ui.browser);
	FightBox fb = getGroupResult();
	logResult(fb);
}
