#ifndef _MAIN_H
#define _MAIN_H

#include <QtGui>
#include <QNetworkCookie>
#include <QtWebKit>
#include <QDate>
#include <QNetworkDiskCache>

#include "ui_mainwindow.h"

struct mwItem {
	QString name;
	QString icn;
	int count;
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

struct FightBox {
	int result;	// 0:lost 1:win 2:draw
	CharBox enemy;
	QList<mwItem> items;
};

// new

struct CharInfo {
	QString name;
	int level;
	int hp;
	int maxhp;
	int tonus;
	int maxtonus;
	int wanted;
	int money;
	int ore;
	int oil;
	int honey;
	int tooth;
	int goldtooth;
	int star;
	int badge;
	int mobile;
	int medal;
	int power;
	int caps;
	QMap<int,int> resMap;
};


// end new

#define	FL_OIL		(1<<5)
#define	FL_SCAN		(1<<10)
#define	FL_STOP		(1<<11)
#define	FL_DIG		(1<<12)		// do dig
#define FL_DIGGING	(1<<13)		// dig in process
#define	FL_DIGRAT	(1<<14)		// atack rats during dig

#define	ATK_EQUAL	1
#define	ATK_WEAK	2
#define	ATK_STRONG	3
#define ATK_ENEMY	4
#define	ATK_VICTIM	5
#define	ATK_LEVEL	6

class MWBotWin : public QMainWindow {
	Q_OBJECT
	public:
		MWBotWin();
		void prepare();
	public slots:
		void loadCookies();
		void saveCookies();
		bool loadPage(QString);
		void setOpts();
		void loadOpts();
		void saveOpts();
	private:
		int timerId;
		QString workDir;
		struct {
			unsigned petRun:1;
			struct {
				unsigned enabled:1;
				int typeA;
				int typeB;
				int minLev;
				int maxLev;
				double statPrc;
				QDateTime time;
			} atk;
			struct {
				unsigned cheese:1;
				unsigned heal:1;
			} group;
			struct {
				unsigned enabled:1;
				unsigned block:1;
				int ratlev;
				int maxlev;
				QDateTime time;
			} ratk;
			struct {
				unsigned make:1;
				int money;
				int ore;
				QDateTime time;
			} petrik;
			struct {
				unsigned block:1;
				unsigned play:1;
				unsigned tickets:1;
				unsigned buy:1;
				unsigned stars:1;
				int minPlaySum;
				int maxPlaySum;
				//QDate date;
			} monya;
			struct {
				//unsigned block:1;
				unsigned train:1;
				unsigned useOre:1;
				unsigned useOil:1;
				int money;
				int ore;
				int oil;
				QDateTime time;
			} bPet;		// battle pet
			struct {
				unsigned fine:1;
				unsigned relations:1;
			} police;
			struct {
				unsigned play:1;
				unsigned buy:1;
				int caps;
				QDate date;
			} kub;
		} opt;
		struct {
			unsigned stop:1;
			unsigned loading:1;
			unsigned botWork:1;
			unsigned firstRun:1;
//			unsigned atkChillout:1;
		} state;

		QString getItemIcon(QString);

		CharInfo info;
		void getResources();
		void getFastRes();
		void getBerezkaRes();

		int goldType;
		int options;
		int flag;
//		int buyCaps;

		Ui::MainWin ui;
		QEventLoop evloop;
		QWebFrame* frm;
		QNetworkAccessManager* mgr;
		QNetworkDiskCache* cache;

		QDateTime curTime;
		QDateTime digTime;
		QDateTime runTime;

		CharBox getStat(QString,QString);

		// FightBox getResult();
		int fightResult();

		FightBox getDuelResult();
		FightBox getGroupResult();
		QList<mwItem> getDuelResultMain();
		QList<mwItem> getDuelResultExtra();
		QList<mwItem> getGroupResultMain();
		QList<mwItem> getGroupResultExtra();
		void logResult(FightBox);

		void groupFight();

		bool loadPath(QStringList);
		void clickElement(QString, int = 1000);
		void clickElement(QWebElement&, int = 1000);

		void waitLoading(int = 1000);
		int getAtackTimer();
		int getRatTimer();
		void restoreHP();
		void setBusy(bool);

		void playMonia();
		void goBankChange();

		void checkPolice();

		void log(QString);

		void debug();

	private slots:
		void attack();
		int atkSelect(int);
		int atkCheck(CharBox&,int);
		bool atkResult();

		void atkRat();
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
		void savePage();
	signals:
		void digEnded();
	protected:
		void closeEvent(QCloseEvent*);
		void timerEvent(QTimerEvent*);
		void keyPressEvent(QKeyEvent*);
};

#endif
