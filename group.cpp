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

int getMyHp(QWebFrame* frm) {
	QWebElement elm = frm->findFirstElement("td.group li.me span.fighter_hp");
	QStringList lst = elm.toPlainText().replace("k","*1000").split("/");
	int hp = lst.first().toInt();
	int maxhp = lst.last().toInt();
	return (hp * 100 / maxhp);
}

int getSumHp(QWebFrame* frm, int ally) {
	QList<QWebElement> grps = frm->findAllElements("td.group").toList();
	QWebElement grp = ally ? grps.first() : grps.last();
	QList<QWebElement> flst = grp.findAll("li.alive").toList();
	QWebElement fgt;
	int res = 0;
	foreach(fgt, flst) {
		res += fgt.findFirst("span.life span.number span.fighter-hp").toPlainText().split("/").first().replace("k","*1000").toInt();
	}
	return res;
}

QString searchFightSlot(QWebFrame* frm, QStringList nList) {
	QString nam;
	QString res;
	QWebElement elm;
	res.clear();
	foreach(nam, nList) {
		nam = QString("div.fight-slots li.filled input[type=radio][rel=\"%0\"]").arg(nam);
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
	//int step = 1;
	QStringList cheeseList, healList;
	cheeseList << "Ароматный сыр" << "Шведский сыр" << "Итальянский сыр" << "Французский сыр";
	healList << "Хот-дог «Сасисыч»" << "Лапша";
	healList << "Пицца «Пепперони»" << "Ройалбургер «Сумоист»" << "Гамбургер «Обжорка»";
	healList << "Орех «Кокосовый»" << "Тыква «Колхозная»" << "Дуриан «Заморский»";
	healList << "Икорка на черством хлебушке" << "Икорка на хлебушке [Ультра]";
	healList << "Роллы «Огурцовые»" << "Роллы «Огурцовые» [Ультра]";
	healList << "Вкусный воккер" << "Вкусный воккер [Ультра]";
	int enemyHp = getSumHp(frm,0);
	int allyHp = getSumHp(frm,1);
	int useCheese = (enemyHp > allyHp*2) ? 1 : 0;
	int useHeal = 1;
	do {
		cnt = getGroupCount(frm,1);
		if (cnt == 0) {
			res = 2;		// lose
		} else {
			cnt = getGroupCount(frm,0);
			if (cnt == 0) {
				res = 1;	// win
			} else {
				target.clear();
				cnt = getMyHp(frm);
				if (useHeal && (cnt < 30)) {
					target = searchFightSlot(frm, healList);
					if (target.isEmpty()) {
						useHeal = 0;
					} else {
						target.prepend("div.fight-slots li.filled input#");
					}
				} else if (useCheese) {
					target = searchFightSlot(frm, cheeseList);
					if (!target.isEmpty()) {
						target.prepend("div.fight-slots li.filled input#");
					}
					useCheese = 0;
				} else {
					target = getWeakest(frm,0);
					target.prepend("input.radio-attack#");
					// step++;
				}
				if (!target.isEmpty()) {
					clickElement(target);
					state.loading = 1;
					clickElement("div#fightAction span.f div.c");
				}
			}
		}
	} while (res == 0);
	state.loading = 1;
	waitLoading();
}
