#include "main.h"

void MWBotWin::checkPolice() {
	QWebElement elm = frm->findFirstElement("a.bubble span.text");
	if (elm.toPlainText().contains(trUtf8("Задержан за бои"))) {
		loadPath("square:police");
		getFastRes();
		if (!opt.police.fine || (info.ore < 5)) {
			if (state.botWork) start();
			log(trUtf8("Вы задержаны. Бот остановлен"));
		} else {
			click(ui.browser, "form#fine-form div.button div.c");
			log(trUtf8("Заплачен штраф в полиции"));
		}
		if (opt.police.relations && (info.ore > 20)) {
			elm = frm->findFirstElement("form.police-relations div.button div.c");
			if (!elm.isNull()) {
				click(ui.browser, "form.police-relations div.button div.c");
				log(trUtf8("Связи в полиции продлены"));
			}
		}
	}
}
