#include <QCoreApplication>

#include <QTimer>
#include <QTextStream>
#include <QDebug>
#include <QString>
#include <iostream>

#define SW_VERSION 1.0

void printShortHelp();
void printLongHelp();
void printVersion();

int main(int argc, char *argv[])
{
    bool readyToRun = false;
    QCoreApplication app(argc, argv); //renamed the a to app
    if (argc <2) printShortHelp();
    if (argc == 2) {
        QString a2 = argv[1];
        if ( (a2=="-h") || (a2=="--help") ) printLongHelp();
        if ( (a2=="-v") || (a2=="--version") ) printVersion();
        else printShortHelp();
    }
    if (argc == 3) {
        QString a2 = argv[1];
        QString a3 = argv[2];

    }
    if (!readyToRun) return 0;
    return app.exec(); //and we run the application
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
