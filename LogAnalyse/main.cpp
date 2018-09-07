#include <QCoreApplication>

#include <QTimer>
#include <QTextStream>
#include <QDebug>
#include <QString>
#include <iostream>
#include <QFile>
#include <QDateTime>

#define SW_VERSION 1.0

void printShortHelp();
void printLongHelp();
void printVersion();

int main(int argc, char *argv[])
{
    bool readyToRun = false;
    int topN;
    QString message;
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

        QString a3 = argv[2];
        bool bOk;
        topN = a3.toInt(&bOk);
        if (!bOk) {
            message = "Can't parse arg %1 to integer\n";
            message = message.arg(a3);
            std::cout << message.toLocal8Bit().data();
            return -1;
        }
        QDateTime qdtStart = QDateTime::currentDateTime();
        qint64 startMilSec = QDateTime::currentMSecsSinceEpoch();


        QTextStream in(&file);
        QString oneLine = in.readLine();
        int i = 0;
        while (oneLine!= nullptr) {
            i++;
            // 1000 rows can be printed out by 6176 milsec
            //qDebug() <<  i  <<topN << oneLine ;//<< std::endl;

            // 1000 rows can be printed out by 2301 milsec
            std::cout <<  i <<' ' <<topN <<' '<< oneLine.toLocal8Bit().data() << std::endl;
            oneLine = in.readLine();
        }
        file.close();

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
    if (!readyToRun) return 0;
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
