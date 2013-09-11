#include "main.h"

#define	AT_OK	0	// ok
#define	AT_NF	1	// not found
#define	AT_BUSY	2	// player is busy
#define	AT_RTRY	3	// retry this victim
#define	AT_STOP	4	// stop pressed
#define	AT_ERR	255	// other error

void MWBotWin::atack() {
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
				asel = atackSel(at ? atackType : atackType2);			// alley : search enemy
				switch (asel) {
					case AT_NF:
						at = !at;
						break;
					case AT_BUSY:
						log(trUtf8("Вы заняты и не можете атаковать"));
						flag |= FL_STOP;
						break;
				}
			} while ((asel != AT_OK) || (~flag & FL_STOP));
			if (flag & FL_STOP) {
				tim = 2;
				break;
			}
			tim = atackCheck(enstat, at ? atackType : atackType2);			// AT_OK:victim found; AT_ERR:error; AT_STOP:stop
			if (tim == AT_OK) {
				clickElement("div.button.button-fight div.c",0);		// click atack
				do {
					// if (flag & FL_STOP) {tim = 2; break;}
					elm = frm->findFirstElement("div#content div.report div.red");
					if (elm.isNull()) {
						tim = atackResult() ? AT_OK : AT_RTRY;
					} else {
						if (elm.toPlainText().contains(trUtf8("недавно нападали"))) {
							clickElement("div#content table.buttons div.button div.c",0);
							tim = AT_RTRY;			// try atack him again
						}
						if (elm.toPlainText().contains(trUtf8("менее 35% жизней"))) {
							loadPage("/alley/");
							tim = AT_ERR;			// enemy is out, try another one
						}
						if (elm.toPlainText().contains(trUtf8("Вы слишком слабы"))) {
							loadPage("/alley/");
							tim = AT_ERR;			// HP is low, try again
						}
					}
				} while (tim == AT_RTRY);
			}
		} while (tim == AT_ERR);
	}
	flag &= ~FL_STOP;
	setBusy(false);
}

// select enemy for atack
// return AT_OK,AT_NF,AT_BUSY,AT_ERR
int MWBotWin::atackSel(int type) {
	if (flag & FL_STOP) return AT_STOP;
	QWebElement elm;
	QString atkId;
	switch (type) {
		case ATACK_WEAK: atkId = "weak"; break;
		case ATACK_EQUAL: atkId = "equal"; break;
		case ATACK_STRONG: atkId = "strong"; break;
		case ATACK_ENEMY: atkId = "enemy"; break;
		case ATACK_VICTIM: atkId = "victim"; break;
	}
	if (!atkId.isEmpty()) {
		foreach (elm, frm->findAllElements("form#searchForm div.opponent select[name=type] option"))
			elm.removeAttribute("selected");
		elm = frm->findFirstElement(QString("form#searchForm div.opponent select[name=type] option[value=").append(atkId).append("]"));
		elm.setAttribute("selected","");
		clickElement("form#searchForm div#search-enemy-btn div.c",0);
	} else {
		//elm = frm->findFirstElement("div#personal a.name b");
		//int lev = elm.toPlainText().remove("]").split("[").last().toInt();
		elm = frm->findFirstElement("input[name=minlevel]");
		elm.setAttribute("value",QString::number(minLev));
		elm = frm->findFirstElement("input[name=maxlevel]");
		elm.setAttribute("value",QString::number(maxLev));
		clickElement("form#searchLevelForm div.button div.c",0);
	}
	elm = frm->findFirstElement("div#alley-search-myself p.error");
	if (elm.isNull()) return AT_OK;
	atkId = elm.toPlainText();
	if (atkId.contains(trUtf8("чучело"))) return AT_NF;
	if (atkId.contains(trUtf8("Вы заняты"))) return AT_BUSY;
	return AT_ERR;
}

// check enemy stats or press "next" button
// return AT_OK,AT_ERR,AT_STOP
int MWBotWin::atackCheck(CharBox& enstat, int type) {
	int res = -1;
	bool checkLev = false;
	CharBox mystat = getStat("div.fighter1","td.fighter1-cell");
	QWebElement elm;
	do {
		if (flag & FL_STOP) {
			res = AT_STOP;										// stop
		} else {
			enstat = getStat("div.fighter2","td.fighter2-cell");
			if (enstat.name.contains(QDialog::trUtf8("Агент госдепа"))) {
				if ((mystat.statsum * 1.15) > enstat.statsum) res = AT_OK;			// agent : ok
			} else {
				checkLev = ((enstat.level >= minLev) && (enstat.level <= maxLev));
				if ((type == ATACK_ENEMY) || (type == ATACK_VICTIM)) checkLev = true;
				if (checkLev && ((mystat.statsum * statPrc) > enstat.statsum)) res = AT_OK;	// player : ok
			}
		}
		if (res < 0) {
			clickElement("div.button.button-search div.c",100);
			elm = frm->findFirstElement("div#alley-search-myself p.error");
			if (!elm.isNull()) res = AT_ERR;							// error
		}
	} while (res < 0);
	return res;
}

// get atack result
bool MWBotWin::atackResult() {
	int res = 0;
	res = fightResult();
	if (res < 0) {
		return false;
	}
	loadPage("alley/");
	getAtackTimer();
	return true;
}
