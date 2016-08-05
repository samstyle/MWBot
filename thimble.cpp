#include "main.h"

void MWBotWin::goMonia() {
	setBusy(true);
	QWebElement elm;
	getFastRes();
	if (info.money < 1500) {
		log(trUtf8("Для игры с Моней недостаточно денег (%0)").arg(info.money),"monya.png");
		return;
	}
	if (!frm->url().toString().contains("/thimble/")) {
		log(trUtf8("Играем с Моней"),"monya.png");
		loadPath(QStringList() << "square" << "metro");
		elm = frm->findFirstElement("div.metro-thimble p.holders");
		if ((elm.toPlainText().split(":").last().trimmed().toInt() == 0) && !opt.monya.tickets) {
			log(trUtf8("Игра с Моней за билеты отключена"),"monya.png");
			opt.monya.block = 1;
			setBusy(false);
			return;
		}
		loadPage("thimble/start/");
	}
	elm = frm->findFirstElement("div#alert-text a[href='/berezka/']");
	if (elm.isNull()) {
		playMonia();
	} else {
		if (opt.monya.buy) {
			loadPage("berezka");
			loadPage("berezka/section/mixed");
			getBerezkaRes();
			if (opt.monya.stars) {
				if (info.star > 0) {
					frm->findFirstElement("li[rel='448'] input#amount_monya_ticket").setAttribute("value","1");
					clickElement("div#monya-ticket[data-button-for-id='448'] div.c");
					loadPage("thimble/start/");
					playMonia();
				} else {
					log(trUtf8("Нет звезд, чтобы купить билет к Моне"),"monya.png");
					opt.monya.block = 1;
				}
			} else {
				if (info.tooth > 0) {
					frm->findFirstElement("li[rel='448'] input#amount_monya_ticket").setAttribute("value","1");
					clickElement("div#monya_ticket[data-button-for-id='446'] div.c");
					loadPage("thimble/start/");
					playMonia();
				} else {
					log(trUtf8("Нет зубов, чтобы купить билет к Моне"),"monya.png");
					opt.monya.block = 1;
				}
			}
		} else {
			log(trUtf8("Билеты кончились, игры с Моней тоже"),"monya.png");
			opt.monya.block = 1;
		}
	}
	setBusy(false);
}

void MWBotWin::playMonia() {
	QString thimble;
	getFastRes();
	QWebElement elm;
	int oldruda = info.ore;
	int games = 0;
	while (info.money > 2100) {
		elm = frm->findFirstElement("div#thimble-controls");
		if (!elm.attribute("style").contains("none")) {
			elm = frm->findFirstElement("div.button.thimble-play[data-count='9']");
			clickElement("div.button.thimble-play[data-count='9']");
			games++;
		}
		do {
			do {
				thimble = QString("i#thimble").append(QString::number(rand() % 9));
				elm = frm->findFirstElement(thimble);
			} while (elm.attribute("class") != "icon thimble-closed-active");
			clickElement(thimble, 500);
			elm = frm->findFirstElement("div#thimble-controls");
		} while(elm.attribute("style").contains("none"));
		getFastRes();
	}
	loadPage("thimble/leave");
	log(trUtf8("Сыграно игр: %0. получено руды: %1").arg(games).arg(info.ore - oldruda),"monya.png");
}

void MWBotWin::goBankChange() {
	setBusy(true);
	getResources();
	int work = 1;
	if (info.resMap[447] == 0) {
		loadPath(QStringList() << "arbat" << "berezka" << "berezka/section/mixed/");
		getBerezkaRes();
		if ((info.star > info.tooth) && (info.star >= 4)) {
			clickElement("li[rel='447'] div.button#ore_ticket div.c");
			log(trUtf8("Рудный билет куплен за звезды"));
		} else if ((info.tooth >= 4)) {
			clickElement("li[rel='449'] div.button#ore_ticket div.c");
			log(trUtf8("Рудный билет куплен за зубы"));
		} else {
			work = 0;
		}
	}
	if (work) {
		loadPath(QStringList() << "arbat" << "bank");
		clickElement("div.bank-robbery button.button div.c");
		log(trUtf8("Деньги обменены в банке на руду"));
	}
	setBusy(false);
}
