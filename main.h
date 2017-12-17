#ifndef _MAIN_H
#define _MAIN_H

#include <QtGui>
#include <QNetworkCookie>
#include <QtWebKit>
#include <QWebFrame>
#include <QDate>
#include <QNetworkDiskCache>
#include <QAbstractItemModel>

#include "ui_mainwindow.h"
#include "ui_textwindow.h"

struct mwItem {
	QString name;
	QString icn;
	int count;
};

struct grpChar {
	unsigned me:1;
	unsigned alive:1;
	QString name;
	int id;
	int maxhp;
	int hp;
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

enum {
	resLose = 0,
	resWin,
	resDraw,
	resChest,
	resBonus
};

struct FightBox {
	int result;
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

#define	FL_STOP		(1<<11)

enum {
	ATK_EQUAL = 1,
	ATK_WEAK,
	ATK_STRONG,
	ATK_ENEMY,
	ATK_VICTIM,
	ATK_LEVEL
};

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
		QDialog* tedit;
		struct {
			struct {
				unsigned enable:1;
				int diceMax;
				QDateTime time;
			} oil;
			struct {
				unsigned enable:1;
				QDateTime time;
			} taxi;
			struct {
				unsigned open:1;
				unsigned need:1;
//				int keyOil;
//				int keyRat;
//				int keyElect;
			} chest;
			struct {
				unsigned enabled:1;
				QString name;
				QDateTime time;
			} run;
			struct {
				unsigned enabled:1;
				unsigned droped:1;
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
				unsigned bomb:1;
				int cheesePrc;
				int healPrc;
				int bombPrc;
				QStringList cheeseList;
				QStringList healList;
				QStringList bombList;
			} group;
			struct {
				unsigned enabled:1;
				unsigned dark:1;
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
				QDateTime time;
			} monya;
			struct {
				unsigned buy:1;
				unsigned stars:1;
				QDateTime time;
			} bank;
			struct {
				unsigned train:1;
				unsigned useOre:1;
				unsigned useOil:1;
				int num;
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
			struct {
				unsigned ride:1;
				QStringList list;
				QDateTime time;
			} car;
		} opt;
		struct {
			unsigned busy:1;
			unsigned stop:1;
//			unsigned loading:1;
			unsigned botWork:1;
			unsigned firstRun:1;
		} state;

		QString getItemIcon(QString);

		CharInfo info;
		void getResources();
		void getFastRes();
		void getBerezkaRes();

		QWebElement getItemElement(int);
		int getItem(int);
		void doChest(int, int = 0);

		int goldType;
		int options;
		int flag;

		Ui::MainWin ui;
		Ui::TEdit tui;
		QEventLoop evloop;
		QWebFrame* frm;
		QNetworkDiskCache* cache;

		QStringList* editList;

		QDateTime curTime;

		CharBox getStat(QString,QString);

		int fightResult();

		FightBox getDuelResult();
		FightBox getGroupResult();
		FightBox getChestResult();
		QList<mwItem> getDuelResultMain();
		QList<mwItem> getDuelResultExtra();
		QList<mwItem> getGroupResultMain();
		QList<mwItem> getGroupResultExtra();
		void logResult(FightBox);

		void groupFight();

		bool loadPath(QString);

		int getAtackTimer();
		int getRatTimer();
		void restoreHP();
		void setBusy(bool);

		void playMonia();
		void goBankChange();

		void oilGameEscape(int);
		int checkSusp(int, int);

		void checkPolice();
		int checkCooldown(QWebElement&, QString);	// check ride cooldown

		void log(QString, QString icon="");

		void debug();

	private slots:
		void attack();
		int atkSelect(int);
		int atkCheck(CharBox&,int);
		bool atkResult();

		void atackOil();
		void atkRat();
		void goMonia();
		void makePetrik();
		void sellLots();
		void gipsy();
		void playKub();
		void trainPet();
		void arena();
		void doTaxi();
		void rideCar();

		void openChests(int = 0);
		void checkChests(FightBox);

		void apply();
		void editCheese();
		void editHeal();
		void editBomb();
		void editRide();
		void setList();

		void onStart();
		void onLoad(bool);

		void start();
		void savePage();
		void chZoom(int);

	protected:
		void closeEvent(QCloseEvent*);
		void timerEvent(QTimerEvent*);
		void keyPressEvent(QKeyEvent*);
};

extern int loading;

int toNumber(QString);
int toLarge(QString);

void doLoop();
int eVisible(QWebElement&);

int click(QWebView*, QString, double = 1.0);
int click(QWebView*, QWebElement&, double = 1.0);

int isLoading(QWebFrame*);
int waitLoading(QWebView*, double = 1.0);
void waitReload(QWebView*);
void pause(double);

// temp?
QList<grpChar> grpGetAllies(QWebFrame*);
QList<grpChar> grpGetEnemies(QWebFrame*);

#endif
