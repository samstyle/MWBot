#include "main.h"

void MWBotWin::trainPet() {
	QWebElement elm;
//	int time;
	setBusy(true);
	loadPath(QStringList() << "tverskaya" << "petrun" << "petarena");
	QList<QWebElement> lst = frm->findAllElements("dl#equipment-accordion").toList();
	QList<QWebElement> ls2;
	QWebElement el2;
	int fnd = 0;
	foreach(elm, lst) {
		el2 = elm.findFirst("dt.selected.active");
		if (el2.toPlainText().contains(trUtf8("Боевые питомцы"))) {
			ls2 = elm.findAll("div.object-thumb").toList();
			if (ls2.size() > 0) {
				elm = ls2.last();
				elm = elm.findFirst("div#train-parrot.action");
				fnd = 1;
			}
		}
		if (fnd) break;
	}
	if (!fnd) {
		log(trUtf8("Ошибка тренировки боевого питомца"));
		opt.bPet.train = 0;
		saveOpts();
	} else {
		clickElement(elm);
		elm = frm->findFirstElement("div.cure table.process td.value span#restore");
		int time = elm.attribute("timer").toInt();
		if (time > 0) {
			time += 20;
			curTime = QDateTime::currentDateTime();
			opt.bPet.time = curTime.addSecs(time);
			log(trUtf8("Питомец в коме, выйдет через ").append(QString::number(time/60)).append(trUtf8(" мин")));
		} else {
			elm = frm->findFirstElement("span#trainpanel table.process td.value span#train");
			time = elm.attribute("timer").toInt();
			if (time > 0) {
				time += 10;
				curTime = QDateTime::currentDateTime();
				opt.bPet.time = curTime.addSecs(time);
				log(trUtf8("До следующей прокачки пета ").append(QString::number(time/60)).append(trUtf8(" мин")));
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
				opt.bPet.money = elm.findFirst("div.text span[rel=cost] span.tugriki").toPlainText().remove("\"").remove(",").toInt();
				opt.bPet.ore = elm.findFirst("div.text span[rel=cost] span.ruda").toPlainText().remove("\"").remove(",").toInt();
				opt.bPet.oil = elm.findFirst("div.text span[rel=cost] span.neft").toPlainText().remove("\"").remove(",").toInt();
				qDebug() << opt.bPet.money << opt.bPet.ore << opt.bPet.oil;
				getFastRes();
				if (info.money < opt.bPet.money + 200) {
					log(trUtf8("не хватает денег на тренировки"));
					//opt.bPet.block = 1;
				} else if ((info.ore < opt.bPet.ore) || (opt.bPet.ore && !opt.bPet.useOre)) {
					log(trUtf8("не хватает руды на тренировки"));
					//opt.bPet.block = 1;
				} else if ((info.oil < opt.bPet.oil) || (opt.bPet.oil && !opt.bPet.useOil)) {
					log(trUtf8("не хватает нефти на тренировки"));
					//opt.bPet.block = 1;
				} else {
					clk.append(" span.f div.c");
					clickElement(clk);
					curTime = QDateTime::currentDateTime();
					time = frm->findFirstElement("span#trainpanel table.process td.value span#train").attribute("timer").toInt() + 10;
					opt.bPet.time = curTime.addSecs(time);
					log(trUtf8("Тренировка пета. До следующей прокачки пета %0 мин").arg(QString::number(time/60)));
				}
			}
		}
	}
	setBusy(false);
}

/*

#if 0
	loadPage("tverskaya");
	loadPage("petrun");
	loadPage("petarena");
	elm = frm->findFirstElement("dl#equipment-accordion");
#else
	loadPage("player");
	elm = frm->findFirstElement("dl#pet-accordion");
#endif
	QWebElementCollection pets = elm.findAll("dd div.object-thumbs div.object-thumb");
	if (pets.count() == 0) {
		opt.bPet.block = 1;
	} else {
		elm = pets.last();
		QWebElement elmt = elm.findFirst("div.padding img");
		if (elmt.attribute("data-mf").isEmpty()) {
			opt.bPet.block = 1;
			log(trUtf8("Не обнаружено боевых питомцев"));
		} else {
			elm = elm.findFirst("div.padding div.action");
			loadPage(elm.attribute("onclick").split("'",QString::SkipEmptyParts).at(1));

			elm = frm->findFirstElement("div.cure table.process td.value span#restore");
			time = elm.attribute("timer").toInt();
			if (time > 0) {
				time += 20;
				curTime = QDateTime::currentDateTime();
				opt.bPet.time = curTime.addSecs(time);
				log(trUtf8("Питомец в коме, выйдет через ").append(QString::number(time/60)).append(trUtf8(" мин")));
			} else {
				elm = frm->findFirstElement("span#trainpanel table.process td.value span#train");
				time = elm.attribute("timer").toInt();
				if (time > 0) {
					time += 10;
					curTime = QDateTime::currentDateTime();
					opt.bPet.time = curTime.addSecs(time);
					log(trUtf8("До следующей прокачки пета ").append(QString::number(time/60)).append(trUtf8(" мин")));
				} else {

					pets = frm->findAllElements("div.petarena-training-form ul.stats li.stat");
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
					opt.bPet.money = elm.findFirst("div.text span[rel=cost] span.tugriki").toPlainText().toInt();
					opt.bPet.ore = elm.findFirst("div.text span[rel=cost] span.ruda").toPlainText().toInt();
					opt.bPet.oil = elm.findFirst("div.text span[rel=cost] span.neft").toPlainText().toInt();
					// qDebug() << tgr << rud << nef;
					getFastRes();
					if (info.money < opt.bPet.money + 200) {
						log(trUtf8("не хватает денег на тренировки"));
						opt.bPet.block = 1;
					} else if ((info.ore < opt.bPet.ore) || (opt.bPet.ore && !opt.bPet.useOre)) {
						log(trUtf8("не хватает руды на тренировки"));
						opt.bPet.block = 1;
					} else if ((info.oil < opt.bPet.oil) || (opt.bPet.oil && !opt.bPet.useOil)) {
						log(trUtf8("не хватает нефти на тренировки"));
						opt.bPet.block = 1;
					} else {
						clk.append(" span.f div.c");
						clickElement(clk);
						curTime = QDateTime::currentDateTime();
						time = frm->findFirstElement("span#trainpanel table.process td.value span#train").attribute("timer").toInt() + 10;
						opt.bPet.time = curTime.addSecs(time);
						log(trUtf8("Тренировка пета. До следующей прокачки пета %0 мин").arg(QString::number(time/60)));
					}
				}
			}
		}
	}
	setBusy(false);
}
*/
