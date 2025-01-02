#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <QDialog>
#include "global.h"

namespace Ui {
class ResetDialog;
}

class ResetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResetDialog(QWidget *parent = nullptr);
    ~ResetDialog();

signals:
    void changeLogin();

private slots:
    void slot_reset_mod_finish(ReqId id, QString res, ErrorCode err);

    void on_get_btn_clicked();

    void on_cancle_btn_clicked();

    void on_sure_btn_clicked();

private:
    bool checkUserValid();
    bool checkEmailValid();
    bool checkPassValid();
    bool checkVarifyValid();
    void initHttpHandlers();
    void AddTipErr(TipErr te,QString tips);
    void DelTipErr(TipErr te);
    void showTip(QString str, bool b_ok);

    QMap<TipErr, QString> _tip_errs;
    QMap<ReqId, std::function<void(const QJsonObject&)>> _handlers;
    Ui::ResetDialog *ui;
};

#endif // RESETDIALOG_H
