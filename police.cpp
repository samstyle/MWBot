#include "main.h"

void MWBotWin::checkPolice() {
	QWebElement elm = frm->findFirstElement("a.bubble span.text");
	if (elm.toPlainText().contains(trUtf8("Задержан за бои"))) {
		loadPath(QStringList() << "square" << "police");
		getFastRes();
		if (!opt.police.fine || (info.ore < 5)) {
			if (state.botWork) start();
			log(tr("Вы задержаны. Бот остановлен"));
		} else {
			clickElement("form#fine-form div.button div.c");
			log(tr("Заплачен штраф в полиции"));
		}
		if (opt.police.relations && (info.ore > 20)) {
			elm = frm->findFirstElement("form.police-relations div.button div.c");
			if (!elm.isNull()) {
				clickElement("form.police-relations div.button div.c");
				log(tr("Связи в полиции продлены"));
			}
		}
	}
}
