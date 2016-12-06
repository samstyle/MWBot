#include "main.h"

void MWBotWin::trainPet() {
	QWebElement elm;
	setBusy(true);
	loadPath(QStringList() << "tverskaya" << "petrun" << "petarena");
	QList<QWebElement> lst = frm->findAllElements("dl#equipment-accordion").toList();
	QList<QWebElement> ls2;
	QWebElement el2;
	int fnd = 0;
	int idx = opt.bPet.num - 1;
	foreach(elm, lst) {
		el2 = elm.findFirst("dt.selected.active");
		if (el2.toPlainText().contains(trUtf8("Боевые питомцы"))) {
			ls2 = elm.findAll("div.object-thumb").toList();
			if (ls2.size() == 0) {
				log(trUtf8("Нет боевых питомцев, тренировка отключена"),"bone.png");
				opt.bPet.train = 0;
				setOpts();
			} else if (idx < ls2.size()) {
				elm = ls2[idx].findFirst("div#train-parrot.action");
				fnd = elm.isNull() ? 0 : 1;
			}
		}
		if (fnd) break;
	}
	if (!fnd) {
		log(trUtf8("Ошибка тренировки боевого питомца. +5 мин"),"bug.png");
		curTime = QDateTime::currentDateTime();
		opt.bPet.time = curTime.addSecs(300);
	} else {
		clickElement(elm);
		elm = frm->findFirstElement("span[rel=name]");
		QString name = elm.toPlainText();
		elm = frm->findFirstElement("div.cure table.process td.value span#restore");
		int time = elm.attribute("timer").toInt();
		if (time > 0) {
			time += 60 + random() % 30;
			curTime = QDateTime::currentDateTime();
			opt.bPet.time = curTime.addSecs(time);
			log(trUtf8("<b>%0</b> в коме, выйдет через %1 мин").arg(name).arg(time/60),"bone.png");
		} else {
			elm = frm->findFirstElement("span#trainpanel table.process td.value span#train");
			time = elm.attribute("timer").toInt();
			if (time > 0) {
				time += 60 + random() % 30;
				curTime = QDateTime::currentDateTime();
				opt.bPet.time = curTime.addSecs(time);
				log(trUtf8("До следующей прокачки <b>%0</b> %1 мин").arg(name).arg(time/60),"bone.png");
			} else {
				QWebElementCollection pets = frm->findAllElements("div.petarena-training-form ul.stats li.stat");
				int lev1 = pets.at(0).findFirst("div.label span.num").toPlainText().toInt();
				int lev2 = pets.at(1).findFirst("div.label span.num").toPlainText().toInt();
				int lev3 = pets.at(2).findFirst("div.label span.num").toPlainText().toInt();
				QString clk;
				if ((lev1 <= lev2) && (lev1 <= lev3)) {
					clk = "li.stat[rel=focus]";
				} else if (lev2 <= lev3) {
					clk = "li.stat[rel=loyality]";
				} else {
					clk = "li.stat[rel=mass]";
				}
				elm = frm->findFirstElement(clk);
				if (elm.isNull()) {
					log(trUtf8("Кнопка тренировки '%0' не найдена").arg(clk),"bug.png");
					opt.bPet.train = 0;
				} else {
					opt.bPet.money = elm.findFirst("div.text span[rel=cost] span.tugriki").toPlainText().remove("\"").remove(",").toInt();
					opt.bPet.ore = elm.findFirst("div.text span[rel=cost] span.ruda").toPlainText().remove("\"").remove(",").toInt();
					opt.bPet.oil = elm.findFirst("div.text span[rel=cost] span.neft").toPlainText().remove("\"").remove(",").toInt();
					// qDebug() << opt.bPet.money << opt.bPet.ore << opt.bPet.oil;
					getFastRes();
					if (info.money < opt.bPet.money + 200) {
						log(trUtf8("Не хватает денег на тренировки"),"bone.png");
						//opt.bPet.block = 1;
					} else if (opt.bPet.ore && !opt.bPet.useOre) {
						log(trUtf8("Тренировка за руду отключена"),"bone.png");
						opt.bPet.train = 0;
						setOpts();
					} else if (info.ore < opt.bPet.ore) {
						log(trUtf8("Не хватает руды на тренировки"),"bone.png");
					} else if (opt.bPet.oil && !opt.bPet.useOil) {
						log(trUtf8("Тренировка за нефть отключена"),"bone.png");
						opt.bPet.train = 0;
						setOpts();
					} else if (info.oil < opt.bPet.oil) {
						log(trUtf8("Не хватает нефти на тренировки"),"bone.png");
					} else {
						clk.append(" div.text button.button span.f div.c");
						clickElement(clk);
						curTime = QDateTime::currentDateTime();
						time = frm->findFirstElement("span#trainpanel table.process td.value span#train").attribute("timer").toInt() + 10;
						opt.bPet.time = curTime.addSecs(time);
						log(trUtf8("Тренировка <b>%0</b> запущена. До следующей %1 мин").arg(name).arg(time/60),"bone.png");
					}
				}
			}
		}
	}
	setBusy(false);
}
