#include "main.h"

void MWBotWin::log(QString text, QString icon) {
	QString res = QTime::currentTime().toString("hh:mm:ss ");
	if (!icon.isEmpty()) {
		res.append(QString("<img height=16 width=16 src=':/images/%0'>&nbsp;").arg(icon));
	}
	res.append(text);
	ui.log->append(res);
}
