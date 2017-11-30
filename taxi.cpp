#include "main.h"

void MWBotWin::doTaxi() {
	if (curTime.date().dayOfWeek() != 1) return;
	if (opt.taxi.time > curTime) return;
	QWebElement elm;
	QWebElement sub;
	int time;
	int count;
	int loop;
	setBusy(true);
	loadPage("arbat");
// check taxi chest and take it
	elm = frm->findFirstElement("div.auto-bombila table.collectbar td.actions button.button");
	if (elm.isNull()) {
		count = 0;
	} else {
		count = elm.classes().contains("disabled") ? 0 : 1;
	}
	if (count) {
		sub = elm.findFirst("div.c");
		click(ui.browser, sub);
		log(trUtf8("Забран сундук бомбилы"),"chest.png");
	}
// check taxi timer, send a car & charge it if it needs
	elm = frm->findFirstElement("div.auto-bombila table.process td#cooldown");
	if (elm.styleProperty("display",QWebElement::ComputedStyle) == "none") {
		time = 0;
	} else {
		time = elm.attribute("timer").toInt();
	}
	if (time != 0) {
		time += (30 + (random() % 30));
		opt.taxi.time = curTime.addSecs(time);
		log(trUtf8("До следующей бомбилки %0 сек").arg(time),"taxi.png");
	} else {
		do {
			loop = 0;
			elm= frm->findFirstElement("div.auto-bombila table.action button.ride-button span.f div.c");
			click(ui.browser, elm);
			elm = frm->findFirstElement("div.alert.alert-error h2#alert-title");
			if (elm.isNull()) {
				elm = frm->findFirstElement("div.auto-bombila table.process td#cooldown");
				time = elm.attribute("timer").toInt();
				time += 30 + random() % 30;
				opt.taxi.time = curTime.addSecs(time);
				log(trUtf8("Авто отправлено бомбить. До следующего раза %0 сек").arg(time),"taxi.png");
			} else {
				elm = frm->findFirstElement("div.alert.alert-error div.actions div.button div.c");
				sub = elm.findFirst("span");
				count = sub.toPlainText().remove("\"").toInt();
				getFastRes();
				if (sub.classes().contains("ruda")) {
					count = (info.ore >= count) ? 1 : 0;
				} else if (sub.classes().contains("oil")) {
					count = (info.oil >= count) ? 1 : 0;
				} else {
					count = 0;
				}
				if (count) {
					click(ui.browser, elm);
					log(trUtf8("Машина заправлена"),"taxi.png");
					loop = 1;
				} else {
					time = 15 * 60;
					opt.taxi.time = curTime.addSecs(time);
					log(trUtf8("Не хватает ресурсов для заправки. Ждём 15 мин"),"taxi.png");
				}
			}
		} while (loop);
	}
	setBusy(false);
}
