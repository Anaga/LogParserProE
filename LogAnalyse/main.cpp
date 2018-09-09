#include <QCoreApplication>

#include <QTimer>
#include <QTextStream>
#include <QDebug>
#include <QString>
#include <iostream>
#include <QFile>
#include <QDateTime>
#include <QSet>

#define SW_VERSION 1.0

struct resurse
{
    QString n_name;
    int m_count;
    long m_summ;
    resurse(QString name, long dur) {
        n_name = name;
        m_summ = dur;
        m_count=1;
    }
    resurse() {
        n_name = "";
        m_summ = 0;
        m_count=0;
    }
};

void printShortHelp();
void printLongHelp();
void printVersion();
void analyseLog(QString fileName, unsigned int topResCount);


int main(int argc, char *argv[])
{
    bool readyToRun = false;
    int topN;
    QString message;
    QString fileName;

    //QCoreApplication app(argc, argv);
    if (argc <2) printShortHelp();
    if (argc == 2) {
        QString a2 = argv[1];
        if ( (a2=="-h") || (a2=="--help") ) printLongHelp();
        if ( (a2=="-v") || (a2=="--version") ) printVersion();
        else printShortHelp();
    }
    if (argc == 3) {
        QString a2 = argv[1];
        QFile file(a2);
        if (!file.exists()){
            message = "Can't find file %1 to read\n";
            message = message.arg(a2);
            std::cout << message.toLocal8Bit().data();
            return -1;
        }

        if(!file.open(QFile::ReadOnly | QFile::Text))  {
            message = "Can't open the file %1  for reading\n";
            message = message.arg(a2);
            std::cout << message.toLocal8Bit().data();
            return -1;
        }

        fileName = a2;

        QString a3 = argv[2];
        bool bOk;
        topN = a3.toInt(&bOk);
        if (!bOk) {
            message = "Can't parse arg %1 to integer\n";
            message = message.arg(a3);
            std::cout << message.toLocal8Bit().data();
            return -1;
        }        
        if (topN <= 0) {
            message = "N shall be positive\n";
            std::cout << message.toLocal8Bit().data();
            return -1;

        }
        file.close();
        readyToRun = true;

    }
    if (!readyToRun) return 0;

    analyseLog(fileName, topN);
    return 0; //app.exec(); //and we run the application
}

void printShortHelp(){
    QString message =  "For this appplication is nessesery to give 2 arguments \n"
                       "First log file name to analyse, and N - number of top resurses. \n"
                       "Enter -h for help\n";
    std::cout << message.toLocal8Bit().data();
}

void printLongHelp(){
    QString message = "Log processing application. version %1 \n"
                      "For this appplication is nessesery to give 2 arguments \n"
                      "First log file name to analyse, and N - number of top resurses. \n";
    message = message.arg(SW_VERSION, 0, 'f', 3);
    std::cout << message.toLocal8Bit().data();
}

void printVersion(){
    QString message = "Version %1 \n";
    message = message.arg(SW_VERSION, 0, 'f', 3);
    std::cout << message.toLocal8Bit().data();
}

QString extractResource(QString URI){
    std::cout << URI.toLocal8Bit().data() << std::endl;
    int index = 0;
    if (URI.contains(".do?action")){
        index = URI.indexOf("&");
        if (index > 1) return URI.left(index);
    }
    index = URI.indexOf(".do");
    if (index > 1) return URI.left(index+3);

    index = URI.indexOf(".jsp?");
    if (index > 1) return URI.left(index+4);

    index = URI.indexOf(".pdf?");
    if (index > 1) return URI.left(index+4);

    return URI;
}

void analyseLog(QString fileName, unsigned int topResCount){
    QDateTime qdtStart = QDateTime::currentDateTime();
    qint64 startMilSec = QDateTime::currentMSecsSinceEpoch();
    QFile file(fileName);

    qint64 iFileSize = file.size();
    QString message = "File size %1 in bytes\n";
    message = message.arg(iFileSize);
    std::cout << message.toLocal8Bit().data();

    file.open(QFile::ReadOnly | QFile::Text);

    QTextStream in(&file);
    QTextStream lineToParse;
    QString oneLine = in.readLine();

    QSet<QString> resurseSet;
    QVector<resurse> resurseVect;

    QVector<QString> resName;
    QVector<long>    resSumm;
    QVector<int>     resCount;


    QString a1,a2,a3,a4,a5,a6,a7;
    QString shortName;

    bool bOk;
    int duration;
    int i = 0;
    while (oneLine!= nullptr) {
        i++;
        lineToParse.setString(&oneLine);
        // 1000 rows can be printed out by 6176 milsec
        //qDebug() <<  i  <<topN << oneLine ;//<< std::endl;

        // 1000 rows can be printed out by 2301 milsec
        //std::cout <<  i <<' ' <<topResCount <<' '<< oneLine.toLocal8Bit().data() << std::endl;

        lineToParse >> a1 >> a2 >> a3 >> a4 >> a5 >> a6 >> a7;
        //std::cout << a1.toLocal8Bit().data() << a2.toLocal8Bit().data() << a3.toLocal8Bit().data()<< a4.toLocal8Bit().data() << std::endl;
        //std::cout << a5.toLocal8Bit().data() << " in "<< a7.toLocal8Bit().data() << std::endl;
        duration = a7.toInt(&bOk);
        if(!bOk) {
            std::cout << "can't convert " << a7.toLocal8Bit().data() << "to int \n";
            duration = 0;
        }
        shortName = extractResource(a5);
        if (!resurseSet.contains(shortName)){

            resName.append(shortName);
            resCount.append(1);
            resSumm.append(duration);

            resurseSet << shortName;
        } else {
            int index = resName.indexOf(shortName);
            if (index == -1) { std::cout << "CODE ERROR! \n"; return ;}
            resCount[index]++;
            resSumm[index]+=duration;

            int N = resCount.at(index);
            long S =  resSumm.at(index);
            float A = (float)S/N;
            std::cout << shortName.toLocal8Bit().data() << " count " << N << " summ "<< S <<" Aver "<< A << std::endl;

        }


        oneLine = in.readLine();

    }
    file.close();

    qDebug() << "resurseSet size is "<<resurseSet.size();
    QStringList qsl1 = resurseSet.toList();

    qsl1.sort();
    foreach (const QString &value, qsl1)
        qDebug() << value;

    QDateTime qdtStop = QDateTime::currentDateTime();
    qint64 stpoMilSec = QDateTime::currentMSecsSinceEpoch();
    qint64 delta = stpoMilSec - startMilSec;
    message = "Start time %1\n"
              "Stop  time %2\n"
              "Delta %3 milsec\n";
    QString qsTimeFormat = "hh:mm:ss.z";
    message = message.arg(qdtStart.toString(qsTimeFormat)).arg(qdtStop.toString(qsTimeFormat)).arg(delta);
    std::cout << message.toLocal8Bit().data();
}
