#include "main.h"

void MWBotWin::loadOpts() {
	QFile file(workDir + "config.conf");
	if (file.open(QFile::ReadOnly)) {
		options = 0;
		QString line;
		QStringList pars;
		QString com,val;
		int ival;
		int bval;
		while (!file.atEnd()) {
			line = QDialog::trUtf8(file.readLine()).remove("\r").remove("\n");
			pars = line.split(":",QString::SkipEmptyParts);
			if (pars.size() == 2) {
				com = pars.first();
				val = pars.last();
				ival = val.toInt();
				bval = (val == "yes") ? 1 : 0;
				if (com == "atack") opt.atk.enabled = bval;
				if (com == "atype") opt.atk.typeA = ival;
				if (com == "atype2") opt.atk.typeB = ival;
				if (com == "droped") opt.atk.droped = bval;

				if (com == "makepetrik") opt.petrik.make = bval;

				if (com == "playmon") opt.monya.play = bval;
				if (com == "playsum") {
					if (ival == 0) ival = 30000;
					opt.monya.minPlaySum = ival;
				}
				if (com == "maxplaysum") {
					if (ival == 0) ival = 100000;
					opt.monya.maxPlaySum = ival;
				}
				if (com == "playtickets") opt.monya.tickets = bval;
				if (com == "buytickets") opt.monya.buy = bval;
				if (com == "buytickets_star") opt.monya.stars = bval;

				if (com == "rathunt") opt.ratk.enabled = bval;
				if (com == "ratdark") opt.ratk.dark = bval;
				if (com == "ratmaxlev") {
					opt.ratk.maxlev = ival;
					if (opt.ratk.maxlev < 1) opt.ratk.maxlev = 1;
					else if (opt.ratk.maxlev > 40) opt.ratk.maxlev = 40;
				}

//				if ((com == "digger") && bval) options |= FL_DIG;
//				if ((com == "digrat") && bval) options |= FL_DIGRAT;

				if (com == "baraban") opt.kub.play = bval;
				if (com == "buycaps") opt.kub.buy = bval;
				if (com == "caps") opt.kub.caps = ival;
				if (com == "kubDate") {
					opt.kub.date = QDate::fromString(val,"dd-MM-yyyy");
					if (!opt.kub.date.isValid()) opt.kub.date = QDate::currentDate().addDays(-1);
				}

				if (com == "minlev") {
					opt.atk.minLev = ival;
					if (opt.atk.minLev > 50) {
						opt.atk.minLev = 50;
					} else if (opt.atk.minLev < -50) {
						opt.atk.minLev = -50;
					}
				}
				if (com == "maxlev") {
					if (ival > 50) ival = 50;
					if (ival < -50) ival = -50;
					opt.atk.maxLev = ival;
				}
				if (com == "statprc") {
					opt.atk.statPrc = val.toDouble();
					if (opt.atk.statPrc <= 0) {
						opt.atk.statPrc = 0.5;
					} else if (opt.atk.statPrc > 2) {
						opt.atk.statPrc = 2;
					}
				}
//				if ((com == "checknpc") && (val == "no")) options |= FL_NONPC;
				if (com == "trainpet") opt.bPet.train = bval;
				if (com == "tp-ruda") opt.bPet.useOre = bval;
				if (com == "tp-oil") opt.bPet.useOil = bval;
				if (com == "runner") opt.petRun = bval;
				if (com == "goldplay") goldType = ival;
				if (com == "useCheese") opt.group.cheese = bval;
				if (com == "useHeal") opt.group.heal = bval;
				if (com == "payfine") opt.police.fine = bval;
				if (com == "setrel") opt.police.relations = bval;

				if (com == "cheeseList") {
					cheeseList.clear();
					do {
						line = QDialog::trUtf8(file.readLine()).remove("\r").remove("\n");
						cheeseList.append(line);
					} while (line != "}");
					cheeseList.removeLast();
				}
				if (com == "healList") {
					healList.clear();
					do {
						line = QDialog::trUtf8(file.readLine()).remove("\r").remove("\n");
						healList.append(line);
					} while (line != "}");
					healList.removeLast();
				}
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
	QString str;
	if (file.open(QFile::WriteOnly)) {
		file.write(QString("atack:%0\n").arg(opt.atk.enabled ? "yes" : "no").toUtf8());
		file.write(QString("atype:%0\n").arg(opt.atk.typeA).toUtf8());
		file.write(QString("atype2:%0\n").arg(opt.atk.typeB).toUtf8());
		file.write(QString("droped:%0\n").arg(opt.atk.droped ? "yes" : "no").toUtf8());

		file.write(QString("useCheese:%0\n").arg(opt.group.cheese ? "yes" : "no").toUtf8());
		file.write(QString("useHeal:%0\n").arg(opt.group.heal ? "yes" : "no").toUtf8());

		file.write(QString("makepetrik:%0\n").arg(opt.petrik.make ? "yes" : "no").toUtf8());
		file.write(QString("playmon:%0\n").arg(opt.monya.play ? "yes" : "no").toUtf8());
		file.write(QString("playsum:%0\n").arg(opt.monya.minPlaySum).toUtf8());
		file.write(QString("maxplaysum:%0\n").arg(opt.monya.maxPlaySum).toUtf8());
		file.write(QString("playtickets:%0\n").arg(opt.monya.tickets ? "yes" : "no").toUtf8());
		file.write(QString("buytickets:%0\n").arg(opt.monya.buy ? "yes" : "no").toUtf8());
		file.write(QString("buytickets_star:%0\n").arg(opt.monya.stars ? "yes" : "no").toUtf8());
		file.write(QString("rathunt:%0\n").arg(opt.ratk.enabled ? "yes" : "no").toUtf8());
		file.write(QString("ratmaxlev:%0\n").arg(opt.ratk.maxlev).toUtf8());
		file.write(QString("ratdark:%0\n").arg(opt.ratk.dark ? "yes" : "no").toUtf8());

//		file.write(QString("digger:").append((options & FL_DIG) ? "yes" : "no").append("\n").toUtf8());
//		file.write(QString("digrat:").append((options & FL_DIGRAT) ? "yes" : "no").append("\n").toUtf8());

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

		file.write(QString("cheeseList:{\n").toUtf8());
		foreach(str, cheeseList) {
			str.append("\n");
			file.write(str.toUtf8());
		}
		file.write("}\n");
		file.write(QString("healList:{\n").toUtf8());
		foreach(str, healList) {
			str.append("\n");
			file.write(str.toUtf8());
		}
		file.write("}\n");

	}
}

void MWBotWin::apply() {
	options = 0;
	opt.atk.enabled = ui.gbAttack->isChecked() ? 1 : 0;
	opt.atk.typeA = ui.cbAtackType->itemData(ui.cbAtackType->currentIndex()).toInt();
	opt.atk.typeB = ui.cbAType2->itemData(ui.cbAType2->currentIndex()).toInt();
	opt.atk.droped = ui.cbDrop->isChecked() ? 1 : 0;
//	goldType = ui.boxGypsy->itemData(ui.boxGypsy->currentIndex()).toInt();
	opt.petrik.make = ui.cbPetrik->isChecked() ? 1 : 0;
	opt.monya.play = ui.gbMonya->isChecked() ? 1 : 0;
	opt.monya.minPlaySum = ui.sbMoniaCoins->value();
	opt.monya.maxPlaySum = ui.sbMaxMoneyPlay->value();
	opt.monya.tickets = ui.cbPlayTickets->isChecked() ? 1 : 0;
	opt.monya.buy = ui.cbBuyTickets->isChecked() ? 1 : 0;
	opt.monya.stars = ui.cbTicketStars->isChecked() ? 1 : 0;
	opt.ratk.enabled = ui.gbRats->isChecked() ? 1 : 0;
	opt.ratk.maxlev = ui.sbRatMax->value();
	opt.ratk.dark = ui.cbDarkTunnel->isChecked() ? 1 : 0;
//	if (ui.cbDigger->isChecked()) options |= FL_DIG;
//	if (ui.cbDigRat->isChecked()) options |= FL_DIGRAT;

	opt.kub.play = ui.gbBaraban->isChecked() ? 1 : 0;
	opt.kub.buy = ui.cbBuyCaps->isChecked() ? 1 : 0;
	opt.kub.caps = ui.sbCaps->value();

	opt.atk.minLev = ui.sbMinLev->value();
	opt.atk.maxLev = ui.sbMaxLev->value();
	opt.atk.statPrc = ui.sbStatCheck->value();
//	if (ui.cbNPCheck->isChecked()) options |= FL_NONPC;
	opt.bPet.train = ui.gbFightPet->isChecked() ? 1 : 0;
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
	ui.gbAttack->setChecked(opt.atk.enabled);
	ui.cbAtackType->setCurrentIndex(ui.cbAtackType->findData(opt.atk.typeA));
	ui.cbAType2->setCurrentIndex(ui.cbAType2->findData(opt.atk.typeB));
	ui.cbDrop->setChecked(opt.atk.droped);
//	ui.boxGypsy->setCurrentIndex(ui.boxGypsy->findData(goldType));
	ui.cbPetrik->setChecked(opt.petrik.make);
	ui.gbMonya->setChecked(opt.monya.play);
	ui.sbMoniaCoins->setValue(opt.monya.minPlaySum);
	ui.sbMaxMoneyPlay->setValue(opt.monya.maxPlaySum);
	ui.cbPlayTickets->setChecked(opt.monya.tickets);
	ui.cbBuyTickets->setChecked(opt.monya.buy);
	ui.cbTicketStars->setChecked(opt.monya.stars);
	ui.gbRats->setChecked(opt.ratk.enabled);
	ui.cbDarkTunnel->setChecked(opt.ratk.dark);
//	ui.cbDigger->setChecked(options & FL_DIG);
//	ui.cbDigRat->setChecked(options & FL_DIGRAT);

	ui.gbBaraban->setChecked(opt.kub.play);
	ui.cbBuyCaps->setChecked(opt.kub.buy);
	ui.sbCaps->setValue(opt.kub.caps);

	ui.sbMinLev->setValue(opt.atk.minLev);
	ui.sbMaxLev->setValue(opt.atk.maxLev);
	ui.sbStatCheck->setValue(opt.atk.statPrc);
//	ui.cbNPCheck->setChecked(options & FL_NONPC);
	ui.gbFightPet->setChecked(opt.bPet.train);
	ui.cbTrainRuda->setChecked(opt.bPet.useOre);
	ui.cbTrainNeft->setChecked(opt.bPet.useOil);
	ui.cbRunner->setChecked(opt.petRun);
	ui.cbGFCheese->setChecked(opt.group.cheese);
	ui.cbGFHeal->setChecked(opt.group.heal);
	ui.cbPolFine->setChecked(opt.police.fine);
	ui.cbPolRelat->setChecked(opt.police.relations);
}
