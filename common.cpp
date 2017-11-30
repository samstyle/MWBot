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

// wait max 30s for data loading finished
int waitLoading(QWebView* view, double time) {
	QWebFrame* frm = view->page()->mainFrame();
	while (isLoading(frm)) {
		doLoop();
	}
	pause(time);
	return 1;
}

// wait for data loading started, then finished
void waitReload(QWebView* view) {
	QWebFrame* frm = view->page()->mainFrame();
	while(!isLoading(frm))
		doLoop();
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
	QWebFrame* frm = view->page()->mainFrame();
	if (!eVisible(elm)) {
		qDebug() << "trying to click non-existing element";
		res = 0;
	} else {
		elm.setAttribute("mustBeClicked","1");
		frm->evaluateJavaScript(clickjs);
		elm.removeAttribute("mustBeClicked");
		if (time > 0)
			waitLoading(view, time);
		res = 1;
	}
	return res;
}

int click(QWebView* view, QString query, double time) {
	int res;
	QWebFrame* frm = view->page()->mainFrame();
	QWebElement elm = frm->findFirstElement(query);
	if (eVisible(elm)) {
		res = click(view, elm, time);
	} else {
		qDebug() << "element '" << query << "not found";
		res = 0;
	}
	return res;
}
