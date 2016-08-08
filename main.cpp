#include <QtWebKit>
#include <QtGui>
#include <unistd.h>
#include <getopt.h>

#include "main.h"

QApplication* app;

MWBotWin::MWBotWin() {
	ui.setupUi(this);

	tedit = new QDialog(this);
	tui.setupUi(tedit);

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

	opt.oil.enable = 1;
	opt.oil.diceMax = 6;
	opt.oil.time = curTime;

	opt.chest.open = 1;
	opt.chest.keyOil = -1;
	opt.chest.keyRat = -1;

	opt.petrik.make = 1;
	opt.petrik.money = 0;
	opt.petrik.ore = 0;
	opt.petrik.time = curTime;

	opt.run.enabled = 1;
	opt.run.name.clear();
	opt.run.time = curTime;

	opt.kub.date = curTime.date().addDays(-1);

	opt.monya.play = 1;
	opt.monya.buy = 1;
	opt.monya.stars = 0;
	opt.monya.block = 0;
//	opt.monya.date = curTime.date();

	opt.bPet.train = 1;
	opt.bPet.useOre = 1;
	opt.bPet.useOil = 0;
	opt.bPet.money = 0;
	opt.bPet.ore = 0;
	opt.bPet.oil = 0;
	opt.bPet.time = curTime;

	opt.taxi.enable = 1;
	opt.taxi.time = curTime;

	state.stop = 0;
	state.botWork = 0;
	state.firstRun = 1;

	goldType = 250;

	setBusy(false);

//	ui.browser->setZoomFactor(0.75);

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

//	ui.boxGypsy->addItem(QIcon(":/images/gold.png"),"250",250);
//	ui.boxGypsy->addItem(QIcon(":/images/gold.png"),"750",750);
//	ui.boxGypsy->addItem(QIcon(":/images/gold.png"),"150 + 2",150);
//	ui.boxGypsy->setCurrentIndex(ui.boxGypsy->findData(goldType));

	connect(ui.pbOptSave,SIGNAL(clicked()),this,SLOT(apply()));

	connect(ui.browser,SIGNAL(loadStarted()),this,SLOT(onStart()));
	connect(ui.browser,SIGNAL(loadProgress(int)),ui.progress,SLOT(setValue(int)));
	connect(ui.browser,SIGNAL(loadFinished(bool)),this,SLOT(onLoad(bool)));
	connect(ui.browser,SIGNAL(titleChanged(QString)),this,SLOT(setWindowTitle(QString)));

//	connect(ui.tbStop,SIGNAL(released()),this,SLOT(stop()));
	connect(ui.tbStart,SIGNAL(released()),this,SLOT(start()));
	connect(ui.tbSave,SIGNAL(released()),this,SLOT(savePage()));

	connect(ui.tbGipsy,SIGNAL(clicked()),this,SLOT(gipsy()));
	connect(ui.tbAtack,SIGNAL(clicked()),this,SLOT(attack()));
	connect(ui.tbRat,SIGNAL(clicked()),this,SLOT(atkRat()));
	connect(ui.tbOil,SIGNAL(clicked()),this,SLOT(atackOil()));
	connect(ui.tbPetrik,SIGNAL(clicked()),this,SLOT(makePetrik()));
	connect(ui.tbSellLot,SIGNAL(clicked()),this,SLOT(sellLots()));
	connect(ui.tbThimble,SIGNAL(clicked()),this,SLOT(goMonia()));
	connect(ui.tbDig,SIGNAL(clicked()),this,SLOT(dig()));
	connect(ui.tbBaraban,SIGNAL(clicked()),this,SLOT(playKub()));
	connect(ui.tbTrainPet,SIGNAL(clicked()),this,SLOT(trainPet()));
	connect(ui.tbArena,SIGNAL(clicked()),this,SLOT(arena()));
	connect(ui.tbTaxi,SIGNAL(clicked()),this,SLOT(doTaxi()));

	connect(ui.zoomSlider,SIGNAL(valueChanged(int)),this,SLOT(chZoom(int)));

	connect(ui.tbCheeseList,SIGNAL(clicked(bool)),this,SLOT(editCheese()));
	connect(ui.tbHealList,SIGNAL(clicked(bool)),this,SLOT(editHeal()));
	connect(tui.okButton,SIGNAL(clicked(bool)),this,SLOT(setList()));
	connect(tui.cancelButton,SIGNAL(clicked(bool)),tedit,SLOT(hide()));

	ui.zoomSlider->setValue(75);

}

void MWBotWin::chZoom(int zoom) {
	if (zoom < 20) return;
	if (zoom > 100) return;
	ui.browser->setZoomFactor(zoom/100.0);
}

void MWBotWin::prepare() {
	loadOpts();
	show();
	loadCookies();
	loadPage("player");
	timerId = startTimer(2000);
}

void MWBotWin::closeEvent(QCloseEvent* ev) {
	state.stop = 1;
	killTimer(timerId);
	waitLoading();
	ev->accept();
}

void MWBotWin::keyPressEvent(QKeyEvent* ev) {
	switch(ev->key()) {
		case Qt::Key_F5:
			ui.browser->reload();
			break;
		case Qt::Key_F2:
			savePage();
			break;
		case Qt::Key_F12:
			debug();
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

void MWBotWin::timerEvent(QTimerEvent* ev) {
	curTime = QDateTime::currentDateTime();
	ev->accept();
	if (!state.botWork) return;
	if (state.loading) return;
	if (state.busy) return;

	QWebElement elm;
	checkPolice();

	if (state.firstRun) {
		getAtackTimer();
		state.firstRun = 0;
	}

	getFastRes();
// open chests
	if (opt.chest.open) {
		openChests();
	}

// train battle pet
	if (opt.bPet.train && \
		(info.money >= opt.bPet.money + 200) && \
		(info.ore >= opt.bPet.ore) && \
		(info.oil >= opt.bPet.oil) && \
		(curTime > opt.bPet.time)) {
		trainPet();
	}
// send pet to arena
	if (opt.run.enabled && (opt.run.time < curTime)) {
		arena();
	}
// play baraban
	if (opt.kub.play && (opt.kub.date < QDate::currentDate())) {
		elm = frm->findFirstElement("div.side-fractionwar");
		if (!elm.isNull()) {
			if (elm.toPlainText().contains(trUtf8("Приз в студию"))) {
				playKub();
			}
		}
	}
// attack
	if (opt.atk.time < curTime) {
		if (opt.oil.enable && (opt.oil.time < curTime)) {
			atackOil();
		}
		if (opt.ratk.enabled && (opt.ratk.time < curTime) && (opt.ratk.ratlev < opt.ratk.maxlev)) {
			atkRat();
		}
		if (opt.atk.enabled) {
			attack();
		}
	}
// make petriks
		if (opt.petrik.make && \
			(info.money > (opt.petrik.money + 200)) && \
			(info.ore > opt.petrik.ore) && \
			(opt.petrik.time < curTime)) {
			makePetrik();
		}
// play with monya
	getFastRes();
	if (!opt.monya.block && opt.monya.play && (info.money > opt.monya.minPlaySum)) {
		if (info.money < opt.monya.maxPlaySum) {
			goMonia();
		} else {
			goBankChange();
		}
	}
// taxi
	if (opt.taxi.enable && (curTime > opt.taxi.time) && (curTime.date().dayOfWeek() == 1)) {
		doTaxi();
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
	log(trUtf8("Копаем в метро, до окончания %0 сек").arg(sec));
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
	state.busy = bsy ? 1 : 0;
}

void MWBotWin::onLoad(bool) {
	state.loading = 0;
}

void MWBotWin::onStart() {
	state.loading = 1;
}

void MWBotWin::start() {
	if (state.botWork) {
		log(trUtf8("Бот остановлен"),"stop.png");
		flag = 0;
		state.botWork = 0;
		ui.tbStart->setIcon(QIcon(":/images/start.png"));
	} else {
		log(trUtf8("Бот запущен"),"start.png");
		if (!ui.browser->isEnabled()) flag |= FL_STOP;
		state.botWork = 1;
		ui.tbStart->setIcon(QIcon(":/images/stop.png"));
	}

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
	if (pth.size() == 0) {
		ui.browser->reload();
	} else {
		for (i = 0; (i < pth.size()) && res; i++)
			res &= loadPage(pth.at(i));
	}
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

QString clickq = "\
var evt=document.createEvent('MouseEvents');\
evt.initMouseEvent('click',true,true,window,0,0,0,0,0,false,false,false,false,0,null);\
var elst = document.querySelectorAll('[mustBeClicked]');\
if (elst.length > 0) {elst[0].dispatchEvent(evt);}\
";

void MWBotWin::clickElement(QWebElement& elm, int speed) {
	QString quer = clickq;
	elm.setAttribute("mustBeClicked","1");
	frm->evaluateJavaScript(quer);
	elm.removeAttribute("mustBeClicked");
	waitLoading(speed);
}

void MWBotWin::clickElement(QString quer, int speed) {
	QWebElement elm = frm->findFirstElement(quer);
	if (!elm.isNull()) {
		clickElement(elm, speed);
	} else {
		qDebug() << trUtf8("элемент '%0' не найден").arg(quer);
		log(trUtf8("DEBUG: элемент <b>%0</b> не найден").arg(quer),"bug.png");
	}
}

void MWBotWin::waitLoading(int speed) {
	if (speed < 0) speed = 1000;
	QWebElement elm;
	do {
		usleep(1000);
		app->processEvents();
		elm = frm->findFirstElement("div.loading-top");
		if (state.stop) break;
	} while (state.loading || !elm.attribute("style").contains("none"));
	if (state.stop) return;
	int sleeptime = 500 + (rand() % 1000);
	while (sleeptime > 0) {
		usleep(speed);
		app->processEvents();
		sleeptime--;
	}
}

void MWBotWin::doPause(int sec) {
	int i;
	while (sec > 0) {
		for (i = 0; i < 20; i++) {
			usleep(5e4);		// 0.05 sec
			app->processEvents();
		}
		sec--;
	}
}

// get numbers

int MWBotWin::getAtackTimer() {
	int res;
	loadPage("alley");
	QWebElement elm = frm->findFirstElement("div.need-some-rest div.holders span.timer");
	opt.atk.time = QDateTime::currentDateTime();
	if (elm.isNull()) {
		res = -1;
	} else {
		res = elm.attribute("timer").toInt();
		if (res > 0) {
			res += (5 + (rand() % 10));
			opt.atk.time = opt.atk.time.addSecs(res);
			log(trUtf8("%0 сек. до следующего нападения").arg(res),"punch.png");
		} else {
			res = 0;
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

	{QObject::trUtf8("expa"),":/images/lamp.png",0},
	{QObject::trUtf8("tugriki"),":/images/money.png",0},
	{QObject::trUtf8("монеты"),":/images/money.png",0},
	{QObject::trUtf8("ruda"),":/images/ruda.png",0},
	{QObject::trUtf8("neft"),":/images/neft.png",0},
	{QObject::trUtf8("star"),":/images/star.png",0},
	{QObject::trUtf8("petric"),":/images/petrik.png",0},
	{QObject::trUtf8("tooth-white"),":/images/tooth.png",0},
	{QObject::trUtf8("tooth-golden"),":/images/toothGold.png",0},
	{QObject::trUtf8("bullet"),":/images/bullet.png",0},
	{QObject::trUtf8("sparkles"),":/images/sparkle.png",0},
	{QObject::trUtf8("snowflake"),":/images/snow.png",0},
	{QObject::trUtf8("badge"),":/images/badge.png",0},
	{QObject::trUtf8("mobila"),":/images/mobila.png",0},
	{QObject::trUtf8("party_signature"),":/images/party-signature.png",0},
	{QObject::trUtf8("anabolic"),":/images/anabolic.png",0},
	{QObject::trUtf8("партбилет"),":/images/partbilet.png",0},

	{QObject::trUtf8("хвост крысомахи"),":/images/ratTail.png",0},
	{QObject::trUtf8("рубль"),":/images/rubel.png",0},

	{QObject::trUtf8("малая шкатулка партии"),":/images/boxes/box33.png",0},
	{QObject::trUtf8("средняя шкатулка партии"),":/images/boxes/box34.png",0},
	{QObject::trUtf8("большая шкатулка партии"),":/images/boxes/box35.png",0},

	{QObject::trUtf8("малый ларец"),":/images/boxes/box_metro1.png",0},
	{QObject::trUtf8("средний ларец"),":/images/boxes/box_metro2.png",0},
	{QObject::trUtf8("большой ларец"),":/images/boxes/box_metro3.png",0},
	{QObject::trUtf8("ключ от шахтерского ларца"),":/images/boxes/box_metro_key.png",0},
	{QObject::trUtf8("шкатулка с нашивками"),":/images/boxes/box_stitch.png",0},

	{QObject::trUtf8("праймари пасс"),":/images/aroundworld/pass.png",0},
	{QObject::trUtf8("дорожные чеки"),":/images/aroundworld/magnet.png",0},
	{QObject::trUtf8("посадочный билет"),":/images/aroundworld/ticket.png",0},

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

void MWBotWin::getResources() {
	getFastRes();
	loadPage("player");
	QWebElement elm,subelm;
	int id,count;
	QList<QWebElement> lst = frm->findAllElements("table.inventary td.equipment-cell div.object-thumb div.padding").toList();
	info.resMap.clear();
	foreach(elm, lst) {
		subelm = elm.findFirst("img");
		id = subelm.attribute("data-st").toInt();
		count = elm.findFirst("div.count").toPlainText().remove("#").toInt();
		if (count == 0) count = 1;
		info.resMap[id] = count;
	}
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
	info.level = frm->findFirstElement("div#personal a.name b").toPlainText().split("[").last().split("]").first().toInt();
}

void MWBotWin::getBerezkaRes() {
	info.tooth = frm->findFirstElement("div.borderdata span.tooth-white").toPlainText().remove(",").toInt();
	info.goldtooth = frm->findFirstElement("div.borderdata span.tooth-golden").toPlainText().remove(",").toInt();
	info.star = frm->findFirstElement("div.borderdata span.star").toPlainText().remove(",").toInt();
	info.badge = frm->findFirstElement("div.borderdata span.badge").toPlainText().remove(",").toInt();
	info.mobile = frm->findFirstElement("div.borderdata span.mobila").toPlainText().remove(",").toInt();
	info.oil = frm->findFirstElement("div.borderdata span.neft").toPlainText().remove(",").toInt();
	//res.ipoints = frm->findFirstElement("div.borderdata span.ipoints-e").toPlainText().toInt();
	info.power = frm->findFirstElement("div.borderdata span.power.counter").toPlainText().remove(",").toInt();
	info.medal = frm->findFirstElement("div.borderdata span.pet-golden.counter").toPlainText().remove(",").toInt();
	info.honey = frm->findFirstElement("div.borderdata span.med").toPlainText().remove(",").toInt();
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
		clickElement("div.alert div.data div.actions button.button div.c");
	}
}

// click select lev-1..lev+1 in alley


// get results



// make petriks

void MWBotWin::makePetrik() {
	QWebElement elm;
	setBusy(true);
	loadPath(QStringList() << "square" << "factory");
	getFastRes();
	curTime = QDateTime::currentDateTime();
	int loop = 1;
	int time;
	do {
		elm = frm->findFirstElement("form.factory-nanoptric button.button div.c");
		if (elm.isNull()) {
			elm = frm->findFirstElement("span#petriksprocess");
			if (elm.isNull()) {
				if (loop < 2) {
					ui.browser->reload();
					waitLoading();
					loop++;
				} else {
					log(trUtf8("Ошибка варки петриков. Следующая попытка через минуту."),"petrik.png");
					opt.petrik.time = curTime.addSecs(60 + (random() % 30));
					loop = 0;
				}
			} else {
				time = elm.attribute("timer").toInt() + 60 + (random() % 30);
				opt.petrik.time = curTime.addSecs(time);
				if (loop < 2) {
					log(trUtf8("Петрики уже варятся, до окончания %0 минут").arg(time / 60 + 1),"petrik.png");
				}
				loop = 0;
			}
		} else {
			opt.petrik.money = elm.findFirst("span.tugriki").toPlainText().remove(",").toInt();
			opt.petrik.ore = elm.findFirst("span.ruda").toPlainText().remove(",").toInt();
			getFastRes();
			if ((info.money > (opt.petrik.money + 199)) && (info.ore >= opt.petrik.ore)) {
				clickElement(elm);
				log(trUtf8("Запущено производство петриков"),"petrik.png");
				loop = 2;
			} else {
				log(trUtf8("Для производства петриков недостаточно ресурсов"),"petrik.png");
				loop = 0;
			}
		}
	} while (loop);
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

// edit lists

void MWBotWin::editCheese() {
	editList = &cheeseList;
	tui.textArea->setPlainText(editList->join("\n"));
	tedit->show();
}

void MWBotWin::editHeal() {
	editList = &healList;
	tui.textArea->setPlainText(editList->join("\n"));
	tedit->show();
}

void MWBotWin::setList() {
	*editList = tui.textArea->toPlainText().split("\n",QString::SkipEmptyParts);
	tedit->hide();
	saveOpts();
}

// main

int main(int ac,char** av) {
	app = new QApplication(ac,av);
	MWBotWin mwbot;
	mwbot.prepare();
	app->exec();
	mwbot.saveCookies();
	return 0;
}

// debug

void MWBotWin::debug() {
	openChests();
}
