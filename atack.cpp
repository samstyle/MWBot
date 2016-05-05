#include "main.h"

#define	AT_OK	0	// ok
#define	AT_NF	1	// not found
#define	AT_BUSY	2	// player is busy
#define	AT_RTRY	3	// retry this victim
#define	AT_STOP	4	// stop pressed
#define	AT_ERR	255	// other error

void MWBotWin::attack() {
	setBusy(true);
	loadPage("alley/");
	int tim = getAtackTimer();
	bool at = true;
	int asel;
	CharBox enstat;
	QWebElement elm;
	if (tim < 1) {
		do {
			at = true;
			do {
				restoreHP();
				asel = atkSelect(at ? opt.atk.typeA : opt.atk.typeB);			// alley : search enemy
				switch (asel) {
					case AT_NF:
						at = !at;
						break;
					case AT_BUSY:
						log(trUtf8("Вы заняты и не можете атаковать"));
						asel = AT_STOP;
						flag |= FL_STOP;
						break;
				}
			} while ((asel != AT_OK) && (asel != AT_STOP));
			if (flag & FL_STOP) {
				tim = AT_STOP;
				break;
			}
			tim = atkCheck(enstat, at ? opt.atk.typeA : opt.atk.typeB);			// AT_OK:victim found; AT_ERR:error; AT_STOP:stop
			if (tim == AT_OK) {
				elm = frm->findFirstElement("div#content div.report div.red");
				if (!elm.isNull()) {
					loadPage("alley");
					tim = AT_ERR;
				} else {
					clickElement("div.button.button-fight div.c");				// click atack
					do {
						// if (flag & FL_STOP) {tim = 2; break;}
						elm = frm->findFirstElement("div#content div.report div.red");
						if (elm.isNull()) {
							tim = atkResult() ? AT_OK : AT_RTRY;
						} else {
							loadPage("alley");
							tim = AT_ERR;
						}
					} while (tim == AT_RTRY);
				}
			}
		} while (tim == AT_ERR);
	}
	flag &= ~FL_STOP;
	setBusy(false);
}

// select enemy for atack
// return AT_OK,AT_NF,AT_BUSY,AT_ERR
int MWBotWin::atkSelect(int type) {
	if (flag & FL_STOP) return AT_STOP;
	QWebElement elm;
	QString atkId;
	int brk = 0;
	switch (type) {
		case ATK_WEAK:
			clickElement("form.search-alley div.button-big.btn.f1");
			break;
		case ATK_EQUAL:
			clickElement("form.search-alley div.button-big.btn.f2");
			break;
		case ATK_STRONG:
			clickElement("form.search-alley div.button-big.btn.f3");
			break;
		case ATK_ENEMY:
			brk = 1;
			break;
		case ATK_VICTIM:
			brk = 1;
			break;
		default:
			elm = frm->findFirstElement("input[name=minlevel]");
			elm.setAttribute("value",QString::number(opt.atk.minLev));
			elm = frm->findFirstElement("input[name=maxlevel]");
			elm.setAttribute("value",QString::number(opt.atk.maxLev));
			clickElement("form#searchLevelForm div.button div.c");
			break;
	}
	if (brk) return AT_ERR;
	elm = frm->findFirstElement("div#alley-search-myself div.error");
	if (!elm.isNull()) return AT_ERR;
	elm = frm->findFirstElement("div#alley-search-myself p.error");
	if (elm.isNull()) return AT_OK;
	atkId = elm.toPlainText();
	if (atkId.contains(trUtf8("чучело"))) return AT_NF;
	if (atkId.contains(trUtf8("Вы заняты"))) return AT_BUSY;
	return AT_ERR;
}

// check enemy stats or press "next" button
// return AT_OK,AT_ERR,AT_STOP
int MWBotWin::atkCheck(CharBox& enstat, int type) {
	int res = -1;
	bool checkLev = false;
	CharBox mystat = getStat("div.fighter1","td.fighter1-cell");
	int levLow = mystat.level + opt.atk.minLev;
	int levHi = mystat.level + opt.atk.maxLev;
	QWebElement elm;
	do {
		if (flag & FL_STOP) {
			res = AT_STOP;										// stop
		} else {
			enstat = getStat("div.fighter2","td.fighter2-cell");
			if (enstat.name.contains(QDialog::trUtf8("Агент госдепа"))) {
				if ((mystat.statsum * 1.15) > enstat.statsum) res = AT_OK;			// agent : ok
			} else {
				checkLev = ((enstat.level >= levLow) && (enstat.level <= levHi));
				if ((type == ATK_ENEMY) || (type == ATK_VICTIM)) checkLev = true;
				if (checkLev && ((mystat.statsum * opt.atk.statPrc) > enstat.statsum)) res = AT_OK;	// player : ok
			}
			if (opt.atk.droped && (enstat.type != "npc")) res = -1;
		}
		if (res < 0) {
			clickElement("div.button.button-search div.c");
			elm = frm->findFirstElement("div#alley-search-myself p.error");
			if (!elm.isNull()) res = AT_ERR;							// error
		}
	} while (res < 0);
	return res;
}
