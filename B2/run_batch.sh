#!/bin/bash

# 1. 定义所有需要模拟的能量点（数组形式，可灵活增删）
ENERGIES=(20 40 80 100 150 200 250 300)
# 2. Geant4 程序名称（替换为你的实际程序名，如 ./your_G4_app）
G4_APP="./your_geant4_application"
# 3. 循环遍历所有能量点
for E in "${ENERGIES[@]}"
do
    echo "===== 开始模拟能量点：${E} GeV ====="
    # 调用 Geant4 程序，通过 < 传入通用宏文件，同时动态设置能量和输出文件名
    ${G4_APP} << EOF
/gun/energy ${E} GeV
/analysis/setFileName PhotonData_E${E}GeV
/control/execute single_energy.mac
EOF
    echo "===== 能量点 ${E} GeV 模拟完成 ====="
done

echo "所有能量点模拟结束！"
