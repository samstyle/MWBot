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
	QString hps;
	int minhp = 0;
	int hp;
	foreach(fgt, flst) {
		hps = fgt.findFirst("span.life span.number span.fighter_hp").toPlainText();
		hp = hps.split("/").first().replace("k","e3").replace(",",".").toDouble();
		if ((minhp == 0) || (hp < minhp)) {
			minhp = hp;
			res = fgt.findFirst("label[for]").attribute("for");
		}
	}
	return res;
}

int getMyHp(QWebFrame* frm) {
	QWebElement elm = frm->findFirstElement("td.group li.me span.fighter_hp");
	QStringList lst = elm.toPlainText().replace("k","e3").replace(",",".").split("/");
	int hp = lst.first().toDouble();
	int maxhp = lst.last().toDouble();
	return (hp * 100 / maxhp);
}

int getSumHp(QWebFrame* frm, int ally) {
	QList<QWebElement> grps = frm->findAllElements("td.group").toList();
	QWebElement grp = ally ? grps.first() : grps.last();
	QList<QWebElement> flst = grp.findAll("li.alive").toList();
	QWebElement fgt;
	QStringList lst;
	int res = 0;
	foreach(fgt, flst) {
		lst = fgt.findFirst("span.fighter_hp").toPlainText().replace("k","e3").replace(",",".").split("/");
		res += lst.first().toDouble();
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
	//int step = 1;
	QStringList cheeseList, healList;
	cheeseList << trUtf8("Ароматный сыр");
	cheeseList << trUtf8("Шведский сыр");
	cheeseList << trUtf8("Итальянский сыр");
	cheeseList << trUtf8("Французский сыр");
	cheeseList << trUtf8("Запрещенный сыр");
	healList << trUtf8("Хот-дог «Сасисыч»");
	healList << trUtf8("Лапша");
	healList << trUtf8("Пицца «Пепперони»");
	healList << trUtf8("Ройалбургер «Сумоист»");
	healList << trUtf8("Гамбургер «Обжорка»");
	healList << trUtf8("Орех «Кокосовый»");
	healList << trUtf8("Тыква «Колхозная»");
	healList << trUtf8("Дуриан «Заморский»");
	healList << trUtf8("Икорка на черством хлебушке");
	healList << trUtf8("Икорка на хлебушке [Ультра]");
	healList << trUtf8("Роллы «Огурцовые»");
	healList << trUtf8("Роллы «Огурцовые» [Ультра]");
	healList << trUtf8("Вкусный воккер");
	healList << trUtf8("Вкусный воккер [Ультра]");
	int useCheese = 0;
	if (opt.group.cheese) {
		int enemyHp = getSumHp(frm,0);
		int allyHp = getSumHp(frm,1);
		useCheese = (enemyHp > allyHp*2) ? 1 : 0;
	}
	int useHeal = opt.group.heal ? 1 : 0;
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
				}
				state.loading = 1;
				clickElement("div#fightAction span.f div.c");
			}
		}
	} while (res == 0);
	state.loading = 1;
	waitLoading();
}

// debug

void MWBotWin::debug() {
	//checkPolice();
	clickElement("a.square");
}
