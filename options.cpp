#include "main.h"

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

				if (pars.first() == "baraban") opt.kub.play = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "buycaps") opt.kub.buy = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "caps") opt.kub.caps = pars.last().toInt();
				if (pars.first() == "kubDate") {
					opt.kub.date = QDate::fromString(pars.last(),"dd-MM-yyyy");
					if (!opt.kub.date.isValid()) opt.kub.date = QDate::currentDate().addDays(-1);
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
//				if ((pars.first() == "checknpc") && (pars.last() == "no")) options |= FL_NONPC;
				if (pars.first() == "trainpet") opt.bPet.train = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "tp-ruda") opt.bPet.useOre = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "tp-oil") opt.bPet.useOil = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "runner") opt.petRun = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "goldplay") goldType = pars.last().toInt();
				if (pars.first() == "useCheese") opt.group.cheese = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "useHeal") opt.group.heal = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "payfine") opt.police.fine = (pars.last() == "yes") ? 1 : 0;
				if (pars.first() == "setrel") opt.police.relations = (pars.last() == "yes") ? 1 : 0;
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
		file.write(QString("useCheese:%0\n").arg(opt.group.cheese ? "yes" : "no").toUtf8());
		file.write(QString("useHeal:%0\n").arg(opt.group.heal ? "yes" : "no").toUtf8());
		file.write(QString("makepetrik:%0\n").arg(opt.petrik.make ? "yes" : "no").toUtf8());
		file.write(QString("playmon:%0\n").arg(opt.monya.play ? "yes" : "no").toUtf8());
		file.write(QString("playsum:%0\n").arg(playSum).toUtf8());
		file.write(QString("playtickets:%0\n").arg(opt.monya.tickets ? "yes" : "no").toUtf8());
		file.write(QString("buytickets:%0\n").arg(opt.monya.buy ? "yes" : "no").toUtf8());
		file.write(QString("buytickets_star:%0\n").arg(opt.monya.stars ? "yes" : "no").toUtf8());
		file.write(QString("rathunt:%0\n").arg(opt.ratk.enabled ? "yes" : "no").toUtf8());
		file.write(QString("ratmaxlev:%0\n").arg(opt.ratk.maxlev).toUtf8());
		file.write(QString("digger:").append((options & FL_DIG) ? "yes" : "no").append("\n").toUtf8());
		file.write(QString("digrat:").append((options & FL_DIGRAT) ? "yes" : "no").append("\n").toUtf8());

		file.write(QString("baraban:%0\n").arg(opt.kub.play ? "yes" : "no").toUtf8());
		file.write(QString("kubDate:%0\n").arg(opt.kub.date.toString("dd-MM-yyyy")).toUtf8());
		file.write(QString("buycaps:%0\n").arg(opt.kub.buy ? "yes" : "no").toUtf8());
		file.write(QString("caps:%0\n").arg(opt.kub.caps).toUtf8());

		file.write(QString("minlev:%0\n").arg(QString::number(opt.atk.minLev)).toUtf8());
		file.write(QString("maxlev:%0\n").arg(QString::number(opt.atk.maxLev)).toUtf8());
		file.write(QString("statprc:%0\n").arg(QString::number(opt.atk.statPrc)).toUtf8());
//		file.write(QString("checknpc:").append((options & FL_NONPC) ? "no" : "yes").append("\n").toUtf8());
		file.write(QString("trainpet:%0\n").arg(opt.bPet.train ? "yes" : "no").toUtf8());
		file.write(QString("tp-ruda:%0\n").arg(opt.bPet.useOre ? "yes" : "no").toUtf8());
		file.write(QString("tp-oil:%0\n").arg(opt.bPet.useOil ? "yes" : "no").toUtf8());
		file.write(QString("runner:%0\n").arg(opt.petRun ? "yes" : "no").toUtf8());
		file.write(QString("goldplay:").append(QString::number(goldType)).append("\n").toUtf8());
		file.write(QString("payfine:%0\n").arg(opt.police.fine ? "yes" : "no").toUtf8());
		file.write(QString("setrel:%0\n").arg(opt.police.relations ? "yes" : "no").toUtf8());
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

	opt.kub.play = ui.cbRoll->isChecked() ? 1 : 0;
	opt.kub.buy = ui.cbBuyCaps->isChecked() ? 1 : 0;
	opt.kub.caps = ui.sbCaps->value();

	opt.atk.minLev = ui.sbMinLev->value();
	opt.atk.maxLev = ui.sbMaxLev->value();
	opt.atk.statPrc = ui.sbStatCheck->value();
//	if (ui.cbNPCheck->isChecked()) options |= FL_NONPC;
	opt.bPet.train = ui.cbTrain->isChecked() ? 1 : 0;
	opt.bPet.useOre = ui.cbTrainRuda->isChecked() ? 1 : 0;
	opt.bPet.useOil = ui.cbTrainNeft->isChecked() ? 1 : 0;
	opt.petRun = ui.cbRunner->isChecked() ? 1 : 0;
	opt.group.cheese = ui.cbGFCheese->isChecked() ? 1 : 0;
	opt.group.heal = ui.cbGFHeal->isChecked() ? 1 : 0;
	opt.police.fine = ui.cbPolFine->isChecked() ? 1 : 0;
	opt.police.relations = ui.cbPolRelat->isChecked() ? 1 : 0;
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

	ui.cbRoll->setChecked(opt.kub.play);
	ui.cbBuyCaps->setChecked(opt.kub.buy);
	ui.sbCaps->setValue(opt.kub.caps);

	ui.sbMinLev->setValue(opt.atk.minLev);
	ui.sbMaxLev->setValue(opt.atk.maxLev);
	ui.sbStatCheck->setValue(opt.atk.statPrc);
//	ui.cbNPCheck->setChecked(options & FL_NONPC);
	ui.cbTrain->setChecked(opt.bPet.train);
	ui.cbTrainRuda->setChecked(opt.bPet.useOre);
	ui.cbTrainNeft->setChecked(opt.bPet.useOil);
	ui.cbRunner->setChecked(opt.petRun);
	ui.cbGFCheese->setChecked(opt.group.cheese);
	ui.cbGFHeal->setChecked(opt.group.heal);
	ui.cbPolFine->setChecked(opt.police.fine);
	ui.cbPolRelat->setChecked(opt.police.relations);
}
