#include "main.h"

void fillList(QStringList& list, QFile& file) {
	QString line;
	list.clear();
	do {
		line = QDialog::trUtf8(file.readLine()).remove("\r").remove("\n");
		list.append(line);
	} while (line != "}");
	list.removeLast();
}

int inRange(int num, int min, int max) {
	if (num < min) {
		num = min;
	} else if (num > max) {
		num = max;
	}
	return num;
}

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
				if (com == "openchest") opt.chest.open = bval;

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

				if (com == "bankticket") opt.bank.buy = bval;
				if (com == "bankstar") opt.bank.stars = bval;

				if (com == "rathunt") opt.ratk.enabled = bval;
				if (com == "ratdark") opt.ratk.dark = bval;
				if (com == "ratmaxlev") {
					opt.ratk.maxlev = ival;
					if (opt.ratk.maxlev < 1) opt.ratk.maxlev = 1;
					else if (opt.ratk.maxlev > 40) opt.ratk.maxlev = 40;
				}

				if (com == "oilpipe") opt.oil.enable = bval;
				if (com == "oildice") opt.oil.diceMax = ival;

				if (com == "baraban") opt.kub.play = bval;
				if (com == "buycaps") opt.kub.buy = bval;
				if (com == "caps") opt.kub.caps = ival;
				if (com == "kubDate") {
					opt.kub.date = QDate::fromString(val,"dd-MM-yyyy");
					if (!opt.kub.date.isValid()) opt.kub.date = QDate::currentDate().addDays(-1);
				}

				if (com == "minlev") opt.atk.minLev = inRange(ival, -50, 50);
				if (com == "maxlev") opt.atk.maxLev = inRange(ival, -50, 50);
				if (com == "statprc") {
					opt.atk.statPrc = val.toDouble();
					if (opt.atk.statPrc <= 0) {
						opt.atk.statPrc = 0.5;
					} else if (opt.atk.statPrc > 2) {
						opt.atk.statPrc = 2;
					}
				}
				if (com == "trainpet") opt.bPet.train = bval;
				if (com == "petnum") opt.bPet.num = inRange(ival, 1, 100);
				if (com == "tp-ruda") opt.bPet.useOre = bval;
				if (com == "tp-oil") opt.bPet.useOil = bval;

				if (com == "runner") opt.run.enabled = bval;
				if (com == "runname") opt.run.name = val;

				if (com == "goldplay") goldType = ival;
				if (com == "payfine") opt.police.fine = bval;
				if (com == "setrel") opt.police.relations = bval;

				if (com == "taxi") opt.taxi.enable = bval;
				if (com == "ridecar") opt.car.ride = bval;

				if (com == "useCheese") opt.group.cheese = bval;
				if (com == "useHeal") opt.group.heal = bval;
				if (com == "useBomb") opt.group.bomb = bval;
				if (com == "bombChance") opt.group.bombPrc = inRange(ival, 0, 100);

				if (com == "cheeseList") fillList(opt.group.cheeseList, file);
				if (com == "healList") fillList(opt.group.healList, file);
				if (com == "bombList") fillList(opt.group.bombList, file);
				if (com == "rideList") fillList(opt.car.list, file);
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

void writeList(QFile& file, const char* name, QStringList& list) {
	QString str;
	file.write(QString("%0:{\n").arg(name).toUtf8());
	foreach(str, list) {
		str.append("\n");
		file.write(str.toUtf8());
	}
	file.write("}\n");
}

void MWBotWin::saveOpts() {
	QFile file(workDir + "config.conf");
	if (file.open(QFile::WriteOnly)) {
		file.write(QString("atack:%0\n").arg(opt.atk.enabled ? "yes" : "no").toUtf8());
		file.write(QString("atype:%0\n").arg(opt.atk.typeA).toUtf8());
		file.write(QString("atype2:%0\n").arg(opt.atk.typeB).toUtf8());
		file.write(QString("droped:%0\n").arg(opt.atk.droped ? "yes" : "no").toUtf8());

		file.write(QString("useCheese:%0\n").arg(opt.group.cheese ? "yes" : "no").toUtf8());
		file.write(QString("useHeal:%0\n").arg(opt.group.heal ? "yes" : "no").toUtf8());
		file.write(QString("useBomb:%0\n").arg(opt.group.bomb ? "yes" : "no").toUtf8());
		file.write(QString("bombChance:%0\n").arg(opt.group.bombPrc).toUtf8());

		file.write(QString("makepetrik:%0\n").arg(opt.petrik.make ? "yes" : "no").toUtf8());

		file.write(QString("playmon:%0\n").arg(opt.monya.play ? "yes" : "no").toUtf8());
		file.write(QString("playsum:%0\n").arg(opt.monya.minPlaySum).toUtf8());
		file.write(QString("maxplaysum:%0\n").arg(opt.monya.maxPlaySum).toUtf8());
		file.write(QString("playtickets:%0\n").arg(opt.monya.tickets ? "yes" : "no").toUtf8());
		file.write(QString("buytickets:%0\n").arg(opt.monya.buy ? "yes" : "no").toUtf8());
		file.write(QString("buytickets_star:%0\n").arg(opt.monya.stars ? "yes" : "no").toUtf8());

		file.write(QString("bankticket:%0\n").arg(opt.bank.buy ? "yes" : "no").toUtf8());
		file.write(QString("bankstar:%0\n").arg(opt.bank.stars ? "yes" : "no").toUtf8());

		file.write(QString("rathunt:%0\n").arg(opt.ratk.enabled ? "yes" : "no").toUtf8());
		file.write(QString("ratmaxlev:%0\n").arg(opt.ratk.maxlev).toUtf8());
		file.write(QString("ratdark:%0\n").arg(opt.ratk.dark ? "yes" : "no").toUtf8());

		file.write(QString("oilpipe:%0\n").arg(opt.oil.enable ? "yes" : "no").toUtf8());
		file.write(QString("oildice:%0\n").arg(opt.oil.diceMax).toUtf8());

		file.write(QString("baraban:%0\n").arg(opt.kub.play ? "yes" : "no").toUtf8());
		file.write(QString("kubDate:%0\n").arg(opt.kub.date.toString("dd-MM-yyyy")).toUtf8());
		file.write(QString("buycaps:%0\n").arg(opt.kub.buy ? "yes" : "no").toUtf8());
		file.write(QString("caps:%0\n").arg(opt.kub.caps).toUtf8());

		file.write(QString("minlev:%0\n").arg(QString::number(opt.atk.minLev)).toUtf8());
		file.write(QString("maxlev:%0\n").arg(QString::number(opt.atk.maxLev)).toUtf8());
		file.write(QString("statprc:%0\n").arg(QString::number(opt.atk.statPrc)).toUtf8());

		file.write(QString("trainpet:%0\n").arg(opt.bPet.train ? "yes" : "no").toUtf8());
		file.write(QString("petnum:%0\n").arg(opt.bPet.num).toUtf8());
		file.write(QString("tp-ruda:%0\n").arg(opt.bPet.useOre ? "yes" : "no").toUtf8());
		file.write(QString("tp-oil:%0\n").arg(opt.bPet.useOil ? "yes" : "no").toUtf8());

		file.write(QString("runner:%0\n").arg(opt.run.enabled ? "yes" : "no").toUtf8());
		file.write(QString("runname:%0\n").arg(opt.run.name).toUtf8());
		file.write(QString("goldplay:").append(QString::number(goldType)).append("\n").toUtf8());
		file.write(QString("payfine:%0\n").arg(opt.police.fine ? "yes" : "no").toUtf8());
		file.write(QString("setrel:%0\n").arg(opt.police.relations ? "yes" : "no").toUtf8());
		file.write(QString("openchest:%0\n").arg(opt.chest.open ? "yes" : "no").toUtf8());

		file.write(QString("taxi:%0\n").arg(opt.taxi.enable ? "yes" : "no").toUtf8());
		file.write(QString("ridecar:%0\n").arg(opt.car.ride ? "yes" : "no").toUtf8());

		writeList(file, "cheeseList", opt.group.cheeseList);
		writeList(file, "healList", opt.group.healList);
		writeList(file, "bombList", opt.group.bombList);
		writeList(file, "rideList", opt.car.list);
	}
}

void MWBotWin::apply() {
	options = 0;
	opt.atk.enabled = ui.gbAttack->isChecked() ? 1 : 0;
	opt.atk.typeA = ui.cbAtackType->itemData(ui.cbAtackType->currentIndex()).toInt();
	opt.atk.typeB = ui.cbAType2->itemData(ui.cbAType2->currentIndex()).toInt();
	opt.atk.droped = ui.cbDrop->isChecked() ? 1 : 0;
	opt.petrik.make = ui.cbPetrik->isChecked() ? 1 : 0;

	opt.monya.play = ui.gbMonya->isChecked() ? 1 : 0;
	opt.monya.minPlaySum = ui.sbMoniaCoins->value();
	opt.monya.maxPlaySum = ui.sbMaxMoneyPlay->value();
	opt.monya.tickets = ui.cbPlayTickets->isChecked() ? 1 : 0;
	opt.monya.buy = ui.cbBuyTickets->isChecked() ? 1 : 0;
	opt.monya.stars = ui.rbMonyaStar->isChecked() ? 1 : 0;

	opt.bank.buy = ui.cbBankTicket->isChecked() ? 1 : 0;
	opt.bank.stars = ui.rbBankStar->isChecked() ? 1 : 0;

	opt.ratk.enabled = ui.gbRats->isChecked() ? 1 : 0;
	opt.ratk.maxlev = ui.sbRatMax->value();
	opt.ratk.dark = ui.cbDarkTunnel->isChecked() ? 1 : 0;

	opt.oil.enable = ui.oilBox->isChecked() ? 1 : 0;
	opt.oil.diceMax = ui.sbMaxDice->value();

	opt.kub.play = ui.gbBaraban->isChecked() ? 1 : 0;
	opt.kub.buy = ui.cbBuyCaps->isChecked() ? 1 : 0;
	opt.kub.caps = ui.sbCaps->value();

	opt.group.cheese = ui.cbGFCheese->isChecked() ? 1 : 0;
	opt.group.heal = ui.cbGFHeal->isChecked() ? 1 : 0;
	opt.group.bomb = ui.cbGFBomb->isChecked() ? 1 : 0;
	opt.group.bombPrc = ui.sbBombChance->value();

	opt.atk.minLev = ui.sbMinLev->value();
	opt.atk.maxLev = ui.sbMaxLev->value();
	opt.atk.statPrc = ui.sbStatCheck->value();

	opt.bPet.train = ui.gbFightPet->isChecked() ? 1 : 0;
	opt.bPet.num = ui.sbPetIndex->value();
	opt.bPet.useOre = ui.cbTrainRuda->isChecked() ? 1 : 0;
	opt.bPet.useOil = ui.cbTrainNeft->isChecked() ? 1 : 0;

	opt.run.enabled = ui.cbRunner->isChecked() ? 1 : 0;
	opt.run.name = ui.leRunName->text();
	opt.police.fine = ui.cbPolFine->isChecked() ? 1 : 0;
	opt.police.relations = ui.cbPolRelat->isChecked() ? 1 : 0;
	opt.chest.open = ui.cbChest->isChecked() ? 1 : 0;

	opt.taxi.enable = ui.cbTaxi->isChecked() ? 1 : 0;
	opt.car.ride = ui.cbRide->isChecked() ? 1 : 0;
	saveOpts();
}

void MWBotWin::setOpts() {
	ui.gbAttack->setChecked(opt.atk.enabled);
	ui.cbAtackType->setCurrentIndex(ui.cbAtackType->findData(opt.atk.typeA));
	ui.cbAType2->setCurrentIndex(ui.cbAType2->findData(opt.atk.typeB));
	ui.cbDrop->setChecked(opt.atk.droped);
	ui.cbPetrik->setChecked(opt.petrik.make);
	ui.gbMonya->setChecked(opt.monya.play);
	ui.sbMoniaCoins->setValue(opt.monya.minPlaySum);
	ui.sbMaxMoneyPlay->setValue(opt.monya.maxPlaySum);
	ui.cbPlayTickets->setChecked(opt.monya.tickets);
	ui.cbBuyTickets->setChecked(opt.monya.buy);
	if (opt.monya.stars) ui.rbMonyaStar->setChecked(true); else ui.rbMonyaTeeth->setChecked(true);

	ui.cbBankTicket->setChecked(opt.bank.buy);
	if (opt.bank.stars) ui.rbBankStar->setChecked(true); else ui.rbBankTeeth->setChecked(true);

	ui.gbRats->setChecked(opt.ratk.enabled);
	ui.cbDarkTunnel->setChecked(opt.ratk.dark);
	ui.sbRatMax->setValue(opt.ratk.maxlev);

	ui.oilBox->setChecked(opt.oil.enable);
	ui.sbMaxDice->setValue(opt.oil.diceMax);

	ui.gbBaraban->setChecked(opt.kub.play);
	ui.cbBuyCaps->setChecked(opt.kub.buy);
	ui.sbCaps->setValue(opt.kub.caps);

	ui.cbGFCheese->setChecked(opt.group.cheese);
	ui.cbGFHeal->setChecked(opt.group.heal);
	ui.cbGFBomb->setChecked(opt.group.bomb);
	ui.sbBombChance->setValue(opt.group.bombPrc);

	ui.sbMinLev->setValue(opt.atk.minLev);
	ui.sbMaxLev->setValue(opt.atk.maxLev);
	ui.sbStatCheck->setValue(opt.atk.statPrc);

	ui.gbFightPet->setChecked(opt.bPet.train);
	ui.sbPetIndex->setValue(opt.bPet.num);
	ui.cbTrainRuda->setChecked(opt.bPet.useOre);
	ui.cbTrainNeft->setChecked(opt.bPet.useOil);

	ui.cbRunner->setChecked(opt.run.enabled);
	ui.leRunName->setText(opt.run.name);
	ui.cbPolFine->setChecked(opt.police.fine);
	ui.cbPolRelat->setChecked(opt.police.relations);
	ui.cbChest->setChecked(opt.chest.open);

	ui.cbTaxi->setChecked(opt.taxi.enable);
	ui.cbRide->setChecked(opt.car.ride);
}
