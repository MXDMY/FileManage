#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("轻量文件管理 Beta 1.0");
    this->setWindowIcon(QIcon(":/icon/windowIcon.png"));

    trayIcon=new QSystemTrayIcon(QIcon(":/icon/windowIcon.png"),this);
    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
    trayIcon->show();

    clipBoard=QApplication::clipboard();    //获得系统剪切板

    progressDialog=new ProgressDialog(this);
    progressDialog->setProgressBarRange(0,0);
    progressDialog->setProgressBarTextVisible(false);
    progressDialog->setModal(true);
    connect(progressDialog,SIGNAL(interruptReceived()),this,SLOT(progressInterrupted()));

    fileSysCpThread=new FMFileSystemCopyThread(this);
    connect(fileSysCpThread,SIGNAL(finished()),this,SLOT(fileSysCpFinished()));

    fileSysDelThread=new FMFileSystemDelThread(this);
    connect(fileSysDelThread,SIGNAL(finished()),this,SLOT(fileSysDelFinished()));

    initTreeViewMenu();
    initListViewMenu();
    initRootListMenu();
    initTrayIconMenu();
    initWidgetSetting();
    setFromINI();

    //安装事件过滤器，不能给自己安装，需要接收谁的事件，就给谁安装
    //this->installEventFilter(this);
    //启用鼠标跟踪，鼠标移动事件不需要鼠标持续按住接收，但该函数对于MainWindow或某些控件有时候无用处，原因未知
    //this->setMouseTracking(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initWidgetSetting()
{
    ui->toolBar->setIconSize(QSize(15,15));

    rootListModel=new QStandardItemModel(ui->listView);

    ui->listView->setModel(rootListModel);
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showRootListMenu(QPoint)));

    ui->tabWidget->setTabText(0,"主页");
    ui->tabWidget->setTabText(1,"设置");

    ui->frame->setFrameShape(QFrame::Box);      //设置路径栏所在的frame有BOX框架
    ui->frame->setFixedHeight(25);

    ui->lineEdit->setFrame(false);              //设置路径栏无框架
    ui->lineEdit->setFixedHeight(22);
    ui->lineEdit->setReadOnly(true);

    ui->lineEdit_2->setFrame(false);
    ui->lineEdit_2->setFixedHeight(22);
    ui->lineEdit_2->setReadOnly(true);

    ui->label->setText("");
    ui->label->setFixedSize(22,22);
    ui->label->setScaledContents(true);         //允许标签自动缩放其内容以填充空间

    ui->label_2->setText("");
    ui->label_2->setFixedSize(22,22);
    ui->label_2->setScaledContents(true);

    treeModel=new FMFileSystemModel(ui->treeView);
    treeModel->setRootPath(QDir::currentPath());
    treeModel->setResolveSymlinks(false);       //设置不自动解析链接文件，只与Windows系统相关

    ui->treeView->setModel(treeModel);          //设置目录树视图的模型
    ui->treeView->installEventFilter(this);
    //ui->treeView->setMouseTracking(true);     //鼠标跟踪对QTreeView与FMListView作用似乎有问题
    ui->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->treeView->setSelectionMode(QAbstractItemView::ContiguousSelection);//设置鼠标拖拉多选
    connect(ui->treeView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showTreeViewMenu(QPoint)));

    ui->tab->installEventFilter(this);
    ui->tab->setMouseTracking(true);

    tabWidget2AddTabButton=new QToolButton(ui->tabWidget_2);
    tabWidget2AddTabButton->setIcon(QIcon(":/icon/add.png"));
    tabWidget2AddTabButton->setAutoRaise(true);
    connect(tabWidget2AddTabButton,SIGNAL(clicked(bool)),this,SLOT(tabWidget2AddTab()));

    tabWidget2BackButton=new QToolButton(ui->tabWidget_2);
    tabWidget2BackButton->setIcon(QIcon(":/icon/back.png"));
    tabWidget2BackButton->setAutoRaise(true);
    connect(tabWidget2BackButton,SIGNAL(clicked(bool)),this,SLOT(tabWidget2Back()));

    ui->tabWidget_2->setCornerWidget(tabWidget2AddTabButton);
    ui->tabWidget_2->setCornerWidget(tabWidget2BackButton,Qt::TopLeftCorner);
    ui->tabWidget_2->setTabsClosable(true);     //自动向新增的Tab添加关闭选项
    connect(ui->tabWidget_2,SIGNAL(tabCloseRequested(int)),this,SLOT(tabCloseRequestedSlot(int)));
    ui->tabWidget_2->setElideMode(Qt::ElideRight);//设置Tab文本过长时省略
    ui->tabWidget_2->setMovable(true);            //设置Tab可移动
    tabWidget2AddTab();
}

void MainWindow::setFromINI()
{
    setting=new QSettings(QApplication::applicationDirPath()+"/FileManage.ini",QSettings::IniFormat,this);

    if(setting->value("保存根目录列表").toBool())
    {
        ui->checkBox->setCheckState(Qt::Checked);

        setting->beginGroup("根目录");
        QStringList rootList=setting->allKeys();
        QFileInfo fileInfo;
        for(int i=0;i<rootList.count();i++)
        {
            fileInfo.setFile(rootList.at(i));
            if(fileInfo.exists() && fileInfo.isDir() && !fileInfo.isSymLink())
                rootListAddItem(rootList.at(i));
        }
        setting->endGroup();
    }
    else
        ui->checkBox->setCheckState(Qt::Unchecked);
}

QStandardItem* MainWindow::rootListAddItem(QString rootDir)
{
    QFileInfo fileInfo(rootDir);
    QStandardItem* item=new QStandardItem();
    if(fileInfo.isRoot())
    {
        item->setText(QDir::toNativeSeparators(rootDir));
    }
    else
    {
        item->setText(fileInfo.fileName());
    }
    item->setToolTip(QDir::toNativeSeparators(rootDir));
    QFileIconProvider fileIconProvider;
    item->setIcon(fileIconProvider.icon(fileInfo));
    rootListModel->appendRow(item);
    return item;
}

void MainWindow::on_actionSetRootDir_triggered()
{
    QString rootDir=QFileDialog::getExistingDirectory(this,"设置目录");
    if(rootDir!="")
    {
        //设置 ui->listView的新增项
        QStandardItem* item=rootListAddItem(rootDir);

        ui->listView->setCurrentIndex(rootListModel->indexFromItem(item));
        ui->treeView->setRootIndex(treeModel->index(rootDir));
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    static bool treeViewDrag=false;                     //treeView控件右侧侧面边缘是否可拖动
    static int beforeMouseX=0;                          //记录鼠标以前的X坐标，用于侧面边缘拖动

    static bool treeViewDrag_2=false;                   //treeView控件顶部边缘是否可拖动
    static int beforeMouseY=0;                          //记录鼠标以前的Y坐标，用于顶部边缘拖动

    static bool changeable=true;                        //鼠标是否可改变外观

    if(watched==ui->tab)
    {
        if(event->type()==QEvent::MouseMove)
        {
            QMouseEvent* e=static_cast<QMouseEvent*>(event);
            if(changeable)
            {
                if(isDragRegion(e))
                    this->setCursor(QCursor(Qt::SplitHCursor));
                else if(isDragRegion_2(e))
                    this->setCursor(QCursor(Qt::SplitVCursor));
                else
                    this->setCursor(QCursor(Qt::ArrowCursor));
            }

            if(treeViewDrag)
            {
                int changeMouseX=e->globalPos().x()-beforeMouseX;
                beforeMouseX=e->globalPos().x();

                //防止控件变形过于严重
                bool a=changeMouseX<0;
                bool b=ui->treeView->frameGeometry().width()<=100;
                bool c=changeMouseX>0;
                bool d=ui->tabWidget_2->frameGeometry().width()<=200;
                if(!( (a&&b) || (c&&d) ))
                {
                    ui->listView->setFixedWidth(ui->treeView->frameGeometry().width()+changeMouseX);//保持同步
                    ui->treeView->setFixedWidth(ui->treeView->frameGeometry().width()+changeMouseX);
                }
            }

            if(treeViewDrag_2)
            {
                int changeMouseY=e->globalPos().y()-beforeMouseY;
                beforeMouseY=e->globalPos().y();

                //防止控件变形过于严重
                bool a=changeMouseY<0;
                bool b=ui->listView->frameGeometry().height()<=50;
                bool c=changeMouseY>0;
                bool d=ui->treeView->frameGeometry().height()<=100;
                if(!( (a&&b) || (c&&d) ))
                {
                    ui->listView->setFixedHeight(ui->listView->frameGeometry().height()+changeMouseY);
                }
            }
        }
        if(event->type()==QEvent::MouseButtonPress)
        {
            QMouseEvent* e=static_cast<QMouseEvent*>(event);
            if(e->button()==Qt::LeftButton)
            {
                if(isDragRegion(e))
                {
                    treeViewDrag=true;
                    changeable=false;
                }
                if(isDragRegion_2(e))
                {
                    treeViewDrag_2=true;
                    changeable=false;
                }
                beforeMouseX=e->globalPos().x();
                beforeMouseY=e->globalPos().y();
            }
        }
        if(event->type()==QEvent::MouseButtonRelease)
        {
            QMouseEvent* e=static_cast<QMouseEvent*>(event);
            if(e->button()==Qt::LeftButton)
            {
                treeViewDrag=false;
                changeable=true;
                treeViewDrag_2=false;
            }
        }
    }
    else//收到其它控件消息，立即改变鼠标外形为箭头，防止鼠标移动过快导致鼠标外形未能即时改变
    {
        if(changeable)
            this->setCursor(QCursor(Qt::ArrowCursor));
    }

    return QObject::eventFilter(watched,event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //下面value()语句应该放在beginGroup()前面，
    //因为键："保存根目录列表" 不在 组："根目录" 里，
    //使用value函数会返回默认值0
    bool b=setting->value("保存根目录列表").toBool();
    setting->beginGroup("根目录");
    setting->remove("");        //清空当前组，以便后续更新键值对
    if(b)
    {
        QString temp;
        for(int i=0;i<rootListModel->rowCount();i++)
        {
            temp=rootListModel->item(i)->toolTip();
            setting->setValue(temp,"");
        }
    }
    setting->endGroup();

    QMainWindow::closeEvent(event);
}

bool MainWindow::isDragRegion(QMouseEvent *e)
{
    int mx=e->globalPos().x();                                       //获得鼠标的全局坐标
    int my=e->globalPos().y();
    int tabWidget2X=ui->tabWidget_2->mapToGlobal(QPoint(0,0)).x();//获得控件的全局坐标X
    int tabWidget2Y=ui->tabWidget_2->mapToGlobal(QPoint(0,0)).y();//获得控件的全局坐标Y
    QRect tabWidget2Rect=ui->tabWidget_2->frameGeometry();           //获得控件的整体几何结构，包括控件的客户区
    //判断鼠标的位置，是否位于可以拖动控件边缘的区域
    bool a=tabWidget2X-4<=mx&&mx<=tabWidget2X-2;
    bool b=tabWidget2Y<=my&&my<=tabWidget2Y+tabWidget2Rect.height();
    return a&&b;
}

bool MainWindow::isDragRegion_2(QMouseEvent *e)
{
    int mx=e->globalPos().x();
    int my=e->globalPos().y();
    int rootListViewX=ui->listView->mapToGlobal(QPoint(0,0)).x();
    int rootListViewY=ui->listView->mapToGlobal(QPoint(0,0)).y();
    QRect rootListViewRect=ui->listView->frameGeometry();
    bool a=rootListViewX<=mx&&mx<=rootListViewX+rootListViewRect.width();
    int si=rootListViewY+rootListViewRect.height();
    bool b=si+2<=my&&my<=si+4;
    return a&&b;
}

void MainWindow::on_treeView_clicked(const QModelIndex &index)
{
    QString path=treeModel->filePath(index);
    int curTabIndex=ui->tabWidget_2->currentIndex();

    tabWidget2UpdateTab(path,curTabIndex);
}

void MainWindow::tabWidget2AddTab()
{
    FMListView* listView=new FMListView();
    FMFileSystemModel* listModel=new FMFileSystemModel(listView);
    listModel->setRootPath(QDir::currentPath());
    listModel->setResolveSymlinks(false);
    listModel->setProgressDialog(progressDialog);
    listModel->setFileSysCpThread(fileSysCpThread);
    listView->setFrameShape(QFrame::NoFrame);           //隐藏外框
    listView->setModel(listModel);
    listView->setSpacing(10);                           //设置间距
    listView->setViewMode(FMListView::IconMode);        //View模式为图标模式
    listView->setIconSize(QSize(60,60));                //图标大小
    listView->setResizeMode(FMListView::Adjust);        //当View被重绘时，同时重新调整所有item项
    listView->setMovement(FMListView::Snap);            //设置item的移动模式为网格，默认为free自由移动
    listView->setGridSize(QSize(60,80));                //设置网格大小
    listView->installEventFilter(this);
    listView->setContextMenuPolicy(Qt::CustomContextMenu);
    listView->setSelectionMode(QAbstractItemView::ContiguousSelection);
    //listView移动模式设置setMovement为Snap或Free（默认）后，会自动启用下列函数
    //listView->setAcceptDrops(true);
    //listView->setDragEnabled(true);
    //listView->setDropIndicatorShown(true);
    //listView->setDragDropMode(QAbstractItemView::DragDrop);

    connect(listView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(showListViewMenu(QPoint)));

    connect(listView,SIGNAL(clicked(QModelIndex)),this,SLOT(listViewClickedSlot(QModelIndex)));
    connect(listView,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(listViewDoubleClickedSlot(QModelIndex)));

    ui->tabWidget_2->addTab(listView,"     ");
}

void MainWindow::tabWidget2Back()
{
    int curTabIndex=ui->tabWidget_2->currentIndex();
    QString curPath=ui->tabWidget_2->tabToolTip(curTabIndex);

    tabWidget2UpdateTab(QFileInfo(curPath).absolutePath(),curTabIndex);
}

void MainWindow::listViewClickedSlot(const QModelIndex &index)
{
    FMListView* listView=(FMListView*)ui->tabWidget_2->currentWidget();
    FMFileSystemModel* listModel=(FMFileSystemModel*)listView->model();

    int curTabIndex=ui->tabWidget_2->currentIndex();
    QIcon tabIcon=ui->tabWidget_2->tabIcon(curTabIndex);
    ui->label->setPixmap(tabIcon.pixmap(ui->label->size()));
    ui->lineEdit->setText(ui->tabWidget_2->tabToolTip(curTabIndex));

    QString fileName=listModel->fileName(index);
    QIcon fileIcon=listModel->fileIcon(index);
    ui->label_2->setPixmap(fileIcon.pixmap(ui->label_2->size()));
    ui->lineEdit_2->setText(fileName);
}

void MainWindow::listViewDoubleClickedSlot(const QModelIndex &index)
{
    FMListView* listView=(FMListView*)ui->tabWidget_2->currentWidget();
    FMFileSystemModel* listModel=(FMFileSystemModel*)listView->model();
    QFileInfo fileInfo=listModel->fileInfo(index);
    if(fileInfo.isFile()||fileInfo.isSymLink())
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    }
    else
    {
        listView->setRootIndex(index);
        int curTabIndex=ui->tabWidget_2->currentIndex();
        QIcon icon=listModel->fileIcon(index);
        ui->tabWidget_2->setTabIcon(curTabIndex,icon);
        ui->tabWidget_2->setTabToolTip(curTabIndex,QDir::toNativeSeparators(fileInfo.absoluteFilePath()));
        if(fileInfo.isRoot())
        {
            ui->tabWidget_2->setTabText(curTabIndex,QDir::toNativeSeparators(fileInfo.absoluteFilePath()));
        }
        else
        {
            ui->tabWidget_2->setTabText(curTabIndex,fileInfo.fileName());
        }
    }
}

void MainWindow::initTreeViewMenu()
{
    treeViewMenu=new QMenu(ui->treeView);
    treeViewCopy=new QAction(QIcon(":/icon/copy.png"),"复制",treeViewMenu);
    treeViewPaste=new QAction(QIcon(":/icon/paste.png"),"粘贴",treeViewMenu);
    treeViewDel=new QAction(QIcon(":/icon/del.png"),"删除",treeViewMenu);
    treeViewShowInExplorer=new QAction(QIcon(":/icon/showInExplorer.png"),"在资源管理器中显示",treeViewMenu);
    treeViewRename=new QAction(QIcon(":/icon/rename.png"),"重命名",treeViewMenu);

    treeViewMenu->addAction(treeViewCopy);
    treeViewMenu->addAction(treeViewPaste);
    treeViewMenu->addAction(treeViewDel);
    treeViewMenu->addAction(treeViewShowInExplorer);
    treeViewMenu->addAction(treeViewRename);

    connect(treeViewCopy,SIGNAL(triggered(bool)),this,SLOT(treeViewCopySlot(bool)));
    connect(treeViewPaste,SIGNAL(triggered(bool)),this,SLOT(treeViewPasteSlot(bool)));
    connect(treeViewShowInExplorer,SIGNAL(triggered(bool)),this,SLOT(treeViewShowInExplorerSlot(bool)));
    connect(treeViewRename,SIGNAL(triggered(bool)),this,SLOT(treeViewRenameSlot(bool)));
    connect(treeViewDel,SIGNAL(triggered(bool)),this,SLOT(treeViewDelSlot(bool)));
}

void MainWindow::initListViewMenu()
{
    listViewMenu=new QMenu(ui->tabWidget_2);
    listViewCopy=new QAction(QIcon(":/icon/copy.png"),"复制",listViewMenu);
    listViewPaste=new QAction(QIcon(":/icon/paste.png"),"粘贴",listViewMenu);
    listViewDel=new QAction(QIcon(":/icon/del.png"),"删除",listViewMenu);
    listViewShowInExplorer=new QAction(QIcon(":/icon/showInExplorer.png"),"在资源管理器中显示",listViewMenu);
    listViewRename=new QAction(QIcon(":/icon/rename.png"),"重命名",listViewMenu);

    listViewMenu->addAction(listViewCopy);
    listViewMenu->addAction(listViewPaste);
    listViewMenu->addAction(listViewDel);
    listViewMenu->addAction(listViewShowInExplorer);
    listViewMenu->addAction(listViewRename);

    connect(listViewCopy,SIGNAL(triggered(bool)),this,SLOT(listViewCopySlot(bool)));
    connect(listViewPaste,SIGNAL(triggered(bool)),this,SLOT(listViewPasteSlot(bool)));
    connect(listViewShowInExplorer,SIGNAL(triggered(bool)),this,SLOT(listViewShowInExplorerSlot(bool)));
    connect(listViewRename,SIGNAL(triggered(bool)),this,SLOT(listViewRenameSlot(bool)));
    connect(listViewDel,SIGNAL(triggered(bool)),this,SLOT(listViewDelSlot(bool)));
}

void MainWindow::initRootListMenu()
{
    rootListMenu=new QMenu(ui->listView);
    rootListRemove=new QAction(QIcon(":/icon/remove.png"),"移除",rootListMenu);

    rootListMenu->addAction(rootListRemove);

    connect(rootListRemove,SIGNAL(triggered(bool)),this,SLOT(rootListRemoveSlot(bool)));
}

void MainWindow::initTrayIconMenu()
{
    trayIconMenu=new QMenu(this);
    trayIconQuit=new QAction("退出",trayIconMenu);
    trayIcon->setContextMenu(trayIconMenu);         //设置系统托盘使用的菜单

    trayIconMenu->addAction(trayIconQuit);
    connect(trayIconQuit,SIGNAL(triggered(bool)),this,SLOT(trayIconQuitSlot(bool)));
}

void MainWindow::showTreeViewMenu(QPoint pos)
{
    treeViewMenuCoo=pos;

    QModelIndex index=ui->treeView->indexAt(pos);   //获取鼠标位置的当前项的index，此处pos为局部坐标
    const QMimeData* data=clipBoard->mimeData();

    if(data->hasUrls())                         //剪切板存在url数据，启用粘贴动作
        treeViewPaste->setEnabled(true);
    else
        treeViewPaste->setEnabled(false);       //系统剪切板的数据不是url数据，禁用粘贴动作

    if(index.isValid())                             //判断当前index是否有效
    {
       treeViewCopy->setEnabled(true);
       treeViewDel->setEnabled(true);
       treeViewRename->setEnabled(true);
    }
    else
    {
        treeViewCopy->setEnabled(false);
        treeViewDel->setEnabled(false);
        treeViewRename->setEnabled(false);
    }

    treeViewShowInExplorer->setEnabled(true);

    treeViewMenu->exec(QCursor::pos());         //QCursor::pos()为全局坐标，该函数应放在最后面
}

void MainWindow::showListViewMenu(QPoint pos)
{
    listViewMenuCoo=pos;

    QModelIndex index=((FMListView*)ui->tabWidget_2->currentWidget())->indexAt(pos);
    const QMimeData* data=clipBoard->mimeData();
    if(data->hasUrls())
        listViewPaste->setEnabled(true);
    else
        listViewPaste->setEnabled(false);

    if(index.isValid())
    {
        listViewCopy->setEnabled(true);
        listViewDel->setEnabled(true);
        listViewRename->setEnabled(true);
    }
    else
    {
        listViewCopy->setEnabled(false);
        listViewDel->setEnabled(false);
        listViewRename->setEnabled(false);
    }

    listViewShowInExplorer->setEnabled(true);

    listViewMenu->exec(QCursor::pos());
}

void MainWindow::showRootListMenu(QPoint pos)
{
    QModelIndex index=ui->listView->indexAt(pos);
    if(index.isValid())
    {
        rootListMenu->exec(QCursor::pos());
    }
}

void MainWindow::trayIconQuitSlot(bool checked)
{
    if(checked)
    {
        //TODO
    }

    if(fileSysCpThread->isRunning()||fileSysDelThread->isRunning())
    {
        QMessageBox::information(this,"提示","当前程序有任务正在进行，无法退出",QMessageBox::Ok);
    }
    else
        this->close();
}

void MainWindow::treeViewCopySlot(bool checked)
{
    if(checked)
    {
        //TODO
    }

    //获取视图中的QItemSelectionModel（被选中项模型），
    //并进一步获取第0列的QModelIndexList（被选中项的QModelIndex列表）
    QModelIndexList indexList=ui->treeView->selectionModel()->selectedRows(0);
    QStringList pathList;
    for(int i=0;i<indexList.count();i++)
    {
        //获得选中项的对应文件路径，并添加进pathList
        pathList.append(treeModel->filePath(indexList.at(i)));
    }

    fmCopy(pathList);
}

void MainWindow::listViewCopySlot(bool checked)
{
    if(checked)
    {
        //TODO
    }

    FMListView* listView=(FMListView*)ui->tabWidget_2->currentWidget();
    FMFileSystemModel* listModel=(FMFileSystemModel*)listView->model();

    QModelIndexList indexList=listView->selectionModel()->selectedIndexes();
    QStringList pathList;
    for(int i=0;i<indexList.count();i++)
    {
        //获得选中项的对应文件路径，并添加进pathList
        pathList.append(listModel->filePath(indexList.at(i)));
    }

    fmCopy(pathList);
}

void MainWindow::fmCopy(QStringList pathList)
{
    QList<QUrl> urls;
    for(int i=0;i<pathList.count();i++)
    {
        //获得文件路径的对应url格式，并添加进urls
        urls.append(QUrl::fromLocalFile(pathList.at(i)));
    }

    QMimeData* data=new QMimeData();
    data->setUrls(urls);
    clipBoard->setMimeData(data);
}

void MainWindow::treeViewDelSlot(bool checked)
{
    if(checked)
    {
        //TODO
    }

    QModelIndexList indexList=ui->treeView->selectionModel()->selectedRows(0);
    QStringList pathList;
    for(int i=0;i<indexList.count();i++)
    {
        //获得选中项的对应文件路径，并添加进pathList
        pathList.append(treeModel->filePath(indexList.at(i)));
    }

    fmDel(pathList);
}

void MainWindow::listViewDelSlot(bool checked)
{
    if(checked)
    {
        //TODO
    }

    FMListView* listView=(FMListView*)ui->tabWidget_2->currentWidget();
    FMFileSystemModel* listModel=(FMFileSystemModel*)listView->model();

    QModelIndexList indexList=listView->selectionModel()->selectedIndexes();
    QStringList pathList;
    for(int i=0;i<indexList.count();i++)
    {
        //获得选中项的对应文件路径，并添加进pathList
        pathList.append(listModel->filePath(indexList.at(i)));
    }

    fmDel(pathList);
}

void MainWindow::fmDel(QStringList pathList)
{
    fileSysDelThread->setPathList(pathList);
    fileSysDelThread->setProgressDialog(progressDialog);

    progressDialog->setProgressThreadType(2);

    fileSysDelThread->start();
    progressDialog->show();
}

void MainWindow::treeViewPasteSlot(bool checked)
{
    if(checked)
    {
        //TODO
    }

    //indexAt函数需要viewport的局部坐标，该坐标与请求上下文菜单事件发出的坐标类型一致，
    //这是QAbstractScrollArea及其子类独有特性
    //但是这里，不可直接采用下列语句，因为右键菜单栏弹出时的坐标与用户选择点击相应QAction时的坐标不一致
    //直接采用下列语句，容易出现 实际操作的项 与 用户希望操作的项 不一致
    //QModelIndex index=ui->treeView->indexAt(ui->treeView->viewport()->mapFromGlobal(QCursor::pos()));
    //改进方法为用QPoint变量在菜单弹出时就存放好坐标，如下所示treeViewMenuCoo
    QModelIndex index=ui->treeView->indexAt(treeViewMenuCoo);
    QString newDir;
    if(index.isValid())
    {
        newDir=treeModel->filePath(ui->treeView->currentIndex());
    }
    else
    {
        newDir=treeModel->filePath(ui->treeView->rootIndex());
    }

    fmPaste(newDir);
}

void MainWindow::listViewPasteSlot(bool checked)
{
    if(checked)
    {
        //TODO
    }

    FMListView* listView=(FMListView*)ui->tabWidget_2->currentWidget();
    FMFileSystemModel* listModel=(FMFileSystemModel*)listView->model();

    QModelIndex index=listView->indexAt(listViewMenuCoo);
    QString newDir;
    if(index.isValid())
    {
        newDir=listModel->filePath(listView->currentIndex());
    }
    else
    {
        newDir=listModel->filePath(listView->rootIndex());
    }

    fmPaste(newDir);
}

void MainWindow::fmPaste(QString newDir)
{
    QFileInfo fileInfo(newDir);
    if(fileInfo.isFile() || fileInfo.isSymLink() || (!fileInfo.exists()) )
    {
        //目的地目录无效，直接退出
        QMessageBox::warning(this,"警告",
                     QDir::toNativeSeparators(newDir)+"::不为有效目录",QMessageBox::Ok);
        return;
    }

    const QMimeData* data=clipBoard->mimeData();
    if(data->hasUrls())
    {
        QList<QUrl> urls=data->urls();
        QStringList pathList;
        for(int i=0;i<urls.count();i++)
        {
            //如果url指向本地文件，则添加进pathList
            if(urls.at(i).isLocalFile())
                pathList.append(urls.at(i).toLocalFile());
        }

        fileSysCpThread->setDestination(newDir);
        fileSysCpThread->setSource(pathList);
        fileSysCpThread->setProgressDialog(progressDialog);

        progressDialog->setProgressThreadType(1);

        fileSysCpThread->start();
        progressDialog->show();
    }
}

void MainWindow::treeViewShowInExplorerSlot(bool checked)
{
    if(checked)
    {
        //TODO
    }

    QModelIndex index=ui->treeView->indexAt(treeViewMenuCoo);
    QString path;
    if(index.isValid())
    {
        path=treeModel->filePath(ui->treeView->currentIndex());
    }
    else
    {
        path=treeModel->filePath(ui->treeView->rootIndex());
    }

    fmShowInExplorer(path);
}

void MainWindow::listViewShowInExplorerSlot(bool checked)
{
    if(checked)
    {
        //TODO
    }

    FMListView* listView=(FMListView*)ui->tabWidget_2->currentWidget();
    FMFileSystemModel* listModel=(FMFileSystemModel*)listView->model();

    QModelIndex index=listView->indexAt(listViewMenuCoo);
    QString path;
    if(index.isValid())
    {
        path=listModel->filePath(listView->currentIndex());
    }
    else
    {
        int curTabIndex=ui->tabWidget_2->currentIndex();
        path=ui->tabWidget_2->tabToolTip(curTabIndex);
    }

    fmShowInExplorer(path);
}

void MainWindow::fmShowInExplorer(QString path)
{
    QFileInfo fileInfo(path);
    //下列函数可以使用合适的程序打开文件，例如：对于目录，将使用资源管理器打开该目录
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
}

void MainWindow::treeViewRenameSlot(bool checked)
{
    if(checked)
    {
        //TODO
    }

    QString path=treeModel->filePath(ui->treeView->currentIndex());

    fmRename(path);
}

void MainWindow::listViewRenameSlot(bool checked)
{
    if(checked)
    {
        //TODO
    }

    FMListView* listView=(FMListView*)ui->tabWidget_2->currentWidget();
    FMFileSystemModel* listModel=(FMFileSystemModel*)listView->model();

    QString path=listModel->filePath(listView->currentIndex());
    fmRename(path);
}

void MainWindow::fmRename(QString path)
{
    QFileInfo fileInfo(path);
    if(fileInfo.isRoot())
    {
        QMessageBox::warning(this,"警告","不能重命名操作系统的根目录",QMessageBox::Ok);
        return;
    }
    QString fileType;
    if(fileInfo.isFile()||fileInfo.isSymLink())
    {
        fileType="文件";
    }
    else
    {
        fileType="目录";
    }

    bool ok;
    QString newName=QInputDialog::getText(this,"请输入新的名称（不包括后缀名）","原文件名："+fileInfo.fileName()
                                          +"\n"+"文件类型："+fileType,QLineEdit::Normal,"",&ok);
    if(ok)
    {
        QDir dir=fileInfo.absoluteDir();
        if(!dir.rename(fileInfo.fileName(),newName+"."+fileInfo.completeSuffix()))
        {
            QMessageBox::warning(this,"警告","重命名失败",QMessageBox::Ok);
        }
    }
}

void MainWindow::tabCloseRequestedSlot(int index)
{
    FMListView* listView=(FMListView*)ui->tabWidget_2->widget(index);
    ui->tabWidget_2->removeTab(index);  //从tabWidget_2删除序号为index的选项卡Tab，但Tab本身不会被删除
    delete listView;                    //删除选项卡本身
}

void MainWindow::progressInterrupted()
{
    int choice=QMessageBox::critical(this,"危险",
       "该操作将终止进度，线程可能在任何地方终止而无法释放相关资源或处理未完成的资源，点击“确认（OK）”将终止",QMessageBox::Ok,
       QMessageBox::Cancel);
    if(choice==QMessageBox::Ok)
    {
        if(progressDialog->getProgressThreadType()==1)
        {
            fileSysCpThread->terminate();
            fileSysCpThread->wait();
        }
        else if(progressDialog->getProgressThreadType()==2)
        {
            fileSysDelThread->terminate();
            fileSysDelThread->wait();
        }
        else
        {
            QMessageBox::warning(this,"警告","中断异常",QMessageBox::Ok);
        }
    }
}

void MainWindow::fileSysCpFinished()
{
    progressDialog->setTextBrowser("");
    progressDialog->hide();
}

void MainWindow::fileSysDelFinished()
{
    progressDialog->setTextBrowser("");
    progressDialog->hide();
}

int MainWindow::whetherToReplaceSlot(QString path)
{
    return QMessageBox::information(this,"提示",
        QDir::toNativeSeparators(path)+"::检测到同名，是否替换",QMessageBox::Ok,QMessageBox::Cancel);
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    QString path=treeModel->filePath(index);
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MainWindow::tabWidget2UpdateTab(QString path, int tabIndex)
{
    QFileInfo fileInfo(path);
    if(!fileInfo.exists())
    {
        return;
    }
    if(fileInfo.isFile()||fileInfo.isSymLink())
    {
        path=fileInfo.absolutePath();
        fileInfo=QFileInfo(path);
    }
    if(tabIndex<0)
    {
        //tabWidget_2无任何tab，直接新增一页并递归调用一次
        tabWidget2AddTab();
        tabWidget2UpdateTab(path,0);
        return;
    }
    if(fileInfo.isRoot())
    {
        ui->tabWidget_2->setTabText(tabIndex,QDir::toNativeSeparators(path));
    }
    else
    {
        ui->tabWidget_2->setTabText(tabIndex,fileInfo.fileName());
    }

    ui->tabWidget_2->setTabToolTip(tabIndex,QDir::toNativeSeparators(path));
    QFileIconProvider fileIconProvider;
    QIcon icon=fileIconProvider.icon(fileInfo);
    ui->tabWidget_2->setTabIcon(tabIndex,icon);

    FMListView* listView=(FMListView*)ui->tabWidget_2->currentWidget();
    FMFileSystemModel* listModel=(FMFileSystemModel*)listView->model();
    listView->setRootIndex(listModel->index(path));
}

void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    QStandardItem* item=rootListModel->itemFromIndex(index);
    QString rootDir=item->toolTip();
    ui->treeView->setRootIndex(treeModel->index(rootDir));
}

void MainWindow::rootListRemoveSlot(bool checked)
{
    if(checked)
    {
        //TODO
    }

    //QStandardItem* item=rootListModel->takeItem(ui->listView->currentIndex().row());
    //delete item;
    rootListModel->removeRow(ui->listView->currentIndex().row());
}

void MainWindow::on_actionShowHideTreeCol_1_triggered()
{
    static bool b=true;
    if(b)
    {
        ui->treeView->hideColumn(1);
        ui->actionShowHideTreeCol_1->setIcon(QIcon(":/icon/sizeColumn.png"));
        b=false;
    }
    else
    {
        ui->treeView->showColumn(1);
        ui->actionShowHideTreeCol_1->setIcon(QIcon(":/icon/sizeColumnGrey.png"));
        b=true;
    }
}

void MainWindow::on_actionShowHideTreeCol_2_triggered()
{
    static bool b=true;
    if(b)
    {
        ui->treeView->hideColumn(2);
        ui->actionShowHideTreeCol_2->setIcon(QIcon(":/icon/typeColumn.png"));
        b=false;
    }
    else
    {
        ui->treeView->showColumn(2);
        ui->actionShowHideTreeCol_2->setIcon(QIcon(":/icon/typeColumnGrey.png"));
        b=true;
    }
}

void MainWindow::on_actionShowHideTreeCol_3_triggered()
{
    static bool b=true;
    if(b)
    {
        ui->treeView->hideColumn(3);
        ui->actionShowHideTreeCol_3->setIcon(QIcon(":/icon/modifyTimeColumn.png"));
        b=false;
    }
    else
    {
        ui->treeView->showColumn(3);
        ui->actionShowHideTreeCol_3->setIcon(QIcon(":/icon/modifyTimeColumnGrey.png"));
        b=true;
    }
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if(reason==QSystemTrayIcon::Trigger)
    {
        this->showNormal();
    }
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    if(arg1==Qt::Unchecked)
    {
        setting->setValue("保存根目录列表",false);
    }
    else
    {
        setting->setValue("保存根目录列表",true);
    }
}
