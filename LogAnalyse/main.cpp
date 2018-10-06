#include <QCoreApplication>

#include <QTimer>
#include <QTextStream>
#include <QDebug>
#include <QString>
#include <iostream>
#include <QFile>
#include <QDateTime>
#include <QSet>
#include <QLocale>

#define SW_VERSION 1.01

struct t_histRec
{
    QString m_timestamp;
    int m_count;

    QString toPrint()
    {
        QString qsTemp = "%1 %2 %3\n";
        qsTemp = qsTemp.arg(m_timestamp).arg(' ',m_count+1, (QChar)'#').arg(m_count);
        return qsTemp;
    }
};

struct t_rec
{
    QString m_name;
    int m_count;
    long m_summ;
    float m_aver;

public:
    QString toPrint()
    {
        QString qsTemp = "Record Name: %1 Count:%2 Sum:%3 Aver:%4\n";
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

    QList<t_histRec> histogrammList;

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

    QString toPrintHistogramm()
    {
        QStringList qslTempList;
        QString qsTemp;
        QList<t_histRec>::iterator i;
        for (i = histogrammList.begin(); i != histogrammList.end(); ++i){
            qsTemp = (*i).toPrint();
            qslTempList.append(qsTemp);
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
            message = "Can't open the file %1 for reading\n";
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
    return 0;  //and we run the application
}

void printShortHelp(){
    QString message =  "For this appplication is nessesery to give 2 arguments \n"
                       "First log file name to analyse, and N - number of top resources. \n"
                       "Enter -h for help\n";
    std::cout << message.toLocal8Bit().data();
}

void printLongHelp(){
    QString message = "Log processing application. version %1 \n"
                      "For this appplication is nessesery to give 2 arguments \n"
                      "First log file name to analyse, and N - number of top resources. \n";
    message = message.arg(SW_VERSION, 0, 'f', 3);
    std::cout << message.toLocal8Bit().data();
}

void printVersion(){
    QString message = "Version %1 \n";
    message = message.arg(SW_VERSION, 0, 'f', 3);
    std::cout << message.toLocal8Bit().data();
}

int grabMinutes(QString time){  //string time in format '00:06:35,769'
    QStringList qsl = time.split(':');
    if (qsl.size()!=3) return -1;

    bool bOk = false;

    int minutes = qsl.at(1).toInt(&bOk);
    if (!bOk) return -2;

    if (minutes > 60) return -3;
    if (minutes < 0) return -4;
    return minutes;
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

    QString date;
    QString timeStamp;
    QString shortName;
    QString qsDur;
    QStringList split;

    bool bOk;
    int duration;
    int i = 0;
    int lineParts =0;

    int hisCount = 0;
    QString hisName = "";
    t_histRec lastHist;
    t_histRec newHisRec = {hisName, hisCount};
    notebook.histogrammList.append(newHisRec);

    while (oneLine!= nullptr) {
        i++;

        // oneLine is
        // 2015-08-19 00:06:16,835 (http--0.0.0.0-28080-391) [] /checkSession.do in 123

        split = oneLine.split(' ');

        // split list:
        // 0 - 2015-08-19
        // 1 - 00:06:16,835
        // 2 - (http--0.0.0.0-28080-391)
        // 3 - []
        // 4 - /checkSession.do
        // 5 - in
        // 6 - 123
        lineParts = split.size();
        if (lineParts <= 6) {
            std::cout << "skip this line: " << i << ", to short \n";
            continue;
        }

        date = split.at(0);                     // 0 - 2015-08-19
        timeStamp = split.at(1);                // 1 - 00:06:16,835
        hisName = date+" "+timeStamp.left(5);   // hisName = "2015-08-19 00:06"

        lastHist = notebook.histogrammList.last();
        if (hisName !=  lastHist.m_timestamp) { // we have new time stamp
            //qDebug () << " we have new time stamp ";
            t_histRec newHisRec = {hisName, 1};
            notebook.histogrammList.append(newHisRec);
            //std::cout << newHisRec.toPrint().toLocal8Bit().data()<< std::endl;

        }else { // this time stamp it the same as last, increase count
           // qDebug() << " this time stam it the same as last, increase count , now count is " << lastHist.m_count;
            notebook.histogrammList.last().m_count++;
         //   std::cout << lastHist.toPrint().toLocal8Bit().data()<< std::endl;
        }
       // std::cout << lastHist.toPrint().toLocal8Bit().data()<< std::endl;

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
    std::cout << message.toLocal8Bit().data();
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

    message = "\nLog histogramm: \n";
    std::cout << message.toLocal8Bit().data();

    message = notebook.toPrintHistogramm();
    std::cout << message.toLocal8Bit().data();


}
