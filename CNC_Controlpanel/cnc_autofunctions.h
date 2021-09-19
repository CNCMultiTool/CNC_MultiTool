#ifndef CNC_AUTOFUNCTIONS_H
#define CNC_AUTOFUNCTIONS_H

#include <QObject>

#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <math.h>
#include <cnc_data.h>


class cnc_autofunctions : public QObject
{
    Q_OBJECT
public:
    cnc_autofunctions(cnc_data *database = nullptr);
    ~cnc_autofunctions();

    void GC_open(QString fileName);
    QByteArray GC_getNextLine();
    void GC_close();

    void G_Code_Start(QString fileName);
    void G_Code_Parser(QString newLine);



private:
    QFile m_inputFile;
    QTextStream m_in;
    cnc_data* m_database = nullptr;

    quint64 m_lineCount = 0;

    QByteArrayList m_LineBuffer;

    bool getValue(const QString indent,const QString line,float *target);
    bool isCommand(const QString indent,const QString line);
    QByteArray createValueArray(QString line);
    void GC_readNextLine();

signals:
    void Log(const QString &s);
    void errorLog(const QString &s);
    void show_state(quint64);
    void serial_send(const QByteArray &mes);
};

#endif // CNC_AUTOFUNCTIONS_H
