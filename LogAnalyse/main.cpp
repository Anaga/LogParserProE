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

struct t_rec
{
    QString m_name;
    int m_count;
    long m_summ;
    float m_aver;

public:
    QString toPrint()
    {
        QString qsTemp = "N: %1 C:%2 S:%3 A:%4\n";
        return qsTemp
                .arg(m_name)
                .arg(m_count)
                .arg(m_summ)
                .arg(m_aver);}

    void calcAver()
    { m_aver = m_summ/(float)m_count; }

    void updateRecord(long duration)
    {
        m_count++;
        m_summ+=duration;
        calcAver();
    }
};

struct t_allResurces
{
    t_allResurces() {}
    QVector<t_rec> myVect;
    QMap <QString, float> sortedMap;
    QList <t_rec> sortedList;
    //QLinkedList<t_rec> sortedList;

public:
    void addOrUpdateRecord(QString resName, long resDuration)
    {
        float average = -1.0;
        bool resExist = false;
        QVector<t_rec>::iterator i;
        for (i = myVect.begin(); i != myVect.end(); ++i){
            if( resName == (*i).m_name ){
                (*i).updateRecord(resDuration);
                average = (*i).m_aver;
                resExist = true;
                break;
            }
        }
        if (!resExist) {
            t_rec newRes = {resName, 1, resDuration, (float)resDuration};
            myVect.append(newRes);
            average = newRes.m_aver;
        }
        sortedMap.insert(resName, average);
    }

    void fillSortedList(int topN)
    {
        bool findPos=false;
        int vecInd;
        int listIndex;
        t_rec vecRec, listRec;
        qDebug() << "myVect size is" << myVect.size();
        qDebug() << "sortedList size is" << sortedList.size();
        for (vecInd =0; vecInd < myVect.size(); ++vecInd){
            findPos=false;
            vecRec = myVect.at(vecInd);
            for (listIndex = 0; listIndex < sortedList.size(); ++listIndex){
                listRec = sortedList.at(listIndex);
                if (vecRec.m_aver > listRec.m_aver){
                    sortedList.insert(listIndex,vecRec);
                    findPos=true;
                    break;
                }
            }
            if (!findPos) {sortedList.append(vecRec);}
        }

        qDebug() << "sortedList size is" << sortedList.size();
        qDebug() << "Cut all exept N top records " << topN;

        for (listIndex = 0; listIndex < sortedList.size() && listIndex < topN; ++listIndex){
            qDebug() << sortedList[listIndex].toPrint();
        }



    }

    int size() {return myVect.size();}

    QString toPrintAll()
    {
        QStringList qslTempList;
        QString qsTemp;
        QVector<t_rec>::iterator i;
        for (i = myVect.begin(); i != myVect.end(); ++i){
            qsTemp = (*i).toPrint();
            qslTempList.append(qsTemp);
        }
        return qslTempList.join("");
    }

    QString toPrintTopN(int N)
    {
        QStringList qslTempList;
        QString qsTemp = "%1: resurse %2, average use %3 \n";
        int i=0;
        QMap<QString, float>::const_iterator it;
        for (it = sortedMap.constBegin(); it != sortedMap.constEnd() && i<N; ++it, ++i){
            qsTemp = qsTemp.arg(i).arg(it.key()).arg(it.value());
            qDebug() << qsTemp;
            qslTempList.append(qsTemp);
            qsTemp = "%1: resurse %2, average use %3 \n";
        }
        return qslTempList.join("");
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
    //std::cout << URI.toLocal8Bit().data() << std::endl;
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

    t_allResurces notebook;

    qint64 iFileSize = file.size();
    QString message = "File size %1 in bytes\n";
    message = message.arg(iFileSize);
    std::cout << message.toLocal8Bit().data();

    file.open(QFile::ReadOnly | QFile::Text);

    QTextStream in(&file);
    QString oneLine = in.readLine();


    QString a1,a2,a3,a4,a5,a6,a7;
    QString shortName;
    QString qsDur;
    QStringList split;

    bool bOk;
    int duration;
    int i = 0;
    int lineParts =0;
    while (oneLine!= nullptr) {
        i++;

        split = oneLine.split(' ');
        lineParts = split.size();
        if (lineParts < 6) {
            std::cout << "skip this line: " << i << ", to short \n";
            continue;
        }

        if (split.at(lineParts-2) != "in"){
            std::cout << "skip this line: " << i << ", no 'in' in the end \n";
            continue;
        }
        qsDur = split.at(lineParts-1);

        duration = qsDur.toInt(&bOk);
        if(!bOk) {
            std::cout <<"skip this line: " << i << ", can't convert " << qsDur.toLocal8Bit().data() << "to int \n";
            continue;
        }

        shortName = extractResource(split.at(4));
        notebook.addOrUpdateRecord(shortName,duration);

        oneLine = in.readLine();
    }
    file.close();

    message = notebook.toPrintAll();
    std::cout << message.toLocal8Bit().data();

    message = notebook.toPrintTopN(topResCount);
    //std::cout << message.toLocal8Bit().data();
    notebook.fillSortedList(topResCount);


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
