#include "main.h"

QList<mwItem> oilGameItems(QWebFrame* frm) {
	QWebElement elm = frm->findFirstElement("div.alert.alert1 div.neft-award");
	QList<mwItem> res;
	if (!eVisible(elm)) return res;
	mwItem obj;
	QWebElement e = elm.firstChild();
	while (eVisible(e)) {
		if (e.tagName() == "span") {
			if (e.classes().contains("object-thumb")) {
				obj.name = e.findFirst("img").attribute("alt");
				obj.count = e.findFirst("span.count").toPlainText().remove(QRegExp("[#,\"]")).trimmed().toInt();
			} else {
				obj.name = e.classes().first();
				obj.count = e.toPlainText().remove(QRegExp("[,#\"]")).trimmed().toInt();
			}
			if (!obj.name.isEmpty()) {
				if (obj.count == 0) obj.count++;
				res.append(obj);
			}
		}
		e = e.nextSibling();
	}
	return res;
}

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

// input:
// < 0 : lose - take susp from left field
// = 0 : win - susp = 0
// > 0 : escape, susp = input
void MWBotWin::oilGameEscape(int result) {
	// qDebug() << "oil game escape";
	QWebElement tlm;
	FightBox res;

	if (result < 0) {			// lose
		tlm = frm->findFirstElement("div#neftlenin_alert_mission div.content-block div.start-block span.counter");
		result = tlm.toPlainText().toInt();
		if (result < 1) result = 60;
		log(trUtf8("Игра на нефтепроводе проиграна"),"neft.png");
	} else if (result == 0) {		// win
		log(trUtf8("Игра на нефтепроводе выиграна"),"neft.png");
		tlm = frm->findFirstElement("div#neftlenin_alert_mission div.content-block div.actions button.button div.c");
		click(ui.browser, tlm);
		res.result = 4;
		res.items = oilGameItems(frm);
		click(ui.browser, "div.alert.alert1 div.neft-award div.actions div.button div.c");
		tlm = frm->findFirstElement("div#neftlenin_alert_mission div.content-block div.actions button.button div.c");
		click(ui.browser, tlm);
		logResult(res);
	} else {				// escape
		log(trUtf8("Побег с игры на нефтепроводе"),"neft.png");
	}

	tlm = frm->findFirstElement("div.pipeline-actions table td.mc div.progress i.counter");		// текущая подозрительность
	int susp = tlm.toPlainText().toInt();
	if ((result > 0) && !checkSusp(susp, result)) {
		tlm = frm->findFirstElement("div#neftlenin_alert_mission div.content-block div.actions button.button div.c");
		click(ui.browser, tlm);
		log(trUtf8("Уходим от патруля за %0 подозрительности").arg(result),"neft.png");
		pause(2);
	}
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
	int lose;
	int type;
	int work;
	int step;
	int idx;
	QWebElement elm,blk,tlm;
	curTime = QDateTime::currentDateTime();
	getFastRes();
	if (info.level < 10) {
		opt.oil.time = curTime.addDays(1);
		log(trUtf8("Нефтепровод доступен только с 10 уровня"),"neft.png");
	} else {
		restoreHP();
		loadPath("tverskaya:neftlenin");

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
				elm = frm->findFirstElement("div.alert.infoalert div.data div.actions");
				if (eVisible(elm)) {
					if (false) {		// TODO: true on dark oilpipe
						blk = elm.findAll("div.button").first();
					} else {
						blk = elm.findAll("div.button").last();
					}
					blk = blk.findFirst("span.f div.c");
					click(ui.browser, blk);
				}

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
						if (!checkSusp(susp, 30)) {
							elm = frm->findFirstElement("div#pipeline-scroll div.enemy-place.fight div.action button.button div.c");
							click(ui.browser, elm);
						}
						break;
					case preGame:			// start event
						elm = frm->findFirstElement("div#pipeline-scroll div.enemy-place.mission div.action button.button div.c");
						click(ui.browser, elm);
						break;
					case preBoss:			// start boss
						if (!checkSusp(susp, 30)) {
							elm = frm->findFirstElement("div#pipeline-scroll div.enemy-place.fightboss div.action button.button div.c");
							click(ui.browser, elm);
						}
						break;
					case evDuel:			// go duel
						elm = frm->findFirstElement("div#neftlenin_alert_d div.action button.button.first div.c");			// кнопка старта
						need = elm.findFirst("span.suspicion").toPlainText().remove("\"").toInt();					// надо подозрительности
						if (!checkSusp(susp, need)) {
							click(ui.browser, elm);
							fightResult();
						}
						break;
					case evGroup:			// go group fight
						elm = frm->findFirstElement("div#neftlenin_alert_g div.action button.button.first div.c");
						need = elm.findFirst("span.suspicion").toPlainText().remove("\"").toInt();
						if (!checkSusp(susp, need)) {
							click(ui.browser, elm);
							groupFight();
						}
						break;
					case evBoss:			// go boss
						elm = frm->findFirstElement("div#neftlenin_alert_b div.action.buttion.button.first div.c");
						need = elm.findFirst("span.suspicion").toPlainText().remove("\"").toInt();
						if (!checkSusp(susp, need)) {
							click(ui.browser, elm);
							groupFight();
						}
						break;
					case evGamePrepare:		// start event timer
						elm = frm->findFirstElement("div#neftlenin_alert_prem_first div.action button.button div.c");
						click(ui.browser, elm);
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
						step = 1;
						while (work) {
							printf("step %i work %i\n",step,work);
							if (step > 4) {								// win
								oilGameEscape(0);
								work = 0;
							} else {
								printf("elm : %i\n",eVisible(elm));
								blk = elm.findFirst("div.step-block.active");
								printf("blk : %i\n",eVisible(blk));
								if (!eVisible(blk)) {
									printf("Active dice not found\n");
									oilGameEscape(-1);
									work = 0;
								} else {
									tlm = blk.findFirst("div.enemy-dice span.dice");			// кубик противника
									printf("tlm : %i\n",eVisible(tlm));
									need = tlm.classes().filter("dice-").first().remove("dice-").toInt();	// число на кубике
									printf("кубик противника %i\n",need);
									tlm = blk.findFirst("div.block-wrapper div.counter");
									lose = tlm.toPlainText().toInt();					// штраф за сбегание
									printf("штраф %i\n",lose);
									// need = 0;
									if (need == 0) {
										qDebug() << "some error";
										log(trUtf8("Ошибка: кубик врага == 0", "bug.png"));
										work = 0;
										opt.oil.time = curTime.addSecs(1800);
									} else if (need <= opt.oil.diceMax) {		// пробуем обыграть
										tlm = blk.findFirst("div.action button.button div.c");
										click(ui.browser, tlm);
										tlm = blk.findFirst("div.player-dice span.dice");			// кубик игрока
										lose = tlm.classes().filter("dice-").first().remove("dice-").toInt();	// число на кубике
										if (lose < need) {			// проиграл
											oilGameEscape(-1);
											work = 0;
										} else {				// выиграл - переход к следующему шагу
											step++;
										}
									} else {					// убегаем
										printf("escape\n");
										oilGameEscape(lose);
										work = 0;
									}
								}
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
