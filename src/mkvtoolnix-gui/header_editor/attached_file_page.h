#ifndef MTX_MKVTOOLNIX_GUI_HEADER_EDITOR_ATTACHED_FILE_PAGE_H
#define MTX_MKVTOOLNIX_GUI_HEADER_EDITOR_ATTACHED_FILE_PAGE_H

#include "common/common_pch.h"

#include "mkvtoolnix-gui/header_editor/page_base.h"
#include "mkvtoolnix-gui/util/files_drag_drop_handler.h"

namespace libmatroska {
class KaxAttached;
};

namespace mtx { namespace gui { namespace HeaderEditor {

namespace Ui {
class AttachedFilePage;
}

using KaxAttachedPtr = std::shared_ptr<KaxAttached>;

class AttachedFilePage: public PageBase {
  Q_OBJECT;

public:
  std::unique_ptr<Ui::AttachedFilePage> ui;
  mtx::gui::Util::FilesDragDropHandler m_filesDDHandler;
  PageBase &m_topLevelPage;
  KaxAttachedPtr m_attachment;
  memory_cptr m_newFileContent;

public:
  AttachedFilePage(Tab &parent, PageBase &topLevelPage, KaxAttachedPtr const &attachment);
  virtual ~AttachedFilePage();

  virtual void init();

  virtual void retranslateUi() override;

  virtual QString title() const override;
  virtual void setItems(QList<QStandardItem *> const &items) const override;

  virtual bool hasThisBeenModified() const;
  virtual void modifyThis();
  virtual bool validateThis() const;

  virtual void setControlsFromAttachment();
  virtual void saveContent();
  virtual void replaceContent(bool deriveNameAndMimeType);

signals:
  void filesDropped(QStringList const &fileNames);

protected:
  virtual void dragEnterEvent(QDragEnterEvent *event) override;
  virtual void dropEvent(QDropEvent *event) override;

  virtual QString formatSize() const;
};

}}}

#endif  // MTX_MKVTOOLNIX_GUI_HEADER_EDITOR_ATTACHED_FILE_PAGE_H
