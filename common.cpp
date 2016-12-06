#include "main.h"

int eVisible(QWebElement& elm) {
	if (elm.isNull()) return 0;
	if (elm.styleProperty(QString("display"),QWebElement::ComputedStyle).contains("none",Qt::CaseInsensitive)) return 0;
	return 1;
}
