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
		clickElement("div#forward-btn",0);				// rewind run
		clickElement("div#result-block div.center div.button div.c",0);	// to arena
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
					log(pet.attribute("data-pet-name").append(trUtf8(" записан на забег")));
					clickElement(QString("ul.lenta li.pet-object[data-id='").append(pet.attribute("data-id")).append("']"),0);
					clickElement("div.center button#checkInEnabled div.c",0);
					runTime = curTime.addSecs(600);		// +10 min
					wtime = 0;
					break;				// break foreach
				} else {
					if (wtime > (21 - prc)) wtime = 21 - prc;
				}
			}
			if (wtime != 0) {
				log(trUtf8("До забегов ").append(QString::number(wtime)).append(trUtf8(" мин")));
				runTime = curTime.addSecs(wtime * 180);		// 3min = 1%
			}
		}
	}
	setBusy(false);
}
