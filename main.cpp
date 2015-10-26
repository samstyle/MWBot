#include <QtWebKit>
#include <QtGui>
#include <unistd.h>
#include <getopt.h>

#include "main.h"

QApplication* app;

QString clickq = "\
var evt=document.createEvent('MouseEvents');\
evt.initMouseEvent('click',true,true,window,0,0,0,0,0,false,false,false,false,0,null);\
document.getElementById('samdaboom').dispatchEvent(evt);\
";

MWBotWin::MWBotWin() {
	ui.setupUi(this);
	frm = ui.browser->page()->mainFrame();

	workDir = QDir::homePath().append("/.config/samstyle/mwbot/");
	QDir dir;
	dir.mkpath(workDir);
	dir.mkpath(workDir + "cache");

	mgr = new QNetworkAccessManager(this);
	cache = new QNetworkDiskCache(this);
	cache->setCacheDirectory(workDir + "cache");
	mgr->setCache(cache);
	ui.browser->page()->setNetworkAccessManager(mgr);

	options = 0;
	flag = 0;//FL_TR_RUDA | FL_TR_OIL;

	curTime = QDateTime::currentDateTime();

	opt.atk.enabled = 1;
	opt.atk.typeA = ATK_WEAK;
	opt.atk.typeB = ATK_VICTIM;
	opt.atk.minLev = 0;
	opt.atk.maxLev = 2;
	opt.atk.statPrc = 0.85;

	opt.ratk.enabled = 1;
	opt.ratk.block = 0;
	opt.ratk.ratlev = 0;
	opt.ratk.maxlev = 5;
	opt.ratk.time = curTime;

	opt.petrik.make = 1;
	opt.petrik.money = 0;
	opt.petrik.ore = 0;
	opt.petrik.time = curTime;

	opt.petRun = 1;

	opt.monya.play = 1;
	opt.monya.buy = 1;
	opt.monya.stars = 0;
	opt.monya.block = 0;
	opt.monya.date = curTime.date();

	opt.kub.date = opt.monya.date;

	opt.bPet.train = 1;
	opt.bPet.useOre = 1;
	opt.bPet.useOil = 0;
	opt.bPet.block = 0;
	opt.bPet.money = 0;
	opt.bPet.ore = 0;
	opt.bPet.oil = 0;
	opt.bPet.time = curTime;

	state.botWork = 0;
	state.firstRun = 1;

	goldType = 250;

	runTime = curTime;

	ui.cbAtackType->addItem(trUtf8("Слабых"),ATK_WEAK);
	ui.cbAtackType->addItem(trUtf8("Равных"),ATK_EQUAL);
	ui.cbAtackType->addItem(trUtf8("Сильных"),ATK_STRONG);
	ui.cbAtackType->addItem(trUtf8("Жертв"),ATK_VICTIM);
	ui.cbAtackType->addItem(trUtf8("Врагов"),ATK_ENEMY);
	ui.cbAtackType->addItem(trUtf8("по уровню"),ATK_LEVEL);
	ui.cbAtackType->setCurrentIndex(ui.cbAtackType->findData(opt.atk.typeA));

	ui.cbAType2->addItem(trUtf8("Слабых"),ATK_WEAK);
	ui.cbAType2->addItem(trUtf8("Равных"),ATK_EQUAL);
	ui.cbAType2->addItem(trUtf8("Сильных"),ATK_STRONG);
	ui.cbAType2->addItem(trUtf8("Жертв"),ATK_VICTIM);
	ui.cbAType2->addItem(trUtf8("Врагов"),ATK_ENEMY);
	ui.cbAType2->addItem(trUtf8("по уровню"),ATK_LEVEL);
	ui.cbAType2->setCurrentIndex(ui.cbAtackType->findData(opt.atk.typeB));

	ui.boxGypsy->addItem(QIcon(":/images/gold.png"),"250",250);
	ui.boxGypsy->addItem(QIcon(":/images/gold.png"),"750",750);
//	ui.boxGypsy->addItem(QIcon(":/images/gold.png"),"150 + 2",150);
	ui.boxGypsy->setCurrentIndex(ui.boxGypsy->findData(goldType));

	ui.browser->setZoomFactor(0.75);

	connect(ui.pbOptSave,SIGNAL(clicked()),this,SLOT(apply()));

	connect(ui.browser,SIGNAL(loadStarted()),this,SLOT(onStart()));
	connect(ui.browser,SIGNAL(loadProgress(int)),ui.progress,SLOT(setValue(int)));
	connect(ui.browser,SIGNAL(loadFinished(bool)),this,SLOT(onLoad(bool)));
	connect(ui.browser,SIGNAL(titleChanged(QString)),this,SLOT(setWindowTitle(QString)));

	connect(ui.tbStop,SIGNAL(released()),this,SLOT(stop()));
	connect(ui.tbStart,SIGNAL(released()),this,SLOT(start()));
	connect(ui.tbSave,SIGNAL(released()),this,SLOT(savePage()));

	connect(ui.tbGipsy,SIGNAL(clicked()),this,SLOT(gipsy()));
	connect(ui.tbAtack,SIGNAL(clicked()),this,SLOT(attack()));
	connect(ui.tbRat,SIGNAL(clicked()),this,SLOT(atkRat()));
//	connect(ui.tbOil,SIGNAL(clicked()),this,SLOT(atackOil()));
	connect(ui.tbPetrik,SIGNAL(clicked()),this,SLOT(makePetrik()));
	connect(ui.tbSellLot,SIGNAL(clicked()),this,SLOT(sellLots()));
	connect(ui.tbThimble,SIGNAL(clicked()),this,SLOT(goMonia()));
	connect(ui.tbDig,SIGNAL(clicked()),this,SLOT(dig()));
	connect(ui.tbBaraban,SIGNAL(clicked()),this,SLOT(playKub()));
	connect(ui.tbTrainPet,SIGNAL(clicked()),this,SLOT(trainPet()));
	connect(ui.tbArena,SIGNAL(clicked()),this,SLOT(arena()));

//	loadCookies();
//	loadPage("player");
}

void MWBotWin::keyPressEvent(QKeyEvent* ev) {
	QString path;
	QFile file;
	switch(ev->key()) {
		case Qt::Key_F5:
			ui.browser->reload();
			break;
		case Qt::Key_F2:
			savePage();
			break;
	}
}

void MWBotWin::savePage() {
	QString path = QDir::homePath().append("/");
	path.append(windowTitle()).append("_").append(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")).append(".html");
	QFile file(path);
	if (file.open(QFile::WriteOnly)) {
		file.write(frm->toHtml().toUtf8());
		file.close();
	}
}

void MWBotWin::timerEvent(QTimerEvent*) {
	curTime = QDateTime::currentDateTime();

	if (!state.botWork) return;

	if (state.firstRun) {
		getAtackTimer();
		state.firstRun = 0;
	}

	getFastRes();
// train battle pet
	if (opt.bPet.train && \
		(info.money >= opt.bPet.money + 200) && \
		(info.ore >= opt.bPet.ore) && \
		(info.oil >= opt.bPet.oil) && \
		(curTime > opt.bPet.time)) {
		trainPet();
	}
// send pet to arena
	if (opt.petRun && (curTime > runTime)) {
		arena();
	}
// play baraban
/*
	if ((options & FL_KUB) && (~flag & FL_KUB)) {
		elm = frm->findFirstElement("div.side-fractionwar");
		if (!elm.isNull()) {
			if (elm.toPlainText().contains(trUtf8("Приз в студию")))
				playKub();
		}
	}
*/
// make petriks
	if (opt.petrik.make && \
		(info.money > (opt.petrik.money + 200)) && \
		(info.ore > opt.petrik.ore) && \
		(opt.petrik.time < curTime)) {
		makePetrik();
	}
// attack
	if (opt.atk.time < curTime) {
		if (!opt.ratk.block && opt.ratk.enabled && (opt.ratk.time < curTime) && (opt.ratk.ratlev < opt.ratk.maxlev)) {
			atkRat();
		}
		if (opt.atk.enabled) {
			attack();
		}
	}
// play with monya
	if (!opt.monya.block && opt.monya.play && (info.money > playSum)) {
		goMonia();
	}
}

// dig

void MWBotWin::dig() {
	setBusy(true);
	loadPath(QStringList() << "square" << "metro");
	clickElement("div.button[onclick='metroWork();'] div.c");
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
			if ((options & FL_DIGRAT) && (elm.toPlainText().contains(trUtf8("Напасть")))) {
				log(trUtf8("Атакуем крысу"));
				restoreHP();
				elm.setAttribute("id","clickthis");
				clickElement("div#clickthis");
				int res = fightResult();
				switch (res) {
					case 0: options &= ~FL_DIG; break;		// lose, don't dig anymore
					case 1: break;
					case 2: break;
				}
			}
			if ((~options & FL_DIGRAT) && (elm.toPlainText().contains(trUtf8("убежать")))) {	// don't atack rat, don't dig
				log(trUtf8("Убегаем от крысы"));
				elm.setAttribute("id","clickthis");
				clickElement("div#clickthis");
				digEnd();
				options &= ~FL_DIG;
			}
		}
	} else {
		clickElement("button.button[onclick='metroDig();'] div.c");
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
	state.loading = 0;
}

void MWBotWin::onStart() {
	state.loading = 1;
}

void MWBotWin::start() {
	log(trUtf8("Бот запущен"));
	ui.tbStart->setEnabled(false);
	flag = 0;
	state.botWork = 1;
	state.firstRun = 1;
}

void MWBotWin::stop() {
	if (!ui.browser->isEnabled()) flag |= FL_STOP;
	state.botWork = 0;
	log(trUtf8("Бот остановлен"));
	ui.tbStart->setEnabled(true);
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
	if (ui.browser->page()->mainFrame()->url() == pth) return true;
	QNetworkRequest rqst;
	rqst.setUrl(QUrl(pth));
	rqst.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);
	ui.browser->load(rqst);
	waitLoading();
	return (ui.browser->page()->mainFrame()->url() == QUrl(pth));
}

void MWBotWin::clickElement(QString quer, int speed) {
	QWebElement elm = frm->findFirstElement(quer);
	if (elm.isNull()) {
		qDebug() << QString("элемент '%0' не найден").arg(quer);
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
	waitLoading(speed);
}

void MWBotWin::waitLoading(int speed) {
	if (speed < 0) speed = 1000;
	QWebElement elm;
	do {
		usleep(1000);
		app->processEvents();
		elm = frm->findFirstElement("div.loading-top");
	} while (state.loading || !elm.attribute("style").contains("none"));
	int sleeptime = 500 + (rand() % 1000);
	while (sleeptime > 0) {
		usleep(speed);
		app->processEvents();
		sleeptime--;
	}
}

// get numbers

int MWBotWin::getAtackTimer() {
	int res;
	loadPage("alley/");
	QWebElement elm = frm->findFirstElement("div.need-some-rest div.holders span.timer");
	opt.atk.time = QDateTime::currentDateTime();
	if (elm.isNull()) {
		res = -1;
	} else {
		res = elm.attribute("timer").toInt();
		if (res > 0) {
			res += (5 + (rand() % 10));
			opt.atk.time = opt.atk.time.addSecs(res);
			log(QString::number(res).append(trUtf8(" сек. до следующего нападения")));
		}
	}
	return res;
}

int MWBotWin::getRatTimer() {
	loadPath(QStringList() << "square" << "metro");
	QWebElement elm = frm->findFirstElement("div#timer-rat-fight td#ratfight");
	int time = elm.attribute("timer").toInt();
	opt.ratk.time = QDateTime::currentDateTime();

	if (time > 0) {
		time += 60;
		opt.ratk.time = curTime.addSecs(time);
		log(trUtf8("До спуска в метро примерно %0 мин.").arg(time / 60));
	} else {
		elm = frm->findFirstElement("div#action-rat-fight div.holders");
		opt.ratk.ratlev = elm.toPlainText().split(":").at(1).left(3).trimmed().toInt();
	}
	return time;
}

mwItem namIcon[] = {
	{QObject::trUtf8("стеклопакет"),":/images/parts/1.png",0},
	{QObject::trUtf8("стеклорез"),":/images/parts/2.png",0},
	{QObject::trUtf8("балка"),":/images/parts/3.png",0},
	{QObject::trUtf8("кирпич"),":/images/parts/4.png",0},
	{QObject::trUtf8("цемент"),":/images/parts/5.png",0},
	{QObject::trUtf8("краска"),":/images/parts/6.png",0},
	{QObject::trUtf8("каучук"),":/images/parts/7.png",0},
	{QObject::trUtf8("плавильная печь"),":/images/parts/8.png",0},
	{QObject::trUtf8("насос"),":/images/parts/9.png",0},
	{QObject::trUtf8("чертёж"),":/images/parts/10.png",0},
	{QObject::trUtf8("болт"),":/images/parts/11.png",0},
	{QObject::trUtf8("напильник"),":/images/parts/12.png",0},

	{QObject::trUtf8("апельсин"),":/images/fruit/fruit1.png",0},
	{QObject::trUtf8("лимон"),":/images/fruit/fruit2.png",0},
	{QObject::trUtf8("яблоко"),":/images/fruit/fruit3.png",0},
	{QObject::trUtf8("мандарин"),":/images/fruit/fruit4.png",0},
	{QObject::trUtf8("ананас"),":/images/fruit/fruit5.png",0},
	{QObject::trUtf8("банан"),":/images/fruit/fruit6.png",0},
	{QObject::trUtf8("арбуз"),":/images/fruit/fruit7.png",0},
	{QObject::trUtf8("киви"),":/images/fruit/fruit8.png",0},
	{QObject::trUtf8("малина"),":/images/fruit/fruit9.png",0},
	{QObject::trUtf8("манго"),":/images/fruit/fruit10.png",0},
	{QObject::trUtf8("дыня"),":/images/fruit/fruit11.png",0},
	{QObject::trUtf8("персик"),":/images/fruit/fruit12.png",0},
	{QObject::trUtf8("виноград"),":/images/fruit/fruit13.png",0},

	{QObject::trUtf8("опыт"),":/images/lamp.png",0},
	{QObject::trUtf8("деньги"),":/images/money.png",0},
	{QObject::trUtf8("руда"),":/images/ruda.png",0},
	{QObject::trUtf8("нефть"),":/images/neft.png",0},
	{QObject::trUtf8("зубы"),":/images/tooth.png",0},
	{QObject::trUtf8("золотые зубы"),":/images/toothGold.png",0},
	{QObject::trUtf8("пули"),":/images/bullet.png",0},
	{QObject::trUtf8("звездочки"),":/images/sparkle.png",0},
	{QObject::trUtf8("искра"),":/images/sparkle.png",0},
	{QObject::trUtf8("снежинки"),":/images/snow.png",0},
	{QObject::trUtf8("жетоны"),":/images/badge.png",0},
	{QObject::trUtf8("мобила"),":/images/mobila.png",0},
	{QObject::trUtf8("подписи"),":/images/party-signature.png",0},
	{QObject::trUtf8("хвост крысомахи"),":/images/ratTail.png",0},

	{QObject::trUtf8("малый ларец"),":/images/boxes/box_metro1.png",0},
	{QObject::trUtf8("средний ларец"),":/images/boxes/box_metro2.png",0},
	{QObject::trUtf8("большой ларец"),":/images/boxes/box_metro3.png",0},
	{QObject::trUtf8("ключ от шахтерского ларца"),":/images/boxes/box_metro_key.png",0},

	{QObject::trUtf8("рубль"),":/images/rubel.png",0},

	{QObject::trUtf8("npc"),":/images/npc.png",0},
	{QObject::trUtf8("arrived"),":/images/arrived.png",0},
	{QObject::trUtf8("resident"),":/images/resident.png",0},

	{"","",-1}
};

QString MWBotWin::getItemIcon(QString name) {
	int idx = 0;
	QString res;
	name = name.toLower();
	while (namIcon[idx].count != -1) {
		if (namIcon[idx].name == name)
			res = namIcon[idx].icn;
		idx++;
	}
	return res;
}

FightBox MWBotWin::getResult() {
	FightBox res;
	QWebElement elm;
	mwItem obj;
	QWebElementCollection coll;
	int mylife = frm->findFirstElement("span#fighter1-life").toPlainText().split("/").first().trimmed().toInt();
	int enlife = frm->findFirstElement("span#fighter2-life").toPlainText().split("/").first().trimmed().toInt();
	res.result = (mylife > 0) ? 1 : ((enlife > 0) ? 0 : 2);

	obj.name = trUtf8("деньги");
	obj.count = 0;

	coll=frm->findAllElements("li.result span.tugriki");

	foreach(elm,coll) {
		if (elm.attribute("title").isNull()) {
			if (res.result == 1) obj.count += elm.toPlainText().trimmed().toInt();
		} else {
			obj.count += elm.attribute("title").split(":").last().trimmed().toInt();
		}
	}
	if (obj.count != 0) res.items.append(obj);

	if (res.result == 1) {			// check all only if win

		obj.name = trUtf8("опыт");
		obj.count = frm->findFirstElement("li.result span.expa").attribute("title").split(":").last().trimmed().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("нефть");
		obj.count = frm->findFirstElement("li.result span.neft").attribute("title").split(":").last().trimmed().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("пули");
		obj.count = frm->findFirstElement("li.result span.bullet").toPlainText().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("звездочки");
		obj.count = frm->findFirstElement("li.result span.sparkles").toPlainText().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("снежинки");
		obj.count = frm->findFirstElement("li.result span.snowflake").toPlainText().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("жетоны");
		obj.count = frm->findFirstElement("li.result span.badge").toPlainText().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("мобила");
		obj.count = frm->findFirstElement("li.result span.mobila").isNull() ? 0 : 1;
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("подписи");
		obj.count = frm->findFirstElement("li.result span.party_signature").toPlainText().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("зубы");
		obj.count = frm->findFirstElement("li.result span.tooth-white").toPlainText().toInt();
		if (obj.count != 0) res.items.append(obj);

		obj.name = trUtf8("золотые зубы");
		obj.count = frm->findFirstElement("li.result span.tooth-golden").toPlainText().toInt();
		if (obj.count != 0) res.items.append(obj);

		coll = frm->findAllElements("li.result span.object-thumb");
		//int idx;
		foreach(elm, coll) {
			obj.count = elm.findFirst("span.count").toPlainText().toInt();
			if (obj.count != 0) {
				obj.name = elm.findFirst("img").attribute("alt");
				res.items.append(obj);
			}
		}
	}
	elm = frm->findFirstElement("div.backlink div.button div.c");
	if (elm.isNull()) return res;

	clickElement("div.backlink div.button div.c");

	obj.name = trUtf8("руда");
	obj.count = frm->findFirstElement("div#alert-text span.ruda").toPlainText().remove("\"").toInt();
	if (obj.count != 0) res.items.append(obj);

	obj.name = trUtf8("деньги");
	obj.count = frm->findFirstElement("div#alert-text span.tugriki").toPlainText().remove("\"").toInt();
	if (obj.count != 0) res.items.append(obj);

	coll = frm->findAllElements("div#alert-text span.object-thumb");
	foreach(elm,coll) {
		obj.name = elm.findFirst("img").attribute("alt");
		obj.count = elm.findFirst("span.count").toPlainText().remove("#").toInt();
		if (obj.count == 0) obj.count = 1;
		res.items.append(obj);
	}
	return res;
}

void MWBotWin::getFastRes() {
	QWebElement elm = frm->findFirstElement("span#currenthp");
	if (elm.isNull()) return;
	info.hp = elm.toPlainText().toInt();
	info.maxhp = frm->findFirstElement("span#maxhp").toPlainText().toInt();
	info.tonus = frm->findFirstElement("span#currenttonus").toPlainText().toInt();
	info.maxtonus = frm->findFirstElement("span#maxenergy").toPlainText().toInt();
	info.money = frm->findFirstElement("li.tugriki-block").attribute("title").split(":").last().trimmed().toInt();
	info.ore = frm->findFirstElement("li.ruda-block").attribute("title").split(":").last().trimmed().toInt();
	info.oil = frm->findFirstElement("li.neft-block").attribute("title").split(":").last().trimmed().toInt();
	info.honey = frm->findFirstElement("li.med-block").attribute("title").split(":").last().trimmed().toInt();
	info.wanted = frm->findFirstElement("div.wanted div.percent").attribute("style").split(QRegExp("[:%]")).at(1).toInt();
}

void MWBotWin::getBerezkaRes() {
	info.tooth = frm->findFirstElement("div.borderdata span.tooth-white").toPlainText().toInt();
	info.goldtooth = frm->findFirstElement("div.borderdata span.tooth-golden").toPlainText().toInt();
	info.star = frm->findFirstElement("div.borderdata span.star").toPlainText().toInt();
	info.badge = frm->findFirstElement("div.borderdata span.badge").toPlainText().toInt();
	info.mobile = frm->findFirstElement("div.borderdata span.mobila").toPlainText().toInt();
	info.oil = frm->findFirstElement("div.borderdata span.neft").toPlainText().toInt();
	//res.ipoints = frm->findFirstElement("div.borderdata span.ipoints-e").toPlainText().toInt();
	info.power = frm->findFirstElement("div.borderdata span.power.counter").toPlainText().toInt();
	info.medal = frm->findFirstElement("div.borderdata span.pet-golden.counter").toPlainText().toInt();
	info.honey = frm->findFirstElement("div.borderdata span.med").toPlainText().toInt();
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
	getFastRes();
	if (info.hp < info.maxhp * 0.95) {
		clickElement("div.life i.plus-icon");
		clickElement("div.actions button.button div.c");
	}
}

void MWBotWin::atkRat() {
//	if ((opt.ratk.ratlev > 0) && (opt.ratk.ratlev % 5 == 0)) {
//		log(trUtf8("В групповые бои с крысами не ходим: уровень %0").arg(opt.ratk.ratlev));
//		opt.ratk.block = 1;
//		return;
//	}
	setBusy(true);
	loadPath(QStringList() << "square" << "metro");
	int time = getRatTimer();
	if (time < 1) {
		log(trUtf8("Уровень крысы: %0").arg(opt.ratk.ratlev));
//		if ((opt.ratk.ratlev % 5) == 0) {
//			log(trUtf8("В групповые бои с крысами не ходим"));
//			opt.ratk.block = 1;
//		} else {
			restoreHP();
			//log(trUtf8("Нападаем на крысу"));
			clickElement("div#action-rat-fight div.button div.c");
			clickElement("div#welcome-rat button.button div.c");
			if (opt.ratk.ratlev % 5 == 0) {
				groupFight();
			} else {
				fightResult();
			}
			getRatTimer();
//		}
	}
	setBusy(false);
}

void MWBotWin::atackOil() {
	return;

	getFastRes();
	if (info.level < 10) return;
	if (~flag & FL_OIL) return;

	int time = getAtackTimer();
	if (time > 0) return;

	setBusy(true);

	loadPath(QStringList() << "tverskaya" << "neft");

//	clickElement("div.enemy div.action button.button div.c");
//	if (fightResult() == 0) flag &= ~FL_OIL;

//	getAtackTimer();

	setBusy(false);
}

// click select lev-1..lev+1 in alley


// get results


int MWBotWin::fightResult() {
	QWebElement elm;
	CharBox enstat = getStat("div.fighter2","td.fighter2-cell");
	QString enname = QString("%0 [%1]").arg(enstat.name).arg(enstat.level);
	QString tolog;
	QString icn;
	QString fightres;
	QString nname;
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
	clickElement("i.icon.icon-forward");
	FightBox res = getResult();	
	nname = getItemIcon(enstat.type);
	if (res.result == 2) {
		tolog = QString("<font style=background-color:#e0e020><img src=%0>&nbsp;%1</font>").arg(nname).arg(enname);
	} else {
		if (res.result == 1) {
			tolog = QString("<font style=background-color:#20e020><img src=%0>&nbsp;").arg(nname);
		} else {
			tolog = QString("<font style=background-color:#e02020><img src=%0>&nbsp;").arg(nname);
		}
		tolog.append(enname).append("</font>&nbsp;");
		foreach(mwItem obj,res.items) {
			fightres.append("<font style=background-color:#d0d0d0>");
			icn = getItemIcon(obj.name);
			// qDebug() << obj.name << icn;
			if (icn.isEmpty()) {
				icn = ":/images/unknown.png";
			}
			fightres.append(QString("<img width=16 height=16 src='%0' title='%1'>").arg(icn).arg(obj.name));
			fightres.append(QString("&nbsp;%0</font>&nbsp;").arg(obj.count));
		}
		tolog.append(fightres);
	}
	log(tolog);
	return res.result;
}

// make petriks

void MWBotWin::makePetrik() {
	QWebElement elm;
	setBusy(true);
	loadPath(QStringList() << "square" << "factory");
	getFastRes();
	curTime = QDateTime::currentDateTime();

	elm = frm->findFirstElement("span#petriksprocess");
	if (!elm.isNull() && (elm.attribute("timer").toInt() > 0)) {
		int time = elm.attribute("timer").toInt() + 80;
		opt.petrik.time = curTime.addSecs(time);
		log(trUtf8("Петрики уже варятся, до окончания %0 минут").arg(QString::number(time / 60 + 1)));
	} else {
		elm = frm->findFirstElement("form.factory-nanoptric button.button div.c");
		if (elm.isNull()) {
			log(trUtf8("Ошибка варки петриков. Следующая попытка через минуту."));
			opt.petrik.time = curTime.addSecs(60);
		} else {
			elm = frm->findFirstElement("form.factory-nanoptric button.button div.c span.tugriki");
			opt.petrik.money = elm.toPlainText().toInt();
			elm = frm->findFirstElement("form.factory-nanoptric button.button div.c span.ruda");
			opt.petrik.ore = elm.toPlainText().toInt();
			if ((info.money > (opt.petrik.money + 199)) && (info.ore >= opt.petrik.ore)) {
				clickElement("form.factory-nanoptric button.button div.c");
				opt.petrik.time = curTime.addSecs(3700);
				log(trUtf8("Начато производство петриков"));
			} else {
				log(trUtf8("Для производства петриков недостаточно ресурсов"));
			}
		}
	}
	setBusy(false);
}


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
						clickElement("div#kzkz");
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
	loadPath(QStringList() << "arbat" << "casino");
	QWebElement elm;
//
//	int caps = elm.toPlainText().toInt();
	if (buyCaps > 0) {
		elm = frm->findFirstElement("input#stash-change-ore");
		if (!elm.isNull()) {
			elm.setAttribute("value",QString::number(buyCaps));
			clickElement("button#button-change-ore div.c");
		}
	}
	/*
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
	*/
	setBusy(false);
}

// options

void MWBotWin::loadOpts() {
	QFile file(workDir + "config.conf");
	if (file.open(QFile::ReadOnly)) {
		options = 0;
		QString line;
		QStringList pars;
		while (!file.atEnd()) {
			line = QDialog::trUtf8(file.readLine()).remove("\r").remove("\n");
			pars = line.split(":",QString::SkipEmptyParts);
			if (pars.size() == 2) {
				if (pars.first() == "atack") opt.atk.enabled = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "atype") opt.atk.typeA = pars.last().toInt();
				if (pars.first() == "atype2") opt.atk.typeB = pars.last().toInt();
				if (pars.first() == "makepetrik") opt.petrik.make = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "playmon") opt.monya.play = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "playsum") {
					playSum = pars.last().toInt();
					if (playSum == 0) playSum = 30000;
				}
				if (pars.first() == "playtickets") opt.monya.tickets = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "buytickets") opt.monya.buy = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "buytickets_star") opt.monya.stars = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "rathunt") opt.ratk.enabled = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "ratmaxlev") {
					opt.ratk.maxlev = pars.last().toInt();
					if (opt.ratk.maxlev < 1) opt.ratk.maxlev = 1;
					else if (opt.ratk.maxlev > 40) opt.ratk.maxlev = 40;
				}
				if ((pars.first() == "digger") && (pars.last() == "yes")) options |= FL_DIG;
				if ((pars.first() == "digrat") && (pars.last() == "yes")) options |= FL_DIGRAT;
				if ((pars.first() == "baraban") && (pars.last() == "yes")) options |= FL_KUB;
				if (pars.first() == "buycaps") {
					buyCaps = pars.last().toInt();
					if (buyCaps > 20) buyCaps = 20;
					if (buyCaps < 0) buyCaps = 0;
				}
				if (pars.first() == "minlev") {
					opt.atk.minLev = pars.last().toInt();
					if (opt.atk.minLev > 50) {
						opt.atk.minLev = 50;
					} else if (opt.atk.minLev < -50) {
						opt.atk.minLev = -50;
					}
				}
				if (pars.first() == "maxlev") {
					opt.atk.maxLev = pars.last().toInt();
					if (opt.atk.maxLev > 50) {
						opt.atk.maxLev = 50;
					} else if (opt.atk.maxLev < -50) {
						opt.atk.maxLev = -50;
					}
				}
				if (pars.first() == "statprc") {
					opt.atk.statPrc = pars.last().toDouble();
					if (opt.atk.statPrc <= 0) {
						opt.atk.statPrc = 0.5;
					} else if (opt.atk.statPrc > 2) {
						opt.atk.statPrc = 2;
					}
				}
				if ((pars.first() == "checknpc") && (pars.last() == "no")) options |= FL_NONPC;
				if (pars.first() == "trainpet") opt.bPet.train = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "tp-ruda") opt.bPet.useOre = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "tp-neft") opt.bPet.useOil = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "runner") opt.petRun = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "goldplay") goldType = pars.last().toInt();
			}
		}
		if (opt.atk.minLev > opt.atk.maxLev) {
			int tmp = opt.atk.minLev;
			opt.atk.minLev =  opt.atk.maxLev;
			opt.atk.maxLev = tmp;
		}
	}
	setOpts();
}

void MWBotWin::saveOpts() {
	QFile file(workDir + "config.conf");
	if (file.open(QFile::WriteOnly)) {
		file.write(QString("atack:%0\n").arg(opt.atk.enabled ? "yes" : "no").toUtf8());
		file.write(QString("atype:%0\n").arg(opt.atk.typeA).toUtf8());
		file.write(QString("atype2:%0\n").arg(opt.atk.typeB).toUtf8());
		file.write(QString("makepetrik:%0\n").arg(opt.petrik.make ? "yes" : "no").toUtf8());
		file.write(QString("playmon:%0\n").arg(opt.monya.play ? "yes" : "no").toUtf8());
		file.write(QString("playsum:").append(playSum).append("\n").toUtf8());
		file.write(QString("playtickets:%0\n").arg(opt.monya.tickets ? "yes" : "no").toUtf8());
		file.write(QString("buytickets:%0\n").arg(opt.monya.buy ? "yes" : "no").toUtf8());
		file.write(QString("buytickets_star%0\n:").arg(opt.monya.stars ? "yes" : "no").toUtf8());
		file.write(QString("rathunt:%0\n").arg(opt.ratk.enabled ? "yes" : "no").toUtf8());
		file.write(QString("ratmaxlev:%0\n").arg(opt.ratk.maxlev).toUtf8());
		file.write(QString("digger:").append((options & FL_DIG) ? "yes" : "no").append("\n").toUtf8());
		file.write(QString("digrat:").append((options & FL_DIGRAT) ? "yes" : "no").append("\n").toUtf8());
		file.write(QString("baraban:").append((options & FL_KUB) ? "yes" : "no").append("\n").toUtf8());
		file.write(QString("buycaps:").append(QString::number(buyCaps)).append("\n").toUtf8());
		file.write(QString("minlev:%0\n").arg(QString::number(opt.atk.minLev)).toUtf8());
		file.write(QString("maxlev:%0\n").arg(QString::number(opt.atk.maxLev)).toUtf8());
		file.write(QString("statprc:%0\n").arg(QString::number(opt.atk.statPrc)).toUtf8());
		file.write(QString("checknpc:").append((options & FL_NONPC) ? "no" : "yes").append("\n").toUtf8());
		file.write(QString("trainpet:%0\n").arg(opt.bPet.train ? "yes" : "no").toUtf8());
		file.write(QString("tp-ruda:%0\n").arg(opt.bPet.useOre ? "yes" : "no").toUtf8());
		file.write(QString("tp-oil:%0\n").arg(opt.bPet.useOil ? "yes" : "no").toUtf8());
		file.write(QString("runner:%0\n").arg(opt.petRun ? "yes" : "no").toUtf8());
		file.write(QString("goldplay:").append(QString::number(goldType)).append("\n").toUtf8());
	}
}

void MWBotWin::apply() {
	options = 0;
	opt.atk.enabled = ui.cbAtack->isChecked() ? 1 : 0;
	opt.atk.typeA = ui.cbAtackType->itemData(ui.cbAtackType->currentIndex()).toInt();
	opt.atk.typeB = ui.cbAType2->itemData(ui.cbAType2->currentIndex()).toInt();
	goldType = ui.boxGypsy->itemData(ui.boxGypsy->currentIndex()).toInt();
	opt.petrik.make = ui.cbPetrik->isChecked() ? 1 : 0;
	opt.monya.play = ui.cbMonia->isChecked() ? 1 : 0;
	playSum = ui.sbMoniaCoins->value();
	opt.monya.tickets = ui.cbPlayTickets->isChecked() ? 1 : 0;
	opt.monya.buy = ui.cbBuyTickets->isChecked() ? 1 : 0;
	opt.monya.stars = ui.cbTicketStars->isChecked() ? 1 : 0;
	opt.ratk.enabled = ui.cbRatHunt->isChecked() ? 1 : 0;
	opt.ratk.maxlev = ui.sbRatMax->value();
	if (ui.cbDigger->isChecked()) options |= FL_DIG;
	if (ui.cbDigRat->isChecked()) options |= FL_DIGRAT;
	if (ui.cbRoll->isChecked()) options |= FL_KUB;
	buyCaps = ui.sbCaps->value();
	opt.atk.minLev = ui.sbMinLev->value();
	opt.atk.maxLev = ui.sbMaxLev->value();
	opt.atk.statPrc = ui.sbStatCheck->value();
	if (ui.cbNPCheck->isChecked()) options |= FL_NONPC;
	opt.bPet.train = ui.cbTrain->isChecked() ? 1 : 0;
	opt.bPet.useOre = ui.cbTrainRuda->isChecked() ? 1 : 0;
	opt.bPet.useOil = ui.cbTrainNeft->isChecked() ? 1 : 0;
	opt.petRun = ui.cbRunner->isChecked() ? 1 : 0;
	saveOpts();
}

void MWBotWin::setOpts() {
	ui.cbAtack->setChecked(opt.atk.enabled);
	ui.cbAtackType->setCurrentIndex(ui.cbAtackType->findData(opt.atk.typeA));
	ui.cbAType2->setCurrentIndex(ui.cbAType2->findData(opt.atk.typeB));
	ui.boxGypsy->setCurrentIndex(ui.boxGypsy->findData(goldType));
	ui.cbPetrik->setChecked(opt.petrik.make);
	ui.cbMonia->setChecked(opt.monya.play);
	ui.sbMoniaCoins->setValue(playSum);
	ui.cbPlayTickets->setChecked(opt.monya.tickets);
	ui.cbBuyTickets->setChecked(opt.monya.buy);
	ui.cbTicketStars->setChecked(opt.monya.stars);
	ui.cbRatHunt->setChecked(opt.ratk.enabled);
	ui.cbDigger->setChecked(options & FL_DIG);
	ui.cbDigRat->setChecked(options & FL_DIGRAT);
	ui.cbRoll->setChecked(options & FL_KUB);
	ui.sbCaps->setValue(buyCaps);
	ui.sbMinLev->setValue(opt.atk.minLev);
	ui.sbMaxLev->setValue(opt.atk.maxLev);
	ui.sbStatCheck->setValue(opt.atk.statPrc);
	ui.cbNPCheck->setChecked(options & FL_NONPC);
	ui.cbTrain->setChecked(opt.bPet.train);
	ui.cbTrainRuda->setChecked(opt.bPet.useOre);
	ui.cbTrainNeft->setChecked(opt.bPet.useOil);
	ui.cbRunner->setChecked(opt.petRun);
}

int main(int ac,char** av) {
	app = new QApplication(ac,av);
	MWBotWin mwbot;
	mwbot.loadOpts();
	mwbot.show();
	mwbot.loadCookies();
	mwbot.loadPage("player");
	mwbot.startTimer(2000);
	app->exec();
	mwbot.saveCookies();
	return 0;
}
