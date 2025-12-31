/// \file B2/src/ActionInitialization.cc
/// \brief Implementation of the B2::ActionInitialization class

// ActionInitialization.cc：动作初始化管理器（总指挥）

#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "SteppingAction.hh"

namespace B2
{

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::BuildForMaster() const
{
  auto runAction = new RunAction;
  SetUserAction(runAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ActionInitialization::Build() const
{
  // 1. 创建粒子产生器动作并注册
  SetUserAction(new PrimaryGeneratorAction);

  // 2. 创建运行动作并注册（用于数据文件初始化、统计汇总）
  auto runAction = new RunAction;
  SetUserAction(runAction);

  // 3. 创建事例动作并注册（用于单个事例的信号累加）
  auto eventAction = new EventAction(runAction);
  SetUserAction(eventAction);

  // 4. 创建步进动作并注册（用于每一步的处理，如能量沉积记录）
  SetUserAction(new SteppingAction(eventAction));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

}
