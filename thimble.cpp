#include "main.h"

void MWBotWin::goMonia() {
	setBusy(true);
	QWebElement elm;
	ResBox rbox = getResources();
	if (rbox.money < 1500) {
		log(trUtf8("Для игры с Моней недостаточно денег (").append(QString::number(rbox.money).append(")")));
		return;
	}
	if (!frm->url().toString().contains("/thimble/")) {
		log(trUtf8("Играем с Моней"));
		loadPage("square/");
		loadPage("metro/");
		elm = frm->findFirstElement("div.metro-thimble p.holders");
		if ((elm.toPlainText().split(":").last().trimmed().toInt() == 0) && (~opt & FL_MONIA_BILET)) {
			log(trUtf8("Игра с Моней за билеты отключена"));
			flag &= ~FL_MONIA;
			setBusy(false);
			return;
		}
		loadPage("thimble/start/");
	}
	elm = frm->findFirstElement("div#alert-text a[href='/berezka/']");
	if (elm.isNull()) {
		playMonia();
	} else {
		if (opt & FL_MONIA_BUY) {
			loadPage("/berezka/");
			rbox = getBerezkaRes();
			if (opt & FL_MONIA_STAR) {
				if (rbox.star > 0) {
					clickElement("div#monya-ticket[data-button-for-id='448'] div.c",0);
					loadPage("thimble/start/");
					playMonia();
				} else {
					log(trUtf8("Нет звезд, чтобы купить билет к Моне"));
					flag &= ~FL_MONIA;
				}
			} else {
				if (rbox.wtooth > 0) {
					clickElement("div#monya_ticket[data-button-for-id='446'] div.c",0);
					loadPage("thimble/start/");
					playMonia();
				} else {
					log(trUtf8("Нет зубов, чтобы купить билет к Моне"));
					flag &= ~FL_MONIA;
				}
			}
		} else {
			log(trUtf8("Билеты кончились, игры с Моней тоже"));
			flag &= ~FL_MONIA;
		}
	}
	setBusy(false);
}

void MWBotWin::playMonia() {
	QString thimble;
	ResBox rbox = getResources();
	QWebElement elm;
	int oldruda = rbox.ruda;
	int games = 0;
	while (rbox.money > 2100) {
		elm = frm->findFirstElement("div#thimble-controls");
		if (!elm.attribute("style").contains("none")) {
			elm = frm->findFirstElement("div.button.thimble-play[data-count='9']");
			clickElement("div.button.thimble-play[data-count='9']",0);
			games++;
		}
		do {
			do {
				thimble = QString("i#thimble").append(QString::number(rand() % 9));
				elm = frm->findFirstElement(thimble);
			} while (elm.attribute("class") != "icon thimble-closed-active");
			clickElement(thimble,200);
			elm = frm->findFirstElement("div#thimble-controls");
		} while(elm.attribute("style").contains("none"));
		rbox = getResources();
	}
	loadPage("thimble/leave");
	log(trUtf8("За ").append(QString::number(games).append(trUtf8(" игр получено ").append(QString::number(rbox.ruda - oldruda).append(trUtf8(" руды"))))));
}
