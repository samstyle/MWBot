#include "main.h"

void MWBotWin::arena() {
	setBusy(true);
	loadPath(QStringList() << "tverskaya" << "petrun");
	curTime = QDateTime::currentDateTime();
	QWebElementCollection pets;
	QWebElement elm,pet;
// rewind record
	elm = frm->findFirstElement("div#forward-btn");
	if (!elm.isNull()) {
		clickElement("div#forward-btn");				// rewind run
		clickElement("div#result-block div.center div.button div.c");	// to arena
	}
// check tickets
	int prc = frm->findFirstElement("div.balance span#grayhound-tickets-num").toPlainText().toInt();
	if (prc == 0) {
		log(trUtf8("Билеты на забеги кончились. Продолжим завтра в 00:05"));
		runTime = curTime;
		runTime.setTime(QTime(0,5));		// 00:05 of tommorow
		runTime = runTime.addDays(1);
	} else {
// set 1st active pet
		elm = frm->findFirstElement("div.pet-slider ul.lenta");
		if (elm.isNull()) {
			runTime = runTime.addSecs(300);
		} else {
			int wtime = 20;
			pets = elm.findAll("li.pet-object[data-id]");
			foreach(pet,pets) {
				elm = pet.findFirst("span.percent");
				prc = elm.attribute("style").split(QRegExp("[:%;]"),QString::SkipEmptyParts).last().toInt();
				if (prc > 19) {
					clickElement(QString("ul.lenta li.pet-object[data-id='").append(pet.attribute("data-id")).append("']"));
					clickElement("div.center button#checkInEnabled div.c");
					elm = frm->findFirstElement("div.alert div#alert-text");
					if (elm.toPlainText().contains(trUtf8("нужен билет"))) {
						clickElement("div.alert div.button div.c");
						clickElement("input[value=ticket]");	// select ticket
						clickElement(QString("ul.lenta li.pet-object[data-id='").append(pet.attribute("data-id")).append("']"));
						clickElement("div.center button#checkInEnabled div.c");
					}
					log(trUtf8("%0 записан на забег").arg(pet.attribute("data-pet-name")));
					runTime = curTime.addSecs(600);		// +10 min
					wtime = 0;
					break;				// break foreach
				} else {
					if (wtime > (21 - prc)) wtime = (21 - prc) * 3;
				}
			}
			if (wtime != 0) {
				log(trUtf8("До забегов %0 мин").arg(wtime));
				runTime = curTime.addSecs(wtime * 60);		// 3min = 1%
			}
		}
	}
	setBusy(false);
}
