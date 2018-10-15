#include "main.h"

// flag : page loaging in progress
int loading = 0;

int toNumber(QString str) {
	return str.remove("#").remove(",").remove("%").remove("\"").toDouble();
}

int toLarge(QString str) {
	return str.replace(",",".").replace("k","e3").replace("M","e6").replace("B","e9").toDouble();
}

// return 1 if element is visible on screen
int eVisible(QWebElement& elm) {
	if (elm.isNull()) return 0;
	if (elm.styleProperty(QString("display"),QWebElement::ComputedStyle).contains("none",Qt::CaseInsensitive)) return 0;
	return 1;
}

// return 1 if data loading in progress
int isLoading(QWebFrame* frm) {
	if (loading) return 1;
	QWebElement elm = frm->findFirstElement("div.loading-top");
	return eVisible(elm);
}

// wait max 10s for data loading finished
int waitLoading(QWebView* view, double time) {
	QWebFrame* frm = view->page()->mainFrame();
	double crtime = 5.0;
	int atempt = 0;
	while (isLoading(frm) && (atempt < 5)) {
		doLoop();
		crtime -= 0.001;
		if (crtime < 0) {
			view->reload();
			crtime = 10.0;
			atempt++;
		}
	}
	pause(time);
	return (atempt < 5);
}

// wait for data loading started, then finished
void waitReload(QWebView* view) {
	QWebFrame* frm = view->page()->mainFrame();
	while(!isLoading(frm)) {
		doLoop();
	}
	waitLoading(view);
}

// do nothing during some time
void pause(double time) {
	while (time > 0) {
		doLoop();
		time -= 0.001;
	}
}

// click element [selected by CSS-query] and wait for data loading after it

const QString clickjs = "\
var evt=document.createEvent('MouseEvents');\
evt.initMouseEvent('click',true,true,window,0,0,0,0,0,false,false,false,false,0,null);\
var elst = document.querySelectorAll('[mustBeClicked]');\
if (elst.length > 0) {elst[0].dispatchEvent(evt);}\
";

int click(QWebView* view, QWebElement& elm, double time) {
	int res ;
	if (!eVisible(elm)) {
		res = 0;
	} else {
		elm.setAttribute("mustBeClicked","1");
		view->page()->mainFrame()->evaluateJavaScript(clickjs);
		elm.removeAttribute("mustBeClicked");
		if (time > 0)
			waitLoading(view, time);
		res = 1;
	}
	return res;
}

int click(QWebView* view, QString query, double time) {
	QWebFrame* frm = view->page()->mainFrame();
	QWebElement elm = frm->findFirstElement(query);
	if (click(view, elm, time))
		return 1;
	qDebug() << "element '" << query << "not found";
	return 0;
}
