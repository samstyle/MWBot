#ifndef _MAIN_H
#define _MAIN_H

#include <QtGui>
#include <QtWebKit>

#include "ui_mainwindow.h"

struct Object {
	QString name;
	QString icon;
	int count;
};

struct FightBox {
	int result;	// 0:lost 1:win 2:draw
	QList<Object> items;
};

struct ResBox {
	int curhp;
	int maxhp;
	int curtonus;
	int maxtonus;
	int wanted;
	int money;
	int ruda;
	int neft;
	int med;
	int gtooth;
	int wtooth;
	int star;
	int badge;
	int mobila;
	int ipoints;
	int medals;
};

struct CharBox{
	int level;
	int id;
	QString type;	// resident,arrived,npc
	QString name;
	QString clan;
	struct {
		int zdor;
		int sila;
		int lovk;
		int vynos;
		int hitr;
		int vnim;
		int hari;
	} stat;
	int statsum;
};

#define	FL_FIRST	1
#define	FL_PETRIK	(1<<1)		// make petriks
#define	FL_ATACK	(1<<2)		// atack enemies
#define	FL_BOT		(1<<3)
#define	FL_RAT		(1<<4)		// atack rats
#define	FL_OIL		(1<<5)
#define	FL_MONIA	(1<<6)		// play with monia
#define	FL_MONIA_BILET	(1<<7)		// ...use tickets
#define	FL_MONIA_BUY	(1<<8)		// ...buy tickets
#define FL_MONIA_STAR	(1<<9)		// ...by stars (else by teeths)
#define	FL_SCAN		(1<<10)
#define	FL_STOP		(1<<11)
#define	FL_DIG		(1<<12)		// do dig
#define FL_DIGGING	(1<<13)		// dig in process
#define	FL_DIGRAT	(1<<14)		// atack rats during dig
#define	FL_KUB		(1<<15)		// play baraban
#define	FL_NONPC	(1<<16)		// don't check npc stats
#define	FL_TRAIN	(1<<17)		// train fight pet
#define	FL_TR_RUDA	(1<<18)
#define	FL_TR_OIL	(1<<19)
#define FL_RUN		(1<<20)

#define	ATACK_EQUAL	1
#define	ATACK_WEAK	2
#define	ATACK_STRONG	3
#define ATACK_ENEMY	4
#define	ATACK_VICTIM	5
#define	ATACK_LEVEL	6

class MWBotWin : public QMainWindow {
	Q_OBJECT
	public:
		MWBotWin();
	public slots:
		void loadCookies();
		void saveCookies();
		bool loadPage(QString);
		void setOpts();
		void loadOpts();
		void saveOpts();
	private:
		int atackType;
		int atackType2;
		int goldType;
		int opt;
		int flag;
		int minLev;
		int maxLev;
		double statPrc;

		bool loading;
		Ui::MainWin ui;
		QEventLoop evloop;
		QWebFrame* frm;

		QDateTime curTime;
		QDateTime ratTime;
		QDateTime ataTime;
		QDateTime ptrTime;
		QDateTime digTime;
		QDateTime trnTime;
		QDateTime runTime;

		struct {
			int nextRat;
			int lev;
		} pers;

		CharBox getStat(QString,QString);
		ResBox getResources();
		ResBox getBerezkaRes();
		FightBox getResult();
		int fightResult();

		bool loadPath(QStringList);
		void clickElement(QString,int);

		void waitLoading(int);
		void waitDropDown();
		int getAtackTimer();
		int getRatTimer();
		void restoreHP();
		void setBusy(bool);

		void playMonia();

		void log(QString);

	private slots:
		void atack();
		int atackSel(int);
		int atackCheck(CharBox&,int);
		bool atackResult();

		void atackRat();
		void atackOil();
		void goMonia();
		void makePetrik();
		void sellLots();
		void gipsy();
		void dig();
		void digEnd();
		void playKub();
		void trainPet();
		void arena();

		void apply();

		void onStart();
		void onLoad(bool);

		void start();
		void stop();
	signals:
		void digEnded();
	protected:
		void timerEvent(QTimerEvent*);
};

#endif
