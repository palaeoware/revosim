#ifndef DARKSTYLETHEME_H
#define DARKSTYLETHEME_H

#include <QApplication>
#include <QProxyStyle>
#include <QStyleFactory>
#include <QFont>
#include <QFile>

class DarkStyleTheme : public QProxyStyle
{
  Q_OBJECT

public:
    DarkStyleTheme();
    explicit DarkStyleTheme(QStyle *style);

    QStyle *baseStyle() const;

    void polish(QPalette &palette) override;
    void polish(QApplication *app) override;

private:
    QStyle *styleBase(QStyle *style = Q_NULLPTR) const;
};

#endif // DARKSYLETHEME_H
