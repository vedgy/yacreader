#ifndef YACREADER_LIBRARY_ITEM_WIDGET_H
#define YACREADER_LIBRARY_ITEM_WIDGET_H

#include <QWidget>

class QLabel;
class QToolButton;
class QMouseEvent;
class QEvent;

class YACReaderLibraryItemWidget : public QWidget
{
	Q_OBJECT

public:
	YACReaderLibraryItemWidget(QString name, QString path, QWidget *parent = 0);
	QString name;
	QString path;

signals:
	void selected(QString,QString);
	void showOptions();
	
public slots:
	void select();
	void deselect();
	void setName(const QString & name);

private:

	QLabel * icon;
	QLabel * nameLabel;

	QToolButton * options;

	bool isSelected;

};

#endif // YACREADER_LIBRARY_ITEM_WIDGET_H
