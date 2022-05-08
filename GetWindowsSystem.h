#include <Windows.h>
#include <QProcess>
#include <QString>

QString GetWindowsSystem()
{
    // 由于windows重定向原因，32位进程运行在64位系统时注册表会被重定向到WOW6432Node，这会导致操作系统信息获取异常
    void* pValue = NULL;
    // 先关掉重定向
    Wow64DisableWow64FsRedirection(&pValue);

    // 该方式简单测试过效率问题，如果后续获取缓慢的话需要改成提前异步处理的方式
    QStringList argument;
    argument << "/c" << "reg" << "query" << "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion" << "/v" << "ProductName";
    QProcess process;
    process.start("cmd.exe", argument);
    process.waitForFinished(5 * 1000);
    QString output = process.readAllStandardOutput();
    QString osVersion;
    if (!output.isEmpty())
    {
        int indexTmp = output.lastIndexOf("Win");
        int lenTmp = output.length() - indexTmp;
        osVersion = output.mid(indexTmp, lenTmp);
    }

    // 再还原重定向
    Wow64RevertWow64FsRedirection(pValue);

    if (osVersion.isEmpty())
    {
        osVersion = "Computer OS: UnKnown";
    }

    return osVersion.trimmed();
}