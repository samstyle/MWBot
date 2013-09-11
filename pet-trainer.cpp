#include "main.h"

void MWBotWin::trainPet() {
	QWebElement elm;
	int time;
	setBusy(true);
	loadPage("tverskaya");
	loadPage("petrun");
	loadPage("petarena");
	elm = frm->findFirstElement("dl#equipment-accordion");
	QWebElementCollection pets = elm.findAll("dd div.object-thumbs div.object-thumb");
	if (pets.count() == 0) {
		flag &= ~FL_TRAIN;
	} else {
		elm = pets.last();
		elm = elm.findFirst("div.padding div.action");
		loadPage(elm.attribute("onclick").split("'",QString::SkipEmptyParts).at(1));

		elm = frm->findFirstElement("div.cure table.process td.value span#restore");
		time = elm.attribute("timer").toInt();
		if (time > 0) {
			time += 20;
			curTime = QDateTime::currentDateTime();
			trnTime = curTime.addSecs(time);
			log(trUtf8("Питомец в коме, выйдет через ").append(QString::number(time/3600)).append(trUtf8(" часов")));
		} else {
			elm = frm->findFirstElement("span#trainpanel table.process td.value span#train");
			time = elm.attribute("timer").toInt();
			if (time > 0) {
				time += 10;
				curTime = QDateTime::currentDateTime();
				trnTime = curTime.addSecs(time);
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
				int tgr = elm.findFirst("div.text span[rel=cost] span.tugriki").toPlainText().toInt();
				int rud = elm.findFirst("div.text span[rel=cost] span.ruda").toPlainText().toInt();
				int nef = elm.findFirst("div.text span[rel=cost] span.neft").toPlainText().toInt();
				qDebug() << tgr << rud << nef;
				ResBox rbox = getResources();
				if (rbox.money < (tgr + 1000)) {
					log(trUtf8("не хватает денег на тренировки"));
					flag &= ~FL_TRAIN;
				} else if ((rbox.ruda < (rud + 6)) || (rud && (~opt & FL_TR_RUDA))) {
					log(trUtf8("не хватает руды на тренировки"));
					flag &= ~FL_TRAIN;
				} else if ((rbox.neft < (nef + 50)) || (nef && (~opt & FL_TR_OIL))) {
					log(trUtf8("не хватает нефти на тренировки"));
					flag &= ~FL_TRAIN;
				} else {
					clk.append(" span.f div.c");
					clickElement(clk,0);
					curTime = QDateTime::currentDateTime();
					time = frm->findFirstElement("span#trainpanel table.process td.value span#train").attribute("timer").toInt() + 10;
					trnTime = curTime.addSecs(time);
					log(trUtf8("Тренировка пета. До следующей прокачки пета ").append(QString::number(time/60)).append(trUtf8(" мин")));
				}
			}
		}
	}
	setBusy(false);
}
