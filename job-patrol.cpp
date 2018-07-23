#include "main.h"

int getPatrolTime(QWebFrame* frm) {
	int res = 0;
	QWebElement elm = frm->findFirstElement("div#content form#patrolForm td#patrol");
	if (eVisible(elm)) {
		qDebug() << "timer found";
		res = elm.attribute("timer").toInt() + 120;
	} else {
		qDebug() << "timer not found";
	}
	return res;
}

QDateTime chkNextPatrol(QWebFrame* frm) {
	QDateTime qdt = QDateTime::currentDateTime();
	qDebug() << "current time" << qdt;
	int tm = getPatrolTime(frm);
	qDebug() << "patrol timer" << tm;
	if (tm == 0) {
		qdt = qdt.addDays(1);
		qdt.setTime(QTime(0,10));
	} else {
		qdt = qdt.addSecs(tm);
	}
	return qdt;
}

void MWBotWin::goPatrol() {
	curTime = QDateTime::currentDateTime();
	if (curTime < opt.patrol.time) return;
	loadPath("alley");
	QWebElement elm = frm->findFirstElement("div#content.alley form#patrolForm div.time select");
	QWebElement ept;
	if (eVisible(elm)) {
		ept = elm.findFirst("option[selected]");
		if (eVisible(ept)) {
			ept.removeAttribute("selected");
		}
		ept = elm.findFirst(QString("option[value='%0']").arg(opt.patrol.period));
		if (!eVisible(ept)) {
			ept = elm.findAll(QString("option")).last();
		}
		if (eVisible(ept)) {
			ept.setAttribute("selected","true");
			elm = frm->findFirstElement("div#content.alley form#patrolForm button#alley-patrol-button div.c");
			if (eVisible(elm)) {
				click(ui.browser, elm);
				log(QString("%0 минут патруля").arg(ept.attribute("value")));
			}
			opt.patrol.time = chkNextPatrol(frm);
			log(QString("следующий патруль в %0").arg(opt.patrol.time.toString()));
		} else {
			log("some error");
		}
	} else {
		qDebug() << "no time select";
		// opt.patrol.period = 60;
		opt.patrol.time = chkNextPatrol(frm);
		log(QString("следующий патруль в %0").arg(opt.patrol.time.toString()));
	}
}

// =================

QDateTime chkNextWork(QWebFrame* frm) {
	QDateTime dt = QDateTime::currentDateTime();
	QWebElement elm = frm->findFirstElement("div#content form#workForm table.process td#shaurma");
	if (eVisible(elm)) {
		dt = dt.addSecs(elm.attribute("timer").toInt() + 120);
	} else {
		dt = dt.addDays(1);
		dt.setTime(QTime(0,10));
	}
	return dt;
}

void MWBotWin::goWork() {
	curTime = QDateTime::currentDateTime();
	if (curTime < opt.job.time) return;
	loadPath("square:shaurburgers");
	QWebElement elm = frm->findFirstElement("div#content form#workForm select[name=time]");
	QWebElement ept;
	if (eVisible(elm)) {
		if (eVisible(ept)) {
			ept.removeAttribute("selected");
		}
		ept = elm.findFirst(QString("option[value='%0']").arg(opt.job.period));
		if (!eVisible(ept)) {
			ept = elm.findAll("option").last();
		}
		if (eVisible(ept)) {
			ept.setAttribute("selected","true");
			elm = frm->findFirstElement("div#content form#workForm span.button span.f div.c");
			if (eVisible(elm)) {
				click(ui.browser, elm);
				log(QString("%0 часов работы").arg(ept.attribute("value")));
			}
			opt.job.time = chkNextWork(frm);
			log(QString("На работу в %0").arg(opt.job.time.toString()));
		} else {
			log("job error");
		}
	} else {
		qDebug() << "selector not found";
		opt.job.time = chkNextWork(frm);
		log(QString("На работу в %0").arg(opt.job.time.toString()));
	}
}
