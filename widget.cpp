#include "widget.h"
#include "./ui_widget.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>

Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget)
{
	bezier_pen=new QPen();
	bezier_pen->setWidth(2);
	bezier_pen->setColor(QColor(12, 12, 230));

	user_point_pen=new QPen();
	user_point_pen->setWidth(1);
	user_point_pen->setColor(QColor(12, 170, 12));

	aux_point_pen=new QPen();
	aux_point_pen->setWidth(1);
	aux_point_pen->setColor(QColor(170, 12, 12));

	p00=new(QPoint);

	bezier_points=new QVector<QVector2D>;
	user_points=new QVector<QVector2D>;
	aux_points=new QVector<QVector2D>;

	ui->setupUi(this);
	this->setWindowTitle("Bezier");

	connect(this, SIGNAL(haveANewTargetPoint(QPoint *)), this, SLOT(collect_points(QPoint *)));
	connect(this, SIGNAL(pointsObtained()), this, SLOT(repaint()));
}

Widget::~Widget()
{
	delete ui;
}

void Widget::keyPressEvent(QKeyEvent *event)
{
	if(event->key()==Qt::Key_F11)
	{
		event->accept();
		if(this->isFullScreen())
		{
			this->showNormal();
		}
		else
		{
			this->showFullScreen();
		}
	}
}

void Widget::mousePressEvent(QMouseEvent *event)
{
	event->accept();
	*p00=QPoint(this->width()/2, this->height()/2);
	new_point=new(QPoint);
	*new_point=event->pos()-*p00;
	emit(haveANewTargetPoint(new_point));
}

void Widget::paintEvent(QPaintEvent *event)
{
	uint d=5, point_in;
	QPainter local_painter(this);
	event->accept();

	local_painter.setRenderHint(QPainter::Antialiasing);

	local_painter.setPen(*user_point_pen);
	if(user_points->size())
	{
		for(point_in=0; point_in<user_points->size(); point_in++)
		{
			local_painter.drawEllipse(*p00+QPoint(user_points->at(point_in).x(), user_points->at(point_in).y()), d, d);
		}
	}

	local_painter.setPen(*aux_point_pen);
	if(aux_points->size())
	{
		for(point_in=0; point_in<aux_points->size(); point_in++)
		{
			local_painter.drawEllipse(*p00+QPoint(aux_points->at(point_in).x(), aux_points->at(point_in).y()), d, d);
		}
	}

	local_painter.setPen(*bezier_pen);

	if(bezier_points->size()>2)
	{
		for(point_in=0; point_in<bezier_points->size()-1; point_in++)
		{
			local_painter.drawLine(*p00+QPoint(bezier_points->at(point_in).x(), bezier_points->at(point_in).y()),
								   *p00+QPoint(bezier_points->at(point_in+1).x(), bezier_points->at(point_in+1).y()));
		}
	}
}

QVector2D lint(QVector2D pa, QVector2D pb, double c)
{
	QVector2D res;
	double dX=pb.x()-pa.x();
	double dY=pb.y()-pa.y();
	res.setX(pa.x()+dX*c);
	res.setY(pa.y()+dY*c);
	return(res);
}

void Widget::build_bezier()
{
	QVector2D ap=lint(user_points->at(0), user_points->at(2), 0.5);
	QVector2D bezier_control_p=user_points->at(1)*2-ap;
	QVector2D ip0, ip1, bp;
	double c;

	aux_points->append(bezier_control_p);
	aux_points->append(ap);

	for(c=-1.0; c<=2.0; c+=1.0/64.0)
	{
		ip0=lint(user_points->at(0), bezier_control_p, c);
		ip1=lint(bezier_control_p, user_points->at(2), c);
		bp=lint(ip0, ip1, c);
		bezier_points->append(bp);
	}
}

void Widget::collect_points(QPoint *new_point)
{
	user_points->append(QVector2D(*new_point));
	delete(new_point);
	if(user_points->size()>=3)
	{
		build_bezier();
	}
	emit(pointsObtained());
}
