#include "common/common_pch.h"

#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QRegularExpression>

#include "common/list_utils.h"
#include "common/qt.h"
#include "mkvtoolnix-gui/app.h"
#include "mkvtoolnix-gui/main_window/main_window.h"
#include "mkvtoolnix-gui/jobs/model.h"
#include "mkvtoolnix-gui/jobs/program_runner.h"
#if defined(SYS_WINDOWS)
#include "mkvtoolnix-gui/jobs/program_runner/windows_program_runner.h"
#endif // SYS_WINDOWS
#include "mkvtoolnix-gui/jobs/tool.h"
#include "mkvtoolnix-gui/util/message_box.h"

namespace mtx { namespace gui { namespace Jobs {

ProgramRunner::ProgramRunner()
  : QObject{}
{
}

ProgramRunner::~ProgramRunner() {
}

void
ProgramRunner::enableActionToExecute(Util::Settings::RunProgramConfig &config,
                                     ExecuteActionCondition condition,
                                     bool enable) {
  if (enable)
    m_actionsToExecute[condition] << &config;
  else
    m_actionsToExecute[condition].remove(&config);
}

bool
ProgramRunner::isActionToExecuteEnabled(Util::Settings::RunProgramConfig &config,
                                        ExecuteActionCondition condition) {
  return m_actionsToExecute[condition].contains(&config);
}

void
ProgramRunner::setup() {
  connect(MainWindow::jobTool()->model(), &Jobs::Model::queueStatusChanged, this, &ProgramRunner::executeActionsAfterQueueFinishes);
}

void
ProgramRunner::executeActionsAfterJobFinishes(Job const &job) {
  executeActions(ExecuteActionCondition::AfterJobFinishes, &job);
}

void
ProgramRunner::executeActionsAfterQueueFinishes(QueueStatus status) {
  if (Jobs::QueueStatus::Stopped == status)
    executeActions(ExecuteActionCondition::AfterQueueFinishes);
}

void
ProgramRunner::executeActions(ExecuteActionCondition condition,
                              Job const *job) {
  for (auto const &config : Util::Settings::get().m_runProgramConfigurations)
    if (isActionToExecuteEnabled(*config, condition)) {
      // The event doesn't really matter as we're forcing a specific configuration to run.
      run(Util::Settings::RunAfterJobQueueFinishes, [job](VariableMap &variables) {
        if (job)
          job->runProgramSetupVariables(variables);
      }, config);
    }

  m_actionsToExecute[condition].clear();
}

void
ProgramRunner::run(Util::Settings::RunProgramForEvent forEvent,
                   std::function<void(VariableMap &)> const &setupVariables,
                   Util::Settings::RunProgramConfigPtr const &forceRunThis) {
  auto &cfg             = Util::Settings::get();
  auto generalVariables = VariableMap{};
  auto configsToRun     = forceRunThis ? Util::Settings::RunProgramConfigList{forceRunThis}
                        :                cfg.m_runProgramConfigurations;

  setupGeneralVariables(generalVariables);

  for (auto const &runConfig : configsToRun) {
    if (!(runConfig->m_active && (runConfig->m_forEvents & forEvent)) && !forceRunThis)
      continue;

    if (runConfig->m_type == Util::Settings::RunProgramType::ExecuteProgram)
      executeProgram(*runConfig, setupVariables, generalVariables);

    else if (runConfig->m_type == Util::Settings::RunProgramType::PlayAudioFile)
      playAudioFile(*runConfig);

    else if (runConfig->m_type == Util::Settings::RunProgramType::ShutDownComputer)
      shutDownComputer(*runConfig);

    else if (runConfig->m_type == Util::Settings::RunProgramType::SuspendComputer)
      suspendComputer(*runConfig);
  }
}

QStringList
ProgramRunner::replaceVariables(QStringList const &commandLine,
                                VariableMap const &variables) {
  auto variableNameRE = QRegularExpression{Q("<MTX_[A-Z0-9_]+>")};
  auto newCommandLine = QStringList{};

  for (auto const &argument: commandLine) {
    auto replacedFully = false;
    auto newArgument   = argument;

    for (auto const &variable : variables.keys()) {
      auto placeholder = Q("<MTX_%1>").arg(variable);

      if (argument == placeholder) {
        newCommandLine += variables[variable];
        replacedFully = true;
        break;

      } else
        newArgument.replace(placeholder, variables[variable].join(Q(' ')));
    }

    if (replacedFully)
      continue;

    newArgument.replace(variableNameRE, Q(""));
    newCommandLine << newArgument;
  }

  return newCommandLine;
}

void
ProgramRunner::setupGeneralVariables(QMap<QString, QStringList> &variables) {
  variables[Q("CURRENT_TIME")] << QDateTime::currentDateTime().toString(Qt::ISODate);
  variables[Q("INSTALLATION_DIRECTORY")] << QDir::toNativeSeparators(App::applicationDirPath());
}

std::unique_ptr<ProgramRunner>
ProgramRunner::create() {
  std::unique_ptr<ProgramRunner> runner;

#if defined(SYS_WINDOWS)
  runner.reset(new WindowsProgramRunner{});
#endif // SYS_WINDOWS

  if (!runner)
    runner.reset(new ProgramRunner{});

  runner->setup();

  return runner;
}

bool
ProgramRunner::isRunProgramTypeSupported(Util::Settings::RunProgramType type) {
  return mtx::included_in(type, Util::Settings::RunProgramType::ExecuteProgram);
}

void
ProgramRunner::executeProgram(Util::Settings::RunProgramConfig &config,
                              std::function<void(VariableMap &)> const &setupVariables,
                              VariableMap const &generalVariables) {
  auto commandLine = config.m_commandLine;
  auto variables   = generalVariables;

  setupVariables(variables);

  commandLine = replaceVariables(commandLine, variables);
  auto exe    = commandLine.value(0);

  if (exe.isEmpty())
    return;

  commandLine.removeFirst();

  if (QProcess::startDetached(exe, commandLine))
    return;

  Util::MessageBox::critical(MainWindow::get())
    ->title(QY("Program execution failed"))
    .text(Q("%1\n%2")
          .arg(QY("The following program could not be executed: %1").arg(exe))
          .arg(QY("Possible causes are that the program does not exist or that you're not allowed to access it or its directory.")))
    .exec();
}

void
ProgramRunner::playAudioFile(Util::Settings::RunProgramConfig &/* config */) {
  // Not supported yet.
}

void
ProgramRunner::shutDownComputer(Util::Settings::RunProgramConfig &/* config */) {
  // Not supported in an OS-agnostic way.
}

void
ProgramRunner::suspendComputer(Util::Settings::RunProgramConfig &/* config */) {
  // Not supported in an OS-agnostic way.
}

}}}
