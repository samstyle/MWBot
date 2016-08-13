#include "main.h"

int MWBotWin::checkSusp(int susp, int need) {
	if (susp + need > 150) {
		need = susp + need - 150;
		opt.oil.time = curTime.addSecs(need * 60);
		log(trUtf8("До снижения подозрительности %0 мин").arg(need),"neft.png");
	} else {
		need = 0;
	}
	return need;
}

int MWBotWin::oilGameEscape() {
	qDebug() << "oil game escape";
	QWebElement blk, tlm;
	blk = frm->findFirstElement("div#neftlenin_alert_mission div.content-block div.actions button.button div.c");
	tlm = blk.findFirst("span.suspicion span.price_escape");
	int need = 0;
	if (!tlm.isNull()) {
		need = tlm.toPlainText().toInt();
	}
	qDebug() << "need" << need << "susp";

	if (need == 0) {
		log(trUtf8("Патруль пройден успешно"),"neft.png");
		fightResult();
		clickElement(blk);
	} else {
		tlm = frm->findFirstElement("div.pipeline-actions table td.mc div.progress i.counter");
		int susp = tlm.toPlainText().toInt();

		if (!checkSusp(susp, need)) {
			clickElement(blk);
			log(trUtf8("Уходим от патруля за %0 подозрительности").arg(need),"neft.png");
			doPause(2);
		}
	}
	return need;
}

typedef struct {
	int type;
	QString query;
} xTypeScan;

enum {
	evNone = 0,
	preFight,
	preGame,
	preBoss,
	evDuel,
	evGroup,
	evBoss,
	evGamePrepare,
	evGameTimer,
	evGame
};

xTypeScan oilType[] = {
	{evGroup,"div#neftlenin_alert_g"},				// group fight
	{evDuel,"div#neftlenin_alert_d"},				// single fight
	{evBoss,"div#neftlenin_alert_b"},				// boss fight

	{evGame,"div#neftlenin_alert_mission"},				// cube game
	{evGameTimer,"div#neftlenin_alert_prem"},			// timer before cube game
	{evGamePrepare,"div#neftlenin_alert_prem_first"},		// cube game prepare

	{preBoss,"div#pipeline-scroll div.enemy-place.fightboss"},	// boss prep
	{preFight,"div#pipeline-scroll div.enemy-place.fight"},		// fight prep
	{preGame,"div#pipeline-scroll div.enemy-place.mission"},	// game prep

	{evNone,""}
};

void MWBotWin::atackOil() {
	setBusy(true);
	int time;
	int susp;
	int need;
	int type;
	int work;
	int idx;
	QWebElement elm,blk,tlm;
	curTime = QDateTime::currentDateTime();
	getFastRes();
	if (info.level < 10) {
		opt.oil.time = curTime.addDays(1);
		log(trUtf8("Нефтепровод доступен только с 10 уровня"),"neft.png");
	} else {
		restoreHP();
		loadPath(QStringList() << "tverskaya" << "neftlenin");

		while (opt.oil.time < curTime) {
			restoreHP();
			curTime = QDateTime::currentDateTime();
			elm = frm->findFirstElement("div#neftlenin_alert_win");
			if (elm.styleProperty("display",QWebElement::ComputedStyle).trimmed() != "none") {	// already win
				elm = elm.findFirst("div.object div.data span.restartNeft");
				time = elm.attribute("timer").toInt() + 60 + (random() % 30);
				opt.oil.time = QDateTime::currentDateTime().addSecs(time);
				log(trUtf8("Нефтепровод будет доступен через %0 сек").arg(time),"neft.png");
			} else {
				// current suspicion
				tlm = frm->findFirstElement("div.pipeline-actions table td.mc div.progress i.counter");
				susp = tlm.toPlainText().toInt();

				// detect event type
				idx = 0;
				type = -1;
				do {
					if (oilType[idx].type == 0) {
						type = 0;
					} else {
						elm = frm->findFirstElement(oilType[idx].query);
						if (!elm.isNull() && (elm.styleProperty("display",QWebElement::ComputedStyle).trimmed() != "none")) {
							type = oilType[idx].type;
						}
					}
					idx++;
				} while (type < 0);

				// qDebug() << type;

				switch (type) {
					case preFight:			// start fight
						elm = frm->findFirstElement("div#pipeline-scroll div.enemy-place.fight div.action button.button div.c");
						clickElement(elm);
						break;
					case preGame:			// start event
						elm = frm->findFirstElement("div#pipeline-scroll div.enemy-place.mission div.action button.button div.c");
						clickElement(elm);
						break;
					case preBoss:			// start boss
						elm = frm->findFirstElement("div#pipeline-scroll div.enemy-place.fightboss div.action button.button div.c");
						clickElement(elm);
						break;
					case evDuel:			// go duel
						elm = frm->findFirstElement("div#neftlenin_alert_d div.action button.button.first div.c");			// кнопка старта
						need = elm.findFirst("span.suspicion").toPlainText().remove("\"").toInt();					// надо подозрительности
						if (!checkSusp(susp, need)) {
							clickElement(elm);
							fightResult();
						}
						break;
					case evGroup:			// go group fight
						elm = frm->findFirstElement("div#neftlenin_alert_g div.action button.button.first div.c");
						need = elm.findFirst("span.suspicion").toPlainText().remove("\"").toInt();
						if (!checkSusp(susp, need)) {
							clickElement(elm);
							groupFight();
						}
						break;
					case evBoss:			// go boss
						elm = frm->findFirstElement("div#neftlenin_alert_b div.action.buttion.button.first div.c");
						need = elm.findFirst("span.suspicion").toPlainText().remove("\"").toInt();
						if (!checkSusp(susp, need)) {
							clickElement(elm);
							groupFight();
						}
						break;
					case evGamePrepare:		// start event timer
						elm = frm->findFirstElement("div#neftlenin_alert_prem_first div.action button.button div.c");
						clickElement(elm);
						break;
					case evGameTimer:		// event timer
						elm = frm->findFirstElement("div#neftlenin_alert_prem div.progress-wrapper span.timeleft");
						need = elm.attribute("timer").toInt() + 15 + random() % 15;
						opt.oil.time = curTime.addSecs(need);
						log(trUtf8("До завершения задания %0 сек").arg(need),"neft.png");
						break;
					case evGame:			// event game
						log(trUtf8("Начало игры на нефтепроводе"),"neft.png");
						elm = frm->findFirstElement("div#neftlenin_alert_mission div.game-process div.progress-block");
						work = 1;
						while (work) {
							blk = elm.findFirst("div.step-block.active");		// блок с кубиками
							if (blk.isNull()) {
								need = oilGameEscape();			// проигрыш или выигрыш
								work = 0;
							} else {
								tlm = blk.findFirst("div.enemy-dice span.dice");	// текущий кубик
								need = tlm.classes().filter("dice-").first().remove("dice-").toInt();	// число на кубике
								//qDebug() << "cube" << need;
								if (need == 0) {
									qDebug() << "some error";
									work = 0;
									opt.oil.time.addMonths(1);
								} else if (need <= opt.oil.diceMax) {			// кликаем
									tlm = blk.findFirst("div.action button.button div.c");
									clickElement(tlm);
								} else {						// убегаем
									oilGameEscape();
									work = 0;
								}
								doPause(2);
							}
						}
						break;
					default:
						log(trUtf8("DEBUG : oilpipe unknown situation"),"bug.png");
						qDebug() << "unknown situation";
						opt.oil.time = curTime.addMonths(1);
						break;
				}
			}

		}
	}
	setBusy(false);
}
