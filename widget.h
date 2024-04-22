#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
	Q_OBJECT
signals:
	void haveANewTargetPoint(QPoint *new_point);
	void pointsObtained();
public:
	Widget(QWidget *parent = nullptr);
	~Widget();
	void keyPressEvent(QKeyEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void paintEvent(QPaintEvent *event);
public slots:
private:
	Ui::Widget *ui;
	QPen *bezier_pen, *user_point_pen, *aux_point_pen;
	QPoint *new_point, *p00;
	QVector<QVector2D> *bezier_points;
	QVector<QVector2D> *user_points;
	QVector<QVector2D> *aux_points;
	void build_bezier();
private slots:
	void collect_points(QPoint *new_point);
};
#endif // WIDGET_H

