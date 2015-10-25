#include "main.h"

void MWBotWin::loadCookies() {
	QList<QNetworkCookie> list;
	QFile file(workDir + "cookie.txt");
	if (file.open(QFile::ReadOnly)) {
		QByteArray line;
		while(!file.atEnd()) {
			line = file.readLine();
			line.remove(line.size() - 1, 1);
			list.append(QNetworkCookie::parseCookies(line));
		}
		file.close();
		ui.browser->page()->networkAccessManager()->cookieJar()->setCookiesFromUrl(list,QUrl("http://moswar.ru"));
	}
}

void MWBotWin::saveCookies() {
	QList<QNetworkCookie> cookies = ui.browser->page()->networkAccessManager()->cookieJar()->cookiesForUrl(QUrl("http://moswar.ru"));
	QFile file(workDir + "cookie.txt");
	file.open(QFile::WriteOnly);
	for(int i = 0; i < cookies.size(); i++) {
		file.write(cookies[i].toRawForm());
		file.write("\n");
	}
	file.close();
}
