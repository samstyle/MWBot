#include "main.h"

void MWBotWin::atkRat() {
	QWebElement elm;
	setBusy(true);
// go to page
	loadPath(QStringList() << "square" << "metro");
// select dark/normal rats
	elm = frm->findFirstElement("div.metro-branch div.metro-rats-light__dark-block");
	if (!elm.isNull()) {
		log(trUtf8("Dark/Normal select"),"bug.png");
		if (opt.ratk.dark) {
			elm = frm->findFirstElement("a.f[href='/metro/select/1/']");
		} else {
			elm = frm->findFirstElement("a.f[href='/metro/select/0/']");
		}
		elm = elm.findFirst("div.c");
		if (elm.isNull()) {
			log(trUtf8("DEBUG: Rat selection error"));
			return;
		}
		clickElement(elm);
	}
// check timer & atk rat
	int time = getRatTimer();
	if (opt.ratk.ratlev > opt.ratk.maxlev) {
		elm = frm->findFirstElement("div#action-rat-fight div small small.dashedlink");
		if (elm.isNull()) {
			time = 60;
		} else {
			time = elm.attribute("timer").toInt() + 60;
		}
		opt.ratk.time = QDateTime::currentDateTime().addSecs(time);
		opt.ratk.ratlev = 1;
		log(trUtf8("Хватит крыс. Ждём обвала. До обвала <b>%0</b> мин.").arg(time/60 + 1),"rat.png");
	} else if (time < 1) {
		log(trUtf8("Уровень крысы: <b>%0</b>").arg(opt.ratk.ratlev),"rat.png");
		restoreHP();
		clickElement("div#action-rat-fight div.button-big div.c");
		clickElement("div#welcome-rat button.button div.c");
		if (opt.ratk.ratlev % 5 == 0) {
			groupFight();
		} else {
			fightResult();
		}
		getRatTimer();
	} else {
		time += 30 + random() % 30;
		opt.ratk.time = QDateTime::currentDateTime().addSecs(time);
		log(trUtf8("До следующей крысы <b>%0</b> мин.").arg(time/60 + 1),"rat.png");
	}
// take tails prize
	elm = frm->findFirstElement("div.metro-branch table.collectbar td.actions button.button");
	if (!elm.isNull()) {
		if (!elm.classes().contains("disabled")) {
			elm = elm.findFirst("div.c");
			clickElement(elm);
			log(trUtf8("Получен приз за крысинные хвосты"),"rattail.png");
		}
	}

	setBusy(false);
}
