#include "main.h"

void MWBotWin::goMonia() {
	setBusy(true);
	QWebElement elm;
	getFastRes();
	if (info.money < 1500) {
		log(trUtf8("Для игры с Моней недостаточно денег (%0)").arg(info.money),"monya.png");
		return;
	}
	if (frm->url().toString().contains("/thimble/")) {	// already there
		playMonia();
	} else {
		loadPage("player");			// check resources
		int tickets = getItem(446);		// monya tickets
		int teeths = getItem(186);		// white teeths
		int stars = getItem(350);		// stars
		log(trUtf8("Играем с Моней"),"monya.png");
		loadPath("square:metro");
		elm = frm->findFirstElement("div.metro-thimble p.holders");
		int tries = elm.toPlainText().split(":").last().trimmed().toInt();
		if (tries == 0) {
			if (!opt.monya.tickets) {
				log(trUtf8("На сегодня игр с Моней больше нет"),"monya.png");
				opt.monya.time = curTime.addDays(1);
				opt.monya.time.setTime(QTime(0, 0, 5));
			} else if (tickets > 0) {
				loadPage("thimble/start");
				playMonia();
			} else if (!opt.monya.buy) {
				log(trUtf8("Билеты к Моне кончились, покупка отключена"),"monya.png");
				opt.monya.time = curTime.addDays(1);
				opt.monya.time.setTime(QTime(0, 0, 5));
			} else if (opt.monya.stars) {
				if (stars == 0) {
					log(trUtf8("Не хватает звёзд для покупки билета к Моне"),"monya.png");
					opt.monya.time = curTime.addDays(1);
					opt.monya.time.setTime(QTime(0, 0, 5));
				} else {
					loadPath("berezka:berezka/section/mixed");
					frm->findFirstElement("li[rel='448'] input#amount_monya_ticket").setAttribute("value","1");
					click(ui.browser, "div#monya-ticket[data-button-for-id='448'] div.c");
					loadPath("metro:thimble/start");
					playMonia();
				}
			} else {
				if (teeths == 0) {
					log(trUtf8("Не хватает зубов для покупки билета к Моне"),"monya.png");
					opt.monya.time = curTime.addDays(1);
					opt.monya.time.setTime(QTime(0, 0, 5));
				} else {
					frm->findFirstElement("li[rel='446'] input#amount_monya_ticket").setAttribute("value","1");
					click(ui.browser, "div#monya_ticket[data-button-for-id='446'] div.c");
					loadPath("metro:thimble/start");
					playMonia();
				}
			}
		} else {
			loadPage("thimble/start");
			playMonia();
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
			click(ui.browser, "div.button.thimble-play[data-count='9']");
			games++;
		}
		do {
			do {
				thimble = QString("i#thimble").append(QString::number(rand() % 9));
				elm = frm->findFirstElement(thimble);
			} while (elm.attribute("class") != "icon thimble-closed-active");
			click(ui.browser, thimble, 0.5);
			elm = frm->findFirstElement("div#thimble-controls");
		} while(elm.attribute("style").contains("none"));
		getFastRes();
	}
	loadPage("thimble/leave");
	log(trUtf8("Сыграно игр: %0. получено руды: %1").arg(games).arg(info.ore - oldruda),"monya.png");
}

void MWBotWin::goBankChange() {
	setBusy(true);
	int tickets = getItem(447);		// bank ore tickets
	int teeths = getItem(186);		// white teeths
	int stars = getItem(350);		// stars
	if (tickets == 0) {
		if (opt.bank.buy) {
			if (opt.bank.stars) {
				if (stars > 3) {
					loadPath("arbat:berezka:berezka/section/mixed/");
					click(ui.browser, "li[rel='447'] div.button#ore_ticket div.c");
					log(trUtf8("Рудный билет куплен за звезды"));
					tickets++;
				} else {
					log(trUtf8("Рудных билетов нет, звёзд не хватает."));
					opt.bank.time = curTime.addDays(1);
				}
			} else {
				if (teeths > 3) {
					loadPath("arbat:berezka:berezka/section/mixed/");
					click(ui.browser, "li[rel='449'] div.button#ore_ticket div.c");
					log(trUtf8("Рудный билет куплен за зубы"));
					tickets++;
				} else {
					log(trUtf8("Рудных билетов нет, звёзд не хватает."));
					opt.bank.time = curTime.addDays(1);
				}
			}
		} else {
			log(trUtf8("Рудных билетов нет, а покупка отключена."));
			opt.bank.time = curTime.addDays(1);
		}

	}
	if (tickets > 0) {
		loadPath("arbat:bank");
		click(ui.browser, "div.bank-robbery button.button div.c");
		log(trUtf8("Деньги обменены в банке на руду"));
	}
	setBusy(false);
}
