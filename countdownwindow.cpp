#include "countdownwindow.h"

CountdownWindow::CountdownWindow(QWidget *parent)
    : QWidget(parent)
{
    // 设置窗口属性为无边框、桌面层级置底、工具型窗口
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint | Qt::Tool);
    // 设置窗口背景完全透明
    setAttribute(Qt::WA_TranslucentBackground);
    // 窗口显示时不会激活, 没有焦点
    setAttribute(Qt::WA_ShowWithoutActivating);

    // 初始化标签
    label = new QLabel(this);
    label->setAlignment(Qt::AlignCenter);   // 设置标签文本在水平和垂直方向上都居中对齐

    // 设置红色字体（带阴影增强可读性）
    label->setStyleSheet("QLabel {color: red; font: bold 20px 'Microsoft YaHei', Arial, sans-serif;}");

    // 设置标签背景透明
    label->setAttribute(Qt::WA_TranslucentBackground);

    // 在构造函数中使用
    QString configPath = findConfigFile();
    QSettings *settings = nullptr;

    // 读取目标时间
    settings = new QSettings(configPath, QSettings::IniFormat);
    QString targetStr = settings->value("Time/Target").toString();
    if (targetStr.isEmpty())
    {
        qWarning() << "Using default (100 days from now).";
        targetDateTime = QDateTime::currentDateTime().addDays(100);
    }
    else
    {
        targetDateTime = QDateTime::fromString(targetStr, "yyyy-MM-ddTHH:mm:ss");
        if (!targetDateTime.isValid())
        {
            qWarning() << "Invalid target time format. Using default (100 days from now).";
            targetDateTime = QDateTime::currentDateTime().addDays(100);
        }
    }

    // 启动定时器
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CountdownWindow::updateCountdown);
    timer->start(100);   // 每1秒更新一次

    // 初始更新
    updateCountdown();
}

QString CountdownWindow::findConfigFile()
{
    // 方案1：优先检查可执行文件目录（打包后使用）
    QString appDirPath = QCoreApplication::applicationDirPath();    // 返回可执行文件.exe的目录
    QString appDirConfig = appDirPath + "/config.ini";
    if (QFile::exists(appDirConfig))
    {
        qDebug() << "Using config in application dir:" << appDirConfig;
        return appDirConfig;
    }

    // 方案2：检索项目所在文件夹的配置文件（开发时使用）
    QDir currentDir = QDir::current(); // 获取当前工作目录
    if (currentDir.cdUp())
    {
        QString projectDirName = "Countdown"; // 替换为您的实际项目文件夹名称
        if (currentDir.cd(projectDirName))
        {
            // 检查配置文件是否存在
            QString projectConfig = currentDir.absoluteFilePath("config.ini");
            if (QFile::exists(projectConfig))
            {
                qDebug() << "Found config file in project directory:" << projectConfig;
                return projectConfig;
            }
        }
    }

    // 方案3：使用默认配置
    qWarning() << "No config file found, using default settings";
    return QString();
}

void CountdownWindow::updateCountdown()
{
    qint64 secondsLeft = QDateTime::currentDateTime().secsTo(targetDateTime);
    if (secondsLeft < 0) secondsLeft = 0;  // 防止负数

    // 计算天数（精确到小数点后6位）
    double daysLeft = static_cast<double>(secondsLeft) / 86400.0;

    // 将天数拆分为整数和小数部分
    QString daysStr = QString::number(daysLeft, 'f', 6);
    int decimalIndex = daysStr.indexOf('.');
    QString integerPart = daysStr.left(decimalIndex);
    QString decimalPart = daysStr.mid(decimalIndex);

    // 创建富文本（整数部分放大）
    QString text = QString("<span style='font-size:35px;'>%1</span><span style='font-size:20px;'>%2 天</span>")
                       .arg(integerPart, decimalPart);
    label->setText(text);

    // 每100次刷新才调整一次窗口大小（约1秒1次）
    static int refreshCount = 0;
    if (refreshCount++ % 100 == 0) {
        // 自适应大小
        QFontMetrics fm(label->font());
        int width = fm.horizontalAdvance(integerPart + decimalPart + " 天") + 100;
        label->setFixedSize(width, 50);
        setFixedSize(width, 50);

        // 确保位置正确
        moveToTopRight();
    }
}

void CountdownWindow::moveToTopRight()
{
    QScreen *screen = QApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    int x = screenGeometry.width() - width() - 50;   // 右边距50像素
    move(x, 50);  // 上边距50像素
}

void CountdownWindow::resizeEvent(QResizeEvent *)
{
    label->resize(size());
}
