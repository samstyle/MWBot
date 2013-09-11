#include "main.h"

void MWBotWin::log(QString text) {
	QString res = QTime::currentTime().toString("hh:mm:ss ");
	res.append(text);
	ui.log->append(res);
}
