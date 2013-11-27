#include "CPUDisassembly.h"

CPUDisassembly::CPUDisassembly(QWidget *parent) : Disassembly(parent)
{
    // Create the action list for the right click context menu
    setupRightClickContextMenu();

}


/************************************************************************************
                            Mouse Management
************************************************************************************/
/**
 * @brief       This method has been reimplemented. It manages the richt click context menu.
 *
 * @param[in]   event       Context menu event
 *
 * @return      Nothing.
 */
void CPUDisassembly::contextMenuEvent(QContextMenuEvent* event)
{
    if(getSize() != 0)
    {
        int wI;
        QMenu* wMenu = new QMenu(this);
        uint_t wVA = rvaToVa(getInitialSelection());
        BPXTYPE wBpType = DbgGetBpxTypeAt(wVA);

        // Build Menu
        wMenu->addAction(mSetLabel);
        wMenu->addAction(mSetComment);
        wMenu->addAction(mSetBookmark);

        // BP Menu
        mBPMenu->clear();

        // Soft BP
        mBPMenu->addAction(mToggleInt3BpAction);


        // Hardware BP
        if((wBpType & bp_hardware) == bp_hardware)
        {
            mBPMenu->addAction(mClearHwBpAction);
        }
        else
        {
            BPMAP wBPList;
            DbgGetBpList(bp_hardware, &wBPList);

            if(wBPList.count < 4)
            {
                mBPMenu->addAction(mSetHwBpAction);
            }
            else
            {
                REGDUMP wRegDump;
                DbgGetRegDump(&wRegDump);

                for(wI = 0; wI < 4; wI++)
                {
                    switch(wBPList.bp[wI].slot)
                    {
                        case 0:
                            msetHwBPOnSlot0Action->setText("Replace Slot 0 (0x" + QString("%1").arg(wBPList.bp[wI].addr, 8, 16, QChar('0')).toUpper() + ")");
                            break;
                        case 1:
                            msetHwBPOnSlot1Action->setText("Replace Slot 1 (0x" + QString("%1").arg(wBPList.bp[wI].addr, 8, 16, QChar('0')).toUpper() + ")");
                            break;
                        case 2:
                            msetHwBPOnSlot2Action->setText("Replace Slot 2 (0x" + QString("%1").arg(wBPList.bp[wI].addr, 8, 16, QChar('0')).toUpper() + ")");
                            break;
                        case 3:
                            msetHwBPOnSlot3Action->setText("Replace Slot 3 (0x" + QString("%1").arg(wBPList.bp[wI].addr, 8, 16, QChar('0')).toUpper() + ")");
                            break;
                        default:
                            break;
                    }
                }

                mHwSlotSelectMenu->addAction(msetHwBPOnSlot0Action);
                mHwSlotSelectMenu->addAction(msetHwBPOnSlot1Action);
                mHwSlotSelectMenu->addAction(msetHwBPOnSlot2Action);
                mHwSlotSelectMenu->addAction(msetHwBPOnSlot3Action);
                mBPMenu->addMenu(mHwSlotSelectMenu);
            }

        }
        wMenu->addMenu(mBPMenu);

        // Separator
        wMenu->addSeparator();

        // Goto Menu
        mGotoMenu->addAction(mGotoOrigin);
        mGotoMenu->addAction(mSetNewOriginHere);
        wMenu->addMenu(mGotoMenu);


        QAction* wAction = wMenu->exec(event->globalPos());
    }
}


/************************************************************************************
                         Context Menu Management
************************************************************************************/
void CPUDisassembly::setupRightClickContextMenu()
{
    // Labels
    mSetLabel = new QAction("Label", this);
    mSetLabel->setShortcutContext(Qt::WidgetShortcut);
    mSetLabel->setShortcut(QKeySequence(":"));
    this->addAction(mSetLabel);
    connect(mSetLabel, SIGNAL(triggered()), this, SLOT(setLabel()));

    // Comments
    mSetComment = new QAction("Comment", this);
    mSetComment->setShortcutContext(Qt::WidgetShortcut);
    mSetComment->setShortcut(QKeySequence(";"));
    this->addAction(mSetComment);
    connect(mSetComment, SIGNAL(triggered()), this, SLOT(setComment()));

    // Bookmarks
    mSetBookmark = new QAction("Bookmark", this);
    mSetBookmark->setShortcutContext(Qt::WidgetShortcut);
    mSetBookmark->setShortcut(QKeySequence("ctrl+d"));
    this->addAction(mSetBookmark);
    connect(mSetBookmark, SIGNAL(triggered()), this, SLOT(setBookmark()));

    //---------------------- Go to -----------------------------------
    // Menu
    mGotoMenu = new QMenu("Go to", this);

    // Origin action
    mGotoOrigin = new QAction("Origin", this);
    mGotoOrigin->setShortcutContext(Qt::WidgetShortcut);
    mGotoOrigin->setShortcut(QKeySequence("*"));
    this->addAction(mGotoOrigin);
    connect(mGotoOrigin, SIGNAL(triggered()), this, SLOT(gotoOrigin()));

    //---------------------- Breakpoints -----------------------------
    // Menu
    mBPMenu = new QMenu("Breakpoint", this);

    // Standard breakpoint (option set using SetBPXOption)
    mToggleInt3BpAction = new QAction("Toggle", this);
    mToggleInt3BpAction->setShortcutContext(Qt::WidgetShortcut);
    mToggleInt3BpAction->setShortcut(QKeySequence(Qt::Key_F2));
    this->addAction(mToggleInt3BpAction);
    connect(mToggleInt3BpAction, SIGNAL(triggered()), this, SLOT(toggleInt3BPAction()));

    // HW BP
    mHwSlotSelectMenu = new QMenu("Set Hardware on Execution", this);

    mSetHwBpAction = new QAction("Set Hardware on Execution", this);
    connect(mSetHwBpAction, SIGNAL(triggered()), this, SLOT(toggleHwBpActionSlot()));

    mClearHwBpAction = new QAction("Remove Hardware", this);
    connect(mClearHwBpAction, SIGNAL(triggered()), this, SLOT(toggleHwBpActionSlot()));

    msetHwBPOnSlot0Action = new QAction("Set Hardware on Execution on Slot 0 (Free)", this);
    connect(msetHwBPOnSlot0Action, SIGNAL(triggered()), this, SLOT(setHwBpOnSlot0ActionSlot()));

    msetHwBPOnSlot1Action = new QAction("Set Hardware on Execution on Slot 1 (Free)", this);
    connect(msetHwBPOnSlot1Action, SIGNAL(triggered()), this, SLOT(setHwBpOnSlot1ActionSlot()));

    msetHwBPOnSlot2Action = new QAction("Set Hardware on Execution on Slot 2 (Free)", this);
    connect(msetHwBPOnSlot2Action, SIGNAL(triggered()), this, SLOT(setHwBpOnSlot2ActionSlot()));

    msetHwBPOnSlot3Action = new QAction("Set Hardware on Execution on Slot 3 (Free)", this);
    connect(msetHwBPOnSlot3Action, SIGNAL(triggered()), this, SLOT(setHwBpOnSlot3ActionSlot()));

    //---------------------- New origin here -----------------------------
    mSetNewOriginHere = new QAction("Set New Origin Here", this);
    mSetNewOriginHere->setShortcutContext(Qt::WidgetShortcut);
    mSetNewOriginHere->setShortcut(QKeySequence("ctrl+*"));
    this->addAction(mSetNewOriginHere);
    connect(mSetNewOriginHere, SIGNAL(triggered()), this, SLOT(setNewOriginHereActionSlot()));
}

void CPUDisassembly::gotoOrigin()
{
    Bridge::getBridge()->execCmd("d cip");
}


void CPUDisassembly::toggleInt3BPAction()
{
    uint_t wVA = rvaToVa(getInitialSelection());
    BPXTYPE wBpType = DbgGetBpxTypeAt(wVA);
    QString wCmd;

    if((wBpType & bp_normal) == bp_normal)
    {
        wCmd = "bc " + QString("%1").arg(wVA, sizeof(int_t) * 2, 16, QChar('0')).toUpper();
    }
    else
    {
        wCmd = "bp " + QString("%1").arg(wVA, sizeof(int_t) * 2, 16, QChar('0')).toUpper();
    }

    Bridge::getBridge()->execCmd(wCmd.toUtf8().constData());
}


void CPUDisassembly::toggleHwBpActionSlot()
{
    uint_t wVA = rvaToVa(getInitialSelection());
    BPXTYPE wBpType = DbgGetBpxTypeAt(wVA);
    QString wCmd;

    if((wBpType & bp_hardware) == bp_hardware)
    {
        wCmd = "bphwc " + QString("%1").arg(wVA, sizeof(int_t) * 2, 16, QChar('0')).toUpper();
    }
    else
    {
        wCmd = "bphws " + QString("%1").arg(wVA, sizeof(int_t) * 2, 16, QChar('0')).toUpper();
    }

    Bridge::getBridge()->execCmd(wCmd.toUtf8().constData());
}


void CPUDisassembly::setHwBpOnSlot0ActionSlot()
{
    setHwBpAt(rvaToVa(getInitialSelection()), 0);
}

void CPUDisassembly::setHwBpOnSlot1ActionSlot()
{
    setHwBpAt(rvaToVa(getInitialSelection()), 1);
}

void CPUDisassembly::setHwBpOnSlot2ActionSlot()
{
    setHwBpAt(rvaToVa(getInitialSelection()), 2);
}

void CPUDisassembly::setHwBpOnSlot3ActionSlot()
{
    setHwBpAt(rvaToVa(getInitialSelection()), 3);
}

void CPUDisassembly::setHwBpAt(uint_t va, int slot)
{
    BPXTYPE wBpType = DbgGetBpxTypeAt(va);

    if((wBpType & bp_hardware) == bp_hardware)
    {
        mBPMenu->addAction(mClearHwBpAction);
    }


    int wI = 0;
    int wSlotIndex = -1;
    BPMAP wBPList;
    QString wCmd = "";

    DbgGetBpList(bp_hardware, &wBPList);

    // Find index of slot slot in the list
    for(wI = 0; wI < wBPList.count; wI++)
    {
        if(wBPList.bp[wI].slot == (unsigned short)slot)
        {
            wSlotIndex = wI;
            break;
        }
    }

    if(wSlotIndex < 0) // Slot not used
    {
        wCmd = "bphws " + QString("%1").arg(va, sizeof(int_t) * 2, 16, QChar('0')).toUpper();
        Bridge::getBridge()->execCmd(wCmd.toUtf8().constData());
    }
    else // Slot used
    {
        wCmd = "bphwc " + QString("%1").arg((uint_t)(wBPList.bp[wSlotIndex].addr), sizeof(uint_t) * 2, 16, QChar('0')).toUpper();
        Bridge::getBridge()->execCmd(wCmd.toUtf8().constData());

        Sleep(200);

        wCmd = "bphws " + QString("%1").arg(va, sizeof(int_t) * 2, 16, QChar('0')).toUpper();
        Bridge::getBridge()->execCmd(wCmd.toUtf8().constData());
    }
}

void CPUDisassembly::setNewOriginHereActionSlot()
{
    uint_t wVA = rvaToVa(getInitialSelection());
    QString wCmd = "cip=" + QString("%1").arg(wVA, sizeof(int_t) * 2, 16, QChar('0')).toUpper();
    Bridge::getBridge()->execCmd(wCmd.toUtf8().constData());
}

void CPUDisassembly::setLabel()
{
    uint_t wVA = rvaToVa(getInitialSelection());
    LineEditDialog mLineEdit(this);
    QString addr_text=QString("%1").arg(wVA, sizeof(int_t) * 2, 16, QChar('0')).toUpper();
    char label_text[MAX_COMMENT_SIZE]="";
    if(DbgGetLabelAt((duint)wVA, SEG_DEFAULT, label_text))
        mLineEdit.setText(QString(label_text));
    mLineEdit.setWindowTitle("Add label at " + addr_text);
    if(mLineEdit.exec()!=QDialog::Accepted)
        return;
    if(!DbgSetLabelAt(wVA, mLineEdit.editText.toUtf8().constData()))
    {
        QMessageBox msg(QMessageBox::Critical, "Error!", "DbgSetLabelAt failed!");
        msg.setWindowIcon(QIcon(":/icons/images/compile-error.png"));
        msg.exec();
    }
}

void CPUDisassembly::setComment()
{
    uint_t wVA = rvaToVa(getInitialSelection());
    LineEditDialog mLineEdit(this);
    QString addr_text=QString("%1").arg(wVA, sizeof(int_t) * 2, 16, QChar('0')).toUpper();
    char comment_text[MAX_COMMENT_SIZE]="";
    if(DbgGetCommentAt((duint)wVA, comment_text))
        mLineEdit.setText(QString(comment_text));
    mLineEdit.setWindowTitle("Add comment at " + addr_text);
    if(mLineEdit.exec()!=QDialog::Accepted)
        return;
    if(!DbgSetCommentAt(wVA, mLineEdit.editText.toUtf8().constData()))
    {
        QMessageBox msg(QMessageBox::Critical, "Error!", "DbgSetCommentAt failed!");
        msg.setWindowIcon(QIcon(":/icons/images/compile-error.png"));
        msg.exec();
    }
}

void CPUDisassembly::setBookmark()
{
    uint_t wVA = rvaToVa(getInitialSelection());
    bool result;
    if(DbgGetBookmarkAt(wVA))
        result=DbgSetBookmarkAt(wVA, false);
    else
        result=DbgSetBookmarkAt(wVA, true);
    if(!result)
    {
        QMessageBox msg(QMessageBox::Critical, "Error!", "DbgSetBookmarkAt failed!");
        msg.setWindowIcon(QIcon(":/icons/images/compile-error.png"));
        msg.exec();
    }
}
