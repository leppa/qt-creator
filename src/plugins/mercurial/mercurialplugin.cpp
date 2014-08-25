#include <vcsbase/vcsbaseconstants.h>
#include <vcsbase/vcsoutputwindow.h>
#ifdef WITH_TESTS
#include <QTest>
#endif

namespace Mercurial {
namespace Internal {

    VcsBaseSubmitEditorParameters::DiffFiles
    const auto widgetCreator = []() { return new MercurialEditorWidget; };
        addAutoReleasedObject(new VcsEditorFactory(editorParameters + i, widgetCreator, m_client, describeSlot));
    addAutoReleasedObject(new VcsSubmitEditorFactory(&submitEditorParameters,
        []() { return new CommitEditor(&submitEditorParameters); }));
    auto cloneWizardFactory = new BaseCheckoutWizardFactory;
    cloneWizardFactory->setId(QLatin1String(VcsBase::Constants::VCS_ID_MERCURIAL));
    cloneWizardFactory->setIcon(QIcon(QLatin1String(":/mercurial/images/hg.png")));
    cloneWizardFactory->setDescription(tr("Clones a Mercurial repository and tries to load the contained project."));
    cloneWizardFactory->setDisplayName(tr("Mercurial Clone"));
    cloneWizardFactory->setWizardCreator([this] (const FileName &path, QWidget *parent) {
        return new CloneWizard(path, parent);
    });
    addAutoReleasedObject(cloneWizardFactory);
    connect(m_client, SIGNAL(parsedStatus(QList<VcsBaseClient::StatusItem>)),
            this, SLOT(showCommitWidget(QList<VcsBaseClient::StatusItem>)));
    disconnect(m_client, SIGNAL(parsedStatus(QList<VcsBaseClient::StatusItem>)),
               this, SLOT(showCommitWidget(QList<VcsBaseClient::StatusItem>)));
        VcsOutputWindow::appendError(tr("There are no changes to commit."));
        VcsOutputWindow::appendError(saver.errorString());
        VcsOutputWindow::appendError(tr("Unable to create an editor for the commit."));
    QTC_ASSERT(submitEditor(), return);
    Core::EditorManager::closeDocument(submitEditor()->document());
    VcsBaseEditorWidget::testDiffFileResolving(editorParameters[2].id);
    VcsBaseEditorWidget::testLogResolving(editorParameters[0].id, data, "18473:692cbda1eb50", "18472:37100f30590f");
} // namespace Internal
} // namespace Mercurial