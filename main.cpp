#include <QtWebKit>
#include <QtGui>
#include <unistd.h>
#include <getopt.h>

#include "main.h"

QApplication* app;
MWBotWin* mwbot;
//int atackType = ATACK_LEVEL;
//int atackType2 = ATACK_VICTIM;
int playSum = 30000;
int buyCaps = 0;
//int opt = FL_ATACK | FL_MONIA | FL_MONIA_BILET | FL_MONIA_BUY | FL_PETRIK | FL_NONPC;
//int flag = 0;
//int minLev = 0;
//int maxLev = 99;
//double statPrc = 0.9;

QString clickq = "var evt=document.createEvent('MouseEvents'); evt.initMouseEvent('click',true,true,window,0,0,0,0,0,false,false,false,false,0,null); document.getElementById('samdaboom').dispatchEvent(evt);";

MWBotWin::MWBotWin() {
	ui.setupUi(this);
	frm = ui.browser->page()->mainFrame();
	opt = FL_ATACK | FL_MONIA | FL_MONIA_BILET | FL_MONIA_BUY | FL_PETRIK; // | FL_DIG | FL_DIGRAT;
	flag = FL_FIRST | FL_ATACK | FL_MONIA;
	atackType = ATACK_WEAK;
	atackType2 = ATACK_VICTIM;
	minLev = 0;
	maxLev = 99;
	statPrc = 0.9;

	ptrTime = QDateTime::currentDateTime();
	ratTime = ptrTime;

	ui.cbAtackType->addItem(trUtf8("Слабых"),ATACK_WEAK);
	ui.cbAtackType->addItem(trUtf8("Равных"),ATACK_EQUAL);
	ui.cbAtackType->addItem(trUtf8("Сильных"),ATACK_STRONG);
	ui.cbAtackType->addItem(trUtf8("Жертв"),ATACK_VICTIM);
	ui.cbAtackType->addItem(trUtf8("Врагов"),ATACK_ENEMY);
	ui.cbAtackType->addItem(trUtf8("по уровню"),ATACK_LEVEL);
	ui.cbAtackType->setCurrentIndex(ui.cbAtackType->findData(atackType));

	ui.cbAType2->addItem(trUtf8("Слабых"),ATACK_WEAK);
	ui.cbAType2->addItem(trUtf8("Равных"),ATACK_EQUAL);
	ui.cbAType2->addItem(trUtf8("Сильных"),ATACK_STRONG);
	ui.cbAType2->addItem(trUtf8("Жертв"),ATACK_VICTIM);
	ui.cbAType2->addItem(trUtf8("Врагов"),ATACK_ENEMY);
	ui.cbAType2->addItem(trUtf8("по уровню"),ATACK_LEVEL);
	ui.cbAType2->setCurrentIndex(ui.cbAtackType->findData(atackType2));

	connect(ui.pbOptSave,SIGNAL(clicked()),this,SLOT(apply()));

	connect(ui.browser,SIGNAL(loadStarted()),this,SLOT(onStart()));
	connect(ui.browser,SIGNAL(loadProgress(int)),ui.progress,SLOT(setValue(int)));
	connect(ui.browser,SIGNAL(loadFinished(bool)),this,SLOT(onLoad(bool)));

	connect(ui.tbStop,SIGNAL(clicked()),this,SLOT(stop()));
	connect(ui.tbStart,SIGNAL(clicked()),this,SLOT(start()));

	connect(ui.tbGipsy,SIGNAL(clicked()),this,SLOT(gipsy()));
	connect(ui.tbAtack,SIGNAL(clicked()),this,SLOT(atack()));
//	connect(ui.tbRat,SIGNAL(clicked()),this,SLOT(atackRat()));
//	connect(ui.tbOil,SIGNAL(clicked()),this,SLOT(atackOil()));
	connect(ui.tbPetrik,SIGNAL(clicked()),this,SLOT(makePetrik()));
	connect(ui.tbSellLot,SIGNAL(clicked()),this,SLOT(sellLots()));
	connect(ui.tbThimble,SIGNAL(clicked()),this,SLOT(goMonia()));
	connect(ui.tbDig,SIGNAL(clicked()),this,SLOT(dig()));
	connect(ui.tbBaraban,SIGNAL(clicked()),this,SLOT(playKub()));

//	loadCookies();
//	loadPage("player");
}

void MWBotWin::timerEvent(QTimerEvent*) {
	curTime = QDateTime::currentDateTime();
	QWebElement elm;

	if ((flag & FL_DIGGING) && (digTime < curTime)) digEnd();

	if (~flag & FL_BOT) return;

	if ((opt & FL_KUB) && (~flag & FL_KUB)) {
		elm = frm->findFirstElement("div.side-fractionwar");
		if (!elm.isNull()) {
			if (elm.toPlainText().contains(trUtf8("Приз в студию")))
				playKub();
		}
	}

	if (flag & FL_FIRST) {
		getAtackTimer();
//		if (opt & FL_RAT) getRatTimer();
//		if (opt & FL_PETRIK) makePetrik();
		flag &= ~FL_FIRST;
	}

	if ((opt & FL_PETRIK) && (ptrTime < curTime)) makePetrik();
	if ((opt & FL_ATACK) && (ataTime < curTime) && (~flag & FL_DIGGING)) {
//		if ((opt & FL_RAT) && (ratTime < curTime)) atackRat();
		atack();
		ResBox rbox = getResources();
		if ((opt & FL_MONIA) && (rbox.money > playSum)) {
			goMonia();
		}
	}
	if ((opt & FL_DIG) && (~flag & FL_DIGGING) && (curTime.addSecs(420) > ataTime)) dig();
}

// dig

void MWBotWin::dig() {
	setBusy(true);
	loadPath(QStringList() << "square" << "metro");
	clickElement("div.button[onclick='metroWork();'] div.c",0);
	QWebElement elm = frm->findFirstElement("td#metrodig");
	int sec = elm.attribute("timer").toInt() + 2;
	curTime = QDateTime::currentDateTime();
	digTime = curTime.addSecs(sec);
	QString tm = trUtf8("Копаем в метро, до окончания ").append(QString::number(sec)).append(trUtf8(" сек"));
	log(tm);
	flag |= FL_DIGGING;
	setBusy(false);
}

void MWBotWin::digEnd() {
	setBusy(true);
	log(trUtf8("Раскопка окончена"));
	loadPath(QStringList() << "square" << "metro");
	QWebElement elm = frm->findFirstElement("button.button[onclick='metroDig();'] div.c");
	if (elm.isNull()) {
		QWebElementCollection elms = frm->findAllElements("button.button[onclick] div.c");
		foreach(elm,elms) {
			if ((opt & FL_DIGRAT) && (elm.toPlainText().contains(trUtf8("Напасть")))) {
				log(trUtf8("Атакуем крысу"));
				restoreHP();
				elm.setAttribute("id","clickthis");
				clickElement("div#clickthis",0);
				int res = fightResult();
				switch (res) {
					case 0: opt &= ~FL_DIG; break;		// lose, don't dig anymore
					case 1: break;
					case 2: break;
				}
			}
			if ((~opt & FL_DIGRAT) && (elm.toPlainText().contains(trUtf8("убежать")))) {	// don't atack rat, don't dig
				log(trUtf8("Убегаем от крысы"));
				elm.setAttribute("id","clickthis");
				clickElement("div#clickthis",0);
				digEnd();
				opt &= ~FL_DIG;
			}
		}
	} else {
		clickElement("button.button[onclick='metroDig();'] div.c",0);
		elm = frm->findFirstElement("div#content div.report div.success");
		if (elm.isNull()) {
			log(trUtf8("Ничего не выкопано"));
		} else {
			log(trUtf8("Раскопка удачна"));
		}
	}
	flag &= ~FL_DIGGING;
	setBusy(false);
}

// gipsy

void MWBotWin::setBusy(bool bsy) {
//	if (flag & FL_BOT) bsy = true;
	ui.browser->setDisabled(bsy);
	ui.toolbar->setDisabled(bsy);
}

/*
void MWBotWin::scan() {
	if (flag & FL_SCAN) {
		log(trUtf8("Сканирование остановлено"));
		flag &= ~FL_SCAN;
		return;
	}
	flag |= FL_SCAN;
	log(trUtf8("Запуск сканирования"));
	loadPage("player/");
	CharBox myStat = getStat("div#content span.user","div#content td.stats-cell");
	CharBox enStat;
	QString str;
	QWebElement elm,tmp;
	QList<QWebElement> elist;
	loadPage("alley/");
	elm = frm->findFirstElement("input[name=minlevel]");
	elm.setAttribute("value",QString::number(myStat.level - 1));
	elm = frm->findFirstElement("input[name=maxlevel]");
	elm.setAttribute("value",QString::number(myStat.level + 1));
	clickElement("form#searchLevelForm div.button div.c",0);
	while (flag & FL_SCAN) {
		enStat = getStat("div.fighter2","td.fighter2-cell");
		if (enStat.statsum < (myStat.statsum * 0.9)) {
			str = QString("[").append(QString::number(enStat.level)).append("] ").append(enStat.name).append(" id=").append(QString::number(enStat.id));
			log(str);
		}
		clickElement("div.button.button-search div.c",0);
	}
}
*/

void MWBotWin::onLoad(bool) {
	loading = false;
}

void MWBotWin::onStart() {
	loading = true;
}

void MWBotWin::start() {
	log(trUtf8("Бот запущен"));
	ui.tbStart->setEnabled(false);
	flag = (FL_BOT | FL_FIRST);
}

void MWBotWin::stop() {
	if (!ui.browser->isEnabled()) flag |= FL_STOP;
	flag &= ~FL_BOT;
	log(trUtf8("Бот остановлен"));
	ui.tbStart->setEnabled(true);
}

// cookies

void MWBotWin::loadCookies() {
	QFile file(QDir::homePath().append("/.config/samstyle/mwbot/cookie.txt"));
	if (file.open(QFile::ReadOnly)) {
		QList<QNetworkCookie> list;
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
	QFile file(QDir::homePath().append("/.config/samstyle/mwbot/cookie.txt"));
	file.open(QFile::WriteOnly);
	for(int i = 0; i < cookies.size(); i++) {
		file.write(cookies[i].toRawForm());
		file.write("\n");
	}
	file.close();
}

// load - click

bool MWBotWin::loadPath(QStringList pth) {
	int i;
	QString fp;
	QUrl cur = ui.browser->page()->mainFrame()->url();
	for (i = 0; i < pth.size(); i++) {
		fp = pth.at(i);
		fp.prepend("http://www.moswar.ru/");
		if (!fp.endsWith("/")) fp.append("/");
		if (cur == QUrl(fp)) pth = pth.mid(i);
	}
	bool res = true;
	for (i = 0; i < pth.size(); i++) res &= loadPage(pth.at(i));
	return res;
}

bool MWBotWin::loadPage(QString pth) {
	pth.prepend("http://www.moswar.ru/");
	if (!pth.endsWith("/")) pth.append("/");
//	if (ui.browser->page()->mainFrame()->url() == QUrl(pth)) return true;
	mwbot->ui.browser->load(QUrl(pth));
	waitLoading(0);
	return (ui.browser->page()->mainFrame()->url() == QUrl(pth));
}

void MWBotWin::clickElement(QString quer,int wait) {
	QWebElement elm = frm->findFirstElement(quer);
	if (elm.isNull()) {
		log(trUtf8("DEBUG: элемент <b>").append(quer).append(trUtf8("</b> не найден")));
		return;
	}
	quer = clickq;
	if (elm.hasAttribute("id")) {
		quer.replace("samdaboom",elm.attribute("id"));
		frm->evaluateJavaScript(quer);
	} else {
		elm.setAttribute("id","samdaboom");
		frm->evaluateJavaScript(quer);
		elm.removeAttribute("id");
	}
	if (wait < 0) {
		waitDropDown();
	} else {
		waitLoading(wait);
	}
}

void MWBotWin::waitLoading(int wait) {
	if (wait < 1) wait = 1000;
	QWebElement elm;
	do {
		usleep(10000);
		app->processEvents();
	} while (loading);
	do {
		usleep(10000);
		app->processEvents();
		elm = frm->findFirstElement("div.loading-top");
	} while (!(elm.isNull() || elm.attribute("style").contains("none")));
	int sleeptime = 500 + (rand() % 1000);
	for (int i = 0; i < sleeptime; i++) {
		app->processEvents();
		usleep(wait);
	}
}

// get numbers

int MWBotWin::getAtackTimer() {
	int res;
	QWebElement elm = frm->findFirstElement("form#searchForm span.timer");
	ataTime = QDateTime::currentDateTime();
	if (elm.isNull()) {
		flag |= FL_ATACK;
		res = -1;
	} else {
		res = elm.attribute("timer").toInt();
		if (res < 1) {
			flag |= FL_ATACK;
		} else {
			flag &= ~FL_ATACK;
			res += (5 + (rand() % 10));
			ataTime = ataTime.addSecs(res);
//			log(QString::number(res).append(trUtf8(" сек. до следующего нападения")));
		}
	}
	return res;
}

int MWBotWin::getRatTimer() {
	loadPath(QStringList() << "square" << "metro");
	int time = frm->findFirstElement("td#ratfight").attribute("timer").toInt();
	ratTime = QDateTime::currentDateTime();
	if (time > 0) {
		time += (10 + (rand() % 40));
		ratTime = ratTime.addSecs(time);
//		ratTimer.singleShot(time * 1000,this,SLOT(switchRat()));
		flag &= ~FL_RAT;
		log(trUtf8("До спуска в метро примерно ").append(QString::number(time/60)).append(trUtf8(" мин.")));
		pers.nextRat = frm->findFirstElement("div#timer-rat-fight td.label").toPlainText().mid(9,2).toInt();
	} else {
		pers.nextRat = frm->findFirstElement("div#action-rat-fight div.holders").toPlainText().split(":").at(1).split(trUtf8("До")).first().trimmed().toInt();
		flag |= FL_RAT;
	}
	log(trUtf8("Следующая крыса уровня ").append(QString::number(pers.nextRat)));
	return time;
}

Object namIcon[] = {
	{QObject::trUtf8("Стеклопакет"),":/images/parts/1.png",0},
	{QObject::trUtf8("Стеклорез"),":/images/parts/2.png",0},
	{QObject::trUtf8("Балка"),":/images/parts/3.png",0},
	{QObject::trUtf8("Кирпич"),":/images/parts/4.png",0},
	{QObject::trUtf8("Цемент"),":/images/parts/5.png",0},
	{QObject::trUtf8("Краска"),":/images/parts/6.png",0},
	{QObject::trUtf8("Каучук"),":/images/parts/7.png",0},
	{QObject::trUtf8("Плавильная печь"),":/images/parts/8.png",0},
	{QObject::trUtf8("Насос"),":/images/parts/9.png",0},
	{QObject::trUtf8("Напильник"),":/images/parts/12.png",0},

	{QObject::trUtf8("Апельсин"),":/images/fruit/fruit1.png",0},
	{QObject::trUtf8("Лимон"),":/images/fruit/fruit2.png",0},
	{QObject::trUtf8("Яблоко"),":/images/fruit/fruit3.png",0},
	{QObject::trUtf8("Ананас"),":/images/fruit/fruit5.png",0},
	{QObject::trUtf8("Банан"),":/images/fruit/fruit6.png",0},
	{QObject::trUtf8("Арбуз"),":/images/fruit/fruit7.png",0},
	{QObject::trUtf8("Киви"),":/images/fruit/fruit8.png",0},
	{QObject::trUtf8("Малина"),":/images/fruit/fruit9.png",0},
	{QObject::trUtf8("Манго"),":/images/fruit/fruit10.png",0},
	{QObject::trUtf8("Дыня"),":/images/fruit/fruit11.png",0},
	{QObject::trUtf8("Персик"),":/images/fruit/fruit12.png",0},
	{QObject::trUtf8("Виноград"),":/images/fruit/fruit13.png",0},

	{"","",-1}
};

FightBox MWBotWin::getResult() {
	FightBox res;
	QWebElement elm;
	Object obj;
	QWebElementCollection coll;
	int mylife = frm->findFirstElement("span#fighter1-life").toPlainText().split("/").first().trimmed().toInt();
	int enlife = frm->findFirstElement("span#fighter2-life").toPlainText().split("/").first().trimmed().toInt();
	res.result = (mylife > 0) ? 1 : ((enlife > 0) ? 0 : 2);

	obj.name = trUtf8("деньги");
	obj.icon = ":/images/money.png";
	obj.count = frm->findFirstElement("li.result span.tugriki").attribute("title").split(":").last().trimmed().toInt();
	if (obj.count != 0) res.items.append(obj);

	if (res.result == 1) {			// check all only if win

		obj.name = trUtf8("опыт");
		obj.icon = ":/images/lamp.png";
		obj.count = frm->findFirstElement("li.result span.expa").attribute("title").split(":").last().trimmed().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("нефть");
		obj.icon = ":/images/neft.png";
		obj.count = frm->findFirstElement("li.result span.neft").attribute("title").split(":").last().trimmed().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("пули");
		obj.icon = ":/images/bullet.png";
		obj.count = frm->findFirstElement("li.result span.bullet").toPlainText().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("звездочки");
		obj.icon = ":/images/sparkle.png";
		obj.count = frm->findFirstElement("li.result span.sparkles").toPlainText().toInt();
		if (obj.count != 0) res.items.append(obj);

		//	obj.icon.clear();
		coll = frm->findAllElements("li.result span.object-thumb");
		int idx;
		foreach(elm, coll) {
			obj.count = elm.findFirst("span.count").toPlainText().toInt();
			if (obj.count != 0) {
				obj.name = elm.findFirst("img").attribute("alt");
				obj.icon.clear();
				idx = 0;
				while (namIcon[idx].count != -1) {
					if (namIcon[idx].name == obj.name) obj.icon = namIcon[idx].icon;
					idx++;
				}
				res.items.append(obj);
			}
		}
	}
	elm = frm->findFirstElement("div.backlink div.button div.c");
	if (elm.isNull()) return res;

	clickElement("div.backlink div.button div.c",0);
	obj.name = trUtf8("руда");
	obj.icon = ":/images/ruda.png";
	obj.count = frm->findFirstElement("div#alert-text span.ruda").toPlainText().remove("\"").toInt();
	if (obj.count != 0) res.items.append(obj);
	obj.name = trUtf8("деньги");
	obj.icon = ":/images/money.png";
	obj.count = frm->findFirstElement("div#alert-text span.tugriki").toPlainText().remove("\"").toInt();
	if (obj.count != 0) res.items.append(obj);
	obj.icon.clear();
	coll = frm->findAllElements("div#alert-text span.object-thumb");
	foreach(elm,coll) {
		obj.name = elm.findFirst("img").attribute("alt");
		obj.count = elm.findFirst("span.count").toPlainText().remove("#").toInt();
		if (obj.count == 0) obj.count = 1;
		res.items.append(obj);
	}
	return res;
}

ResBox MWBotWin::getResources() {
	ResBox res;
	QWebElement elm;
	elm = frm->findFirstElement("span#currenthp");
	if (elm.isNull()) return res;
	res.curhp = elm.toPlainText().toInt();
	res.maxhp = frm->findFirstElement("span#maxhp").toPlainText().toInt();
	res.curtonus = frm->findFirstElement("span#currenttonus").toPlainText().toInt();
	res.maxtonus = frm->findFirstElement("span#maxenergy").toPlainText().toInt();
	res.money = frm->findFirstElement("li.tugriki-block").attribute("title").split(":").last().trimmed().toInt();
	res.ruda = frm->findFirstElement("li.ruda-block").attribute("title").split(":").last().trimmed().toInt();
	res.neft = frm->findFirstElement("li.neft-block").attribute("title").split(":").last().trimmed().toInt();
	res.med = frm->findFirstElement("li.med-block").attribute("title").split(":").last().trimmed().toInt();
	res.wanted = frm->findFirstElement("div.wanted div.percent").attribute("style").split(QRegExp("[:%]")).at(1).toInt();
	return res;
}

ResBox MWBotWin::getBerezkaRes() {
	ResBox res;
	res.gtooth = frm->findFirstElement("div.borderdata span.tooth-golden").toPlainText().toInt();
	res.wtooth = frm->findFirstElement("div.borderdata span.tooth-white").toPlainText().toInt();
	res.star = frm->findFirstElement("div.borderdata span.star").toPlainText().toInt();
	res.badge = frm->findFirstElement("div.borderdata span.badge").toPlainText().toInt();
	res.mobila = frm->findFirstElement("div.borderdata span.mobila").toPlainText().toInt();
	res.neft = frm->findFirstElement("div.borderdata span.neft").toPlainText().toInt();
	res.ipoints = frm->findFirstElement("div.borderdata span.ipoints-e").toPlainText().toInt();
	res.medals = frm->findFirstElement("div.borderdata span.pet-golden.counter").toPlainText().toInt();
	res.med = frm->findFirstElement("div.borderdata span.med").toPlainText().toInt();
	return res;
}

CharBox MWBotWin::getStat(QString querA, QString querB) {
	CharBox res;
	QWebElement elm = frm->findFirstElement(querA);
	QWebElement tmp = elm.findFirst("a[href]");
	if (tmp.isNull()) {
		res.clan.clear();
		res.id = 0;
		tmp = elm.findFirst("span.user");
	} else {
		if (tmp.attribute("href").contains("player")) {
			res.clan.clear();
		} else {
			res.clan = tmp.findFirst("img").attribute("title");
			tmp = elm.findAll("a[href]").last();
		}
		res.id = tmp.attribute("href").split("/",QString::SkipEmptyParts).last().trimmed().toInt();
	}
	res.name = tmp.toPlainText().trimmed();
	res.level = elm.findFirst("span.level").toPlainText().remove("[").remove("]").trimmed().toInt();
	res.type = elm.findFirst("i").attribute("class");

//	if (querB == "") querB = querA.replace("div","td").append("-cell");
	elm = frm->findFirstElement(querB);
	if (!elm.isNull()) {
		res.stat.zdor = elm.findFirst("li[data-type='health'] span.num").toPlainText().toInt();
		res.stat.sila = elm.findFirst("li[data-type='strength'] span.num").toPlainText().toInt();
		res.stat.lovk = elm.findFirst("li[data-type='dexterity'] span.num").toPlainText().toInt();
		res.stat.vynos = elm.findFirst("li[data-type='resistance'] span.num").toPlainText().toInt();
		res.stat.hitr = elm.findFirst("li[data-type='intuition'] span.num").toPlainText().toInt();
		res.stat.vnim = elm.findFirst("li[data-type='attention'] span.num").toPlainText().toInt();
		res.stat.hari = elm.findFirst("li[data-type='charism'] span.num").toPlainText().toInt();
		res.statsum = res.stat.zdor + res.stat.sila + res.stat.lovk + res.stat.vynos + res.stat.hitr + res.stat.vnim;
	} else {
		printf("ZZZ\n");
	}
	return res;
}

// attack

/*
void MWBotWin::switchAtack() {
	flag |= FL_ATACK;
	log(trUtf8("Атака возможна"));
}
*/

void MWBotWin::restoreHP() {
	ResBox rbox = getResources();
	if (rbox.curhp < rbox.maxhp * 0.95) {
		clickElement("div.life i.plus-icon",0);
		clickElement("div.actions button.button div.c",0);
	}
}

/*
void MWBotWin::switchRat() {
	flag |= FL_RAT;
	log(trUtf8("Атака на крысу возможна"));
}
*/

void MWBotWin::atackRat() {
	if (~flag & FL_ATACK) return;
	if ((pers.nextRat > 0) && (pers.nextRat % 5 == 0)) {
		log(trUtf8("В групповые бои с крысами не ходим: уровень ").append(QString::number(pers.nextRat)));
		flag &= ~FL_RAT;
		return;
	}

	setBusy(true);

	loadPath(QStringList() << "square" << "metro");
//	loadPage("square/");
//	loadPage("metro/");

	int time = getRatTimer();
	if (time < 1) {
		log(trUtf8("Уровень крысы: ").append(QString::number(pers.nextRat)));
		if ((pers.nextRat % 5) == 0) {
			log(trUtf8("В групповые бои с крысами не ходим"));
			flag &= ~FL_RAT;
		} else {
			restoreHP();
			log(trUtf8("Нападаем на крысу"));
			clickElement("div#action-rat-fight div.button div.c",0);
			clickElement("div#welcome-rat button.button div.c",0);
			fightResult();
			getRatTimer();
		}
	}
	setBusy(false);
}

void MWBotWin::atackOil() {
	if (pers.lev < 10) return;
	if (~flag & FL_ATACK) return;
	if (~flag & FL_OIL) return;

	int time = getAtackTimer();
	if (time > 0) return;

	setBusy(true);

	loadPath(QStringList() << "tverskaya" << "neft");
//	loadPage("tverskaya/");
//	loadPage("neft/");

//	QWebElement elm;
	// frm->findFirstElement("div#ventel-overtip div.object h2").toPlainText();	// name [lev]
//	elm = frm->findFirstElement("div.enemy div.action button.button div.c");
	clickElement("div.enemy div.action button.button div.c",0);
	if (fightResult() == 0) flag &= ~FL_OIL;

	getAtackTimer();

	setBusy(false);
}

// click select lev-1..lev+1 in alley


// get results


int MWBotWin::fightResult() {
	QWebElement elm;
	CharBox enstat = getStat("div.fighter2","td.fighter2-cell");
	QString enname = enstat.name.append(" [").append(QString::number(enstat.level)).append("]");
	QString tolog;
	int cnt = 100;
	do {
		elm = frm->findFirstElement("i.icon.icon-forward");
		app->processEvents();
		usleep(20000);
		cnt--;
	} while (elm.isNull() && (cnt > 0));
	if (cnt < 1) {
		log(trUtf8("...ошибка при нападении"));
		return -1;
	}
	clickElement("i.icon.icon-forward",0);
//	frm->evaluateJavaScript("fightForward();");
	FightBox res = getResult();
	if (res.result == 2) {
		tolog = trUtf8("<font style=background-color:#e0e020>Нападение на ").append(enname).append(trUtf8(" закончилось ничьей</font>"));
	} else {
		if (res.result == 1) {
			tolog = trUtf8("<font style=background-color:#20e020>Победа над ");
		} else {
			tolog = trUtf8("<font style=background-color:#e02020>Поражение от ");
		}
		tolog.append(enname).append("</font>&nbsp;");
		QString fightres;
		foreach(Object obj,res.items) {
			fightres.append("<font style=background-color:#d0d0d0>");
			if (obj.icon.isEmpty()) {
				fightres.append(obj.name);
			} else {
				fightres.append("<img width=16 height=16 src=").append(obj.icon).append(">");
			}
			fightres.append("&nbsp;").append(QString::number(obj.count)).append("</font>&nbsp;");
		}
		tolog.append(fightres);
	}
	log(tolog);
	return res.result;
}

// make petriks

/*
void MWBotWin::switchPetrik() {
	flag |= FL_PETRIK;
	log(trUtf8("Петрики сварены"));
}
*/

void MWBotWin::makePetrik() {
	ResBox rbox = getResources();
	if ((rbox.money < 600) || (rbox.ruda < 5)) {
		ptrTime = ptrTime.addSecs(600);		// 10 min delay
		log(trUtf8("Для производства петриков не хватает ресурсов"));
	}
	setBusy(true);
	loadPath(QStringList() << "square" << "factory");
	QWebElement elm = frm->findFirstElement("span#petriksprocess");
	ptrTime = QDateTime::currentDateTime();
	if (!elm.isNull() && (elm.attribute("timer").toInt() > 0)) {
		flag &= ~FL_PETRIK;
		int time = (elm.attribute("timer").toInt() + 80);
		ptrTime = ptrTime.addSecs(time);
		log(trUtf8("Петрики уже варятся, до окончания ").append(QString::number(time/60).append(trUtf8(" мин."))));
	} else {
		elm = frm->findFirstElement("form.factory-nanoptric button[type='submit'] div.c");
		if (!elm.isNull()) {
			flag &= ~FL_PETRIK;
			clickElement("form.factory-nanoptric button[type='submit'] div.c",0);
			ptrTime = ptrTime.addSecs(3670);
			log(trUtf8("Начато производство петриков"));
		}
	}
	setBusy(false);
}

// play thimble


// sell coctails

void MWBotWin::sellLots() {
	setBusy(true);
	QWebElement elm;
	QWebElement tmp;
	loadPage("square/");
	loadPage("nightclub/");
	loadPage("nightclub/vip/");
	elm = frm->findFirstElement("div.rating span.respect.value");
	int resp = elm.toPlainText().trimmed().toInt();
	if (resp < 2000) {
		log(trUtf8("Для аукциона недостаточно респекта"));
	} else {
		// loadPage("nightclub/vip/mystuff/");
		bool selled;
		int count = 0;
		QWebElementCollection col;
		do {
			selled = false;
			col = frm->findAllElements("div.block-rounded.glamour-sale");
			foreach(elm,col) {
				tmp = elm.findFirst("h2");
				if (tmp.toPlainText().contains(trUtf8("Коктейль"))) {
					tmp = elm.findFirst("form[action='/nightclub/vip/toauction/']");
					if (!tmp.isNull()) {
						tmp = tmp.findFirst("button.button div.c");
						tmp.setAttribute("id","kzkz");
						clickElement("div#kzkz",500);
						count++;
						selled = true;
						break;
					}
				}
			}
		} while(selled);
		log(trUtf8("На продажу выставлено ").append(QString::number(count)).append(trUtf8(" коктейлей")));
	}
	setBusy(false);
}

// roll baraban

void MWBotWin::waitDropDown() {
	QWebElement elm;
	do {
		elm = frm->findFirstElement("div.loading-top");
		usleep(10000);
		app->processEvents();
	} while (elm.attribute("style").contains("none"));
	do {
		elm = frm->findFirstElement("div.loading-top");
		usleep(10000);
		app->processEvents();
	} while (!elm.attribute("style").contains("none"));
}

void MWBotWin::playKub() {
	setBusy(true);
	loadPath(QStringList() << "/arbat/" << "/casino/");
	QWebElement elm;
//
//	int caps = elm.toPlainText().toInt();
	if (buyCaps > 0) {
		elm = frm->findFirstElement("input#stash-change-ore");
		if (!elm.isNull()) {
			elm.setAttribute("value",QString::number(buyCaps));
			clickElement("button#button-change-ore div.c",0);
		}
	}
	loadPage("/casino/kubovich/");
	int caps;
	int ncaps;
	int needCaps;
	do {
		elm = frm->findFirstElement("button#push-ellow");
		if (elm.isNull()) break;
		if (!elm.attribute("class").contains("disabled")) {	// yellow
			log(trUtf8("Крутим желтый барабан"));
			clickElement("button#push-ellow div.c",0);
		}
		elm = frm->findFirstElement("span#fishki-balance-num");
		caps = elm.toPlainText().remove(",").toInt();
		elm = frm->findFirstElement("button#push.button span.cost span.fishki");
		if (elm.isNull())
			needCaps = 0;
		else
			needCaps = elm.toPlainText().toInt();
//		log(QString::number(caps).append(":").append(QString::number(needCaps)));
		flag |= FL_KUB;
		if (caps < needCaps) break;
		log(trUtf8("Крутим барабан за ").append(QString::number(needCaps)).append(trUtf8(" фишек")));
		clickElement("button#push.button div.c",-1);
//		waitDropDown();
		if (needCaps != 0) {
			do {
				ncaps = frm->findFirstElement("span#fishki-balance-num").toPlainText().toInt();
				usleep(10000);
				app->processEvents();
			} while (caps == ncaps);
		}
//		log("Reload");
		ui.browser->reload();
		waitLoading(0);
//		log("----");
	} while (caps >= needCaps);
	setBusy(false);
}

// options

void MWBotWin::loadOpts() {
	QFile file(QDir::homePath().append("/.config/samstyle/mwbot/config.conf"));
	if (file.open(QFile::ReadOnly)) {
		opt = 0;
		QString line;
		QStringList pars;
		while (!file.atEnd()) {
			line = QDialog::trUtf8(file.readLine()).remove("\r").remove("\n");
			pars = line.split(":",QString::SkipEmptyParts);
			if (pars.size() == 2) {
				if ((pars.first() == "atack") && (pars.last() == "yes")) opt |= FL_ATACK;
				if (pars.first() == "atype") atackType = pars.last().toInt();
				if (pars.first() == "atype2") atackType2 = pars.last().toInt();
				if ((pars.first() == "makepetrik") && (pars.last() == "yes")) opt |= FL_PETRIK;
				if ((pars.first() == "playmon") && (pars.last() == "yes")) opt |= FL_MONIA;
				if (pars.first() == "playsum") {
					playSum = pars.last().toInt();
					if (playSum == 0) playSum = 30000;
				}
				if ((pars.first() == "playtickets") && (pars.last() == "yes")) opt |= FL_MONIA_BILET;
				if ((pars.first() == "buytickets") && (pars.last() == "yes")) opt |= FL_MONIA_BUY;
				if ((pars.first() == "buytickets_star") && (pars.last() == "yes")) opt |= FL_MONIA_STAR;
				if ((pars.first() == "rathunt") && (pars.last() == "yes")) opt |= FL_RAT;
				if ((pars.first() == "digger") && (pars.last() == "yes")) opt |= FL_DIG;
				if ((pars.first() == "digrat") && (pars.last() == "yes")) opt |= FL_DIGRAT;
				if ((pars.first() == "baraban") && (pars.last() == "yes")) opt |= FL_KUB;
				if (pars.first() == "buycaps") {
					buyCaps = pars.last().toInt();
					if (buyCaps > 20) buyCaps = 20;
					if (buyCaps < 0) buyCaps = 0;
				}
				if (pars.first() == "minlev") {
					minLev = pars.last().toInt();
					if (minLev > 99) minLev = 99;
					if (minLev < 1) minLev = 1;
				}
				if (pars.first() == "maxlev") {
					maxLev = pars.last().toInt();
					if (maxLev > 99) maxLev = 99;
					if (maxLev < 1) maxLev = 1;
				}
				if (pars.first() == "statprc") {
					statPrc = pars.last().toDouble();
					if (statPrc <= 0) statPrc = 0.5;
					if (statPrc > 2) statPrc = 2;
				}
				if ((pars.first() == "checknpc") && (pars.last() == "no")) opt |= FL_NONPC;
			}
		}
		if (minLev > maxLev) {
			int tmp = minLev;
			minLev =  maxLev;
			maxLev = tmp;
		}
	}
	mwbot->setOpts();
}

void MWBotWin::saveOpts() {
	QFile file(QDir::homePath().append("/.config/samstyle/mwbot/config.conf"));
	if (file.open(QFile::WriteOnly)) {
		file.write(QString("atack:").append((opt & FL_ATACK) ? "yes" : "no").append("\n").toUtf8());
		file.write(QString("atype:").append(QString::number(atackType)).append("\n").toUtf8());
		file.write(QString("atype2:").append(QString::number(atackType2)).append("\n").toUtf8());
		file.write(QString("makepetrik:").append((opt & FL_PETRIK) ? "yes" : "no").append("\n").toUtf8());
		file.write(QString("playmon:").append((opt & FL_MONIA) ? "yes" : "no").append("\n").toUtf8());
		file.write(QString("playsum:").append(QString::number(playSum)).append("\n").toUtf8());
		file.write(QString("playtickets:").append((opt & FL_MONIA_BILET) ? "yes" : "no").append("\n").toUtf8());
		file.write(QString("buytickets:").append((opt & FL_MONIA_BUY) ? "yes" : "no").append("\n").toUtf8());
		file.write(QString("buytickets_star:").append((opt & FL_MONIA_STAR) ? "yes" : "no").append("\n").toUtf8());
		file.write(QString("rathunt:").append((opt & FL_RAT) ? "yes" : "no").append("\n").toUtf8());
		file.write(QString("digger:").append((opt & FL_DIG) ? "yes" : "no").append("\n").toUtf8());
		file.write(QString("digrat:").append((opt & FL_DIGRAT) ? "yes" : "no").append("\n").toUtf8());
		file.write(QString("baraban:").append((opt & FL_KUB) ? "yes" : "no").append("\n").toUtf8());
		file.write(QString("buycaps:").append(QString::number(buyCaps)).append("\n").toUtf8());
		file.write(QString("minlev:").append(QString::number(minLev)).append("\n").toUtf8());
		file.write(QString("maxlev:").append(QString::number(maxLev)).append("\n").toUtf8());
		file.write(QString("statprc:").append(QString::number(statPrc)).append("\n").toUtf8());
		file.write(QString("checknpc:").append((opt & FL_NONPC) ? "no" : "yes").append("\n").toUtf8());
	}
}

void MWBotWin::apply() {
	opt = 0;
	if (ui.cbAtack->isChecked()) opt |= FL_ATACK;
	atackType = ui.cbAtackType->itemData(ui.cbAtackType->currentIndex()).toInt();
	atackType2 = ui.cbAType2->itemData(ui.cbAType2->currentIndex()).toInt();
	if (ui.cbPetrik->isChecked()) opt |= FL_PETRIK;
	if (ui.cbMonia->isChecked()) opt |= FL_MONIA;
	playSum = ui.sbMoniaCoins->value();
	if (ui.cbPlayTickets->isChecked()) opt |= FL_MONIA_BILET;
	if (ui.cbBuyTickets->isChecked()) opt |= FL_MONIA_BUY;
	if (ui.cbTicketStars->isChecked()) opt |= FL_MONIA_STAR;
	if (ui.cbRatHunt->isChecked()) opt |= FL_RAT;
	if (ui.cbDigger->isChecked()) opt |= FL_DIG;
	if (ui.cbDigRat->isChecked()) opt |= FL_DIGRAT;
	if (ui.cbRoll->isChecked()) opt |= FL_KUB;
	buyCaps = ui.sbCaps->value();
	minLev = ui.sbMinLev->value();
	maxLev = ui.sbMaxLev->value();
	statPrc = ui.sbStatCheck->value();
	if (ui.cbNPCheck->isChecked()) opt |= FL_NONPC;
	saveOpts();
}

void MWBotWin::setOpts() {
	ui.cbAtack->setChecked(opt & FL_ATACK);
	ui.cbAtackType->setCurrentIndex(ui.cbAtackType->findData(atackType));
	ui.cbAType2->setCurrentIndex(ui.cbAType2->findData(atackType2));
	ui.cbPetrik->setChecked(opt & FL_PETRIK);
	ui.cbMonia->setChecked(opt & FL_MONIA);
	ui.sbMoniaCoins->setValue(playSum);
	ui.cbPlayTickets->setChecked(opt & FL_MONIA_BILET);
	ui.cbBuyTickets->setChecked(opt & FL_MONIA_BUY);
	ui.cbTicketStars->setChecked(opt & FL_MONIA_STAR);
	ui.cbRatHunt->setChecked(opt & FL_RAT);
	ui.cbDigger->setChecked(opt & FL_DIG);
	ui.cbDigRat->setChecked(opt & FL_DIGRAT);
	ui.cbRoll->setChecked(opt & FL_KUB);
	ui.sbCaps->setValue(buyCaps);
	ui.sbMinLev->setValue(minLev);
	ui.sbMaxLev->setValue(maxLev);
	ui.sbStatCheck->setValue(statPrc);
	ui.cbNPCheck->setChecked(opt & FL_NONPC);
}

int main(int ac,char** av) {
	app = new QApplication(ac,av);
//	int c;
//	int sum;
	mwbot = new MWBotWin;
	mwbot->loadOpts();
	mwbot->show();
	mwbot->loadCookies();
	mwbot->loadPage("player");
	mwbot->startTimer(2000);
//	QTimer::singleShot(1000,mwbot,SLOT(onTimer()));
	app->exec();
	mwbot->saveCookies();
	return 0;
}
