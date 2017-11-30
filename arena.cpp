#include "main.h"

void MWBotWin::arena() {
	setBusy(true);
	loadPath("tverskaya:petrun");
	curTime = QDateTime::currentDateTime();
	QWebElementCollection pets;
	QWebElement elm,pet;
	QString name;
	int wtime;
// rewind record
	elm = frm->findFirstElement("div#forward-btn");
	if (!elm.isNull()) {
		click(ui.browser, "div#forward-btn");				// rewind run
		click(ui.browser, "div#result-block div.center div.button div.c");	// to arena
	}
// check tickets
	int prc = frm->findFirstElement("div.balance span#grayhound-tickets-num").toPlainText().toInt();
	if (prc == 0) {
		log(trUtf8("Билеты на забеги кончились. Продолжим завтра в 00:05"),"medal.png");
		opt.run.time = curTime;
		opt.run.time.setTime(QTime(0,5));		// 00:05 of tommorow
		opt.run.time = opt.run.time.addDays(1);
	} else {
// set 1st active pet
		elm = frm->findFirstElement("div.pet-slider ul.lenta");
		if (elm.isNull()) {
			opt.run.time = opt.run.time.addSecs(300);
		} else {
			wtime = 20;
			pets = elm.findAll("li.pet-object[data-id]");
			//qDebug() << pets.toList().size();
			foreach(pet, pets) {
				name = pet.attribute("data-pet-name");
				//qDebug() << name;
				if (opt.run.name.isNull() || (name.toLower().contains(opt.run.name.toLower()))) {
					if (pet.attribute("selected").isNull())
						click(ui.browser, pet);
					elm = pet.findFirst("span.percent");
					prc = elm.attribute("style").split(QRegExp("[:%;]"),QString::SkipEmptyParts).last().toInt();
					if (prc > 19) {
						click(ui.browser, QString("ul.lenta li.pet-object[data-id='%0']").arg(pet.attribute("data-id")));
						click(ui.browser, "div.center button#checkInEnabled div.c");
						elm = frm->findFirstElement("div.alert div#alert-text");
						if (elm.toPlainText().contains(trUtf8("нужен билет"))) {
							click(ui.browser, "div.alert div.button div.c");
							click(ui.browser, "input[value=ticket]");	// select ticket
							click(ui.browser, QString("ul.lenta li.pet-object[data-id='%0']").arg(pet.attribute("data-id")));
							click(ui.browser, "div.center button#checkInEnabled div.c");
						}
						log(trUtf8("<b>%0</b> записан на забег").arg(pet.attribute("data-pet-name")),"medal.png");
						opt.run.time = curTime.addSecs(600);		// +10 min
						wtime = 0;
					} else {
						if (wtime > (21 - prc)) wtime = (21 - prc) * 3;
					}
				}
			}
			if (wtime != 0) {
				log(trUtf8("До забегов %0 мин").arg(wtime),"medal.png");
				opt.run.time = curTime.addSecs(wtime * 60);		// 3min = 1%
			}
		}
	}
	setBusy(false);
}
