#include "main.h"

int checkName(QString chkname, QStringList list) {
	QStringList white;
	QStringList black;
	QString name;
	foreach(name, list) {		// filling blacklist, whitelist
		if (name.startsWith("!")) {
			black.append(name.mid(1));
		} else {
			white.append(name);
		}
	}
	foreach (name, black) {
		if (chkname.contains(name, Qt::CaseInsensitive)) {
			return 0;
		}
	}
	if (white.isEmpty()) return 1;
	foreach (name, white) {
		if (chkname.contains(name, Qt::CaseInsensitive)) {
			return 1;
		}
	}
	return 0;
}

struct intPair {
	int rideCooldown;
	int carCooldown;
};

intPair getCooldown(QWebElement& rid) {
	intPair res;
	QWebElement elt = rid.findFirst("div.picture span.timeout span.ride-cooldown");
	res.rideCooldown = eVisible(elt) ? elt.attribute("timer").toInt() : 0;		// ride cooldown
	elt = rid.findFirst("div.require div.car div.car-place div.timeout span.car-cooldown");
	res.carCooldown = eVisible(elt) ? elt.attribute("timer").toInt() : 0;			// car cooldown
	return res;
}

int MWBotWin::checkCooldown(QWebElement& elm, QString) {
	intPair cDown = getCooldown(elm);
	QDateTime timer;
	int time = (cDown.rideCooldown > cDown.carCooldown) ? cDown.rideCooldown : cDown.carCooldown;	// max
	if (time) {
//		if (time == cDown.rideCooldown) {
//			log(trUtf8("Поездка '<b>%0</b>' будет доступна через %1:%2 ч").arg(name).arg(time/3600).arg((time/60)%60));
//		} else {
//			log(trUtf8("Машина для поездки <b>%0</b> будет готова через %1:%2 ч").arg(name).arg(time/3600).arg((time/60)%60));
//		}
		curTime = QDateTime::currentDateTime();
		timer = curTime.addSecs(time + 120 + random() % 60);
		if ((opt.car.time < curTime) || (opt.car.time > timer))
			opt.car.time = timer;
	}
	return time;
}

void MWBotWin::rideCar() {
	setBusy(true);
	loadPage("home");
	QWebElement elm = frm->findFirstElement("div#home-garage.object-thumbs");
	QWebElement el2, el3;
	QWebElement rid;
	QString name;
	QString line;
	FightBox res;
	mwItem item;
	int rpt;
	int num = elm.findAll("div.object-thumb").count();
	curTime = QDateTime::currentDateTime();
	if (num == 0) {
		log(trUtf8("Нет машин в гараже. Поездки отключены."),"ride.png");
		opt.car.ride = 0;
	} else {
		elm = frm->findFirstElement("div.home-garage dd div.about button.button span.f div.c");
		click(ui.browser, elm);		// click 'go ride' button

		do {
			rpt = 0;
			elm = frm->findFirstElement("div.cars-trip div.cars-trip-choose div.cars-trip-accordion ul");	// ride list part
			foreach(rid, elm.findAll("li[id^=direction]")) {						// for each ride in list
				if (rid.isNull()) qDebug() << "NULL";
				if (rid.findFirst("i.icon-locked").isNull()) {						// check ride isn't locked
					name = rid.findFirst("table.title h3").toPlainText();				// ride name
					if (checkName(name, opt.car.list) && (checkCooldown(rid, name) == 0)) {
						rpt = 0;
						el2 = rid.findFirst("div.actions button.button span.f div.c");	// click 'go ride'
						click(ui.browser, el2);
						el2 = frm->findFirstElement("div.alert.infoalert");		// alert window
						if (eVisible(el2)) {
							el3 = el2.findFirst("div.padding div.data div#alert-text");
							line = el3.toPlainText();					// alert text
							if (line.contains(trUtf8("пустым баком"),Qt::CaseInsensitive)) {
								foreach(el3, el2.findAll("div.padding div.data div.actions div.button")) {	// push refill button
									if (el3.toPlainText().contains(trUtf8("заправить"),Qt::CaseInsensitive)) {
										el3 = el3.findFirst("span.f div.c");
										// qDebug() << "refill";
										click(ui.browser, el3);
										// ui.browser->reload();
										log(trUtf8("Машина заправлена"),"ride.png");
										rpt = 1;
									}
									if (rpt) break;
								}
							} else if (line.contains(trUtf8("следующая поездка"),Qt::CaseInsensitive)) {
								log(trUtf8("Машина отправлена в поездку <b>%0</b>").arg(name));
								el3 = el2.findFirst("div.auto-trip-result");
								line = el3.findFirst("p.bonus").toInnerXml().replace("<br>",", ").trimmed();			// bonus stats
								log(line,"ride.png");
								foreach(el3, el2.findAll("div#alert-text span.object-thumb")) {					// collect bonus items
									item.name = el3.findFirst("div.padding img").attribute("src").split("/").last();
									item.count = el3.findFirst("div.padding span.count").toPlainText().remove("#").toInt();
									res.items.append(item);
								}
								res.result = resBonus;
								logResult(res);
								click(ui.browser, "div.alert.infoalert div.data div.actions div.button span.f div.c");		// close alert window
								rpt = 1;
							} else {
								qDebug() << "some alert";
							}
						}
					}
				}
			}
		} while (rpt);
	}
	setBusy(false);
}
