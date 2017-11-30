#include "main.h"

void MWBotWin::playKub() {
	setBusy(true);
	loadPath("arbat:casino");
	QWebElement elm;
	elm = frm->findFirstElement("div.balance span.fishki span#fishki-balance-num");
	info.caps = elm.toPlainText().remove(",").toInt();
	if ((info.caps < opt.kub.caps) && opt.kub.buy) {
		int tobuy = opt.kub.caps - info.caps;
		int needore = 1 + tobuy/10;
		getFastRes();
		if (info.ore < needore) {
			needore = info.ore;
		}
		elm = frm->findFirstElement("input#stash-change-ore");
		elm.setAttribute("value",QString::number(needore));
		click(ui.browser, "button#button-change-ore div.c");
	}
	loadPage("casino/kubovich");
	int rcaps = 0;
	int price;
	do {
		elm = frm->findFirstElement("div.balance span.fishki span#fishki-balance-num");
		info.caps = elm.toPlainText().remove(",").toInt();
		elm = frm->findFirstElement("button#push-ellow");
		if (!elm.classes().contains("disabled")) {
			click(ui.browser, "button#push-ellow div.c");
			click(ui.browser, "button#push div.c");
			ui.browser->reload();
			waitLoading(ui.browser);
		}
		elm = frm->findFirstElement("div.controls button#push");
		price = elm.findFirst("span.price").toPlainText().remove("\"").toInt();
		if (((rcaps + price) > opt.kub.caps) || (price > info.caps)) {
			break;
		}
		elm = frm->findFirstElement("button#push div.c");
		click(ui.browser, elm);
		ui.browser->reload();
		waitLoading(ui.browser);
		rcaps += price;
	} while (rcaps < opt.kub.caps);
	log(trUtf8("Покрутили барабан"),"baraban.png");
	opt.kub.date = QDate::currentDate();
	setBusy(false);
}
