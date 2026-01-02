#!/bin/bash

# 1. 定义需要模拟的能量点 (GeV)
ENERGIES=(20 40 80 100 150 200 250 300)

# 2. 定义程序名称
G4_APP="./B2"

# 3. 循环遍历能量点
for E in "${ENERGIES[@]}"
do
    echo "============================================="
    echo "正在模拟能量点: ${E} GeV"
    
    # 定义当前能量点的输出文件名
    OUT_NAME="PhotonData_E${E}GeV"

    # 4. 在脚本内直接生成临时宏文件 temp_run.mac
    # 这里的内容就是你原来宏文件里的指令
    cat > temp_run.mac << EOF
/run/initialize

# 设置物理量
/gun/energy ${E} GeV

# 设置分析输出
/analysis/setFileName ${OUT_NAME}
/analysis/openFile

# 启动模拟 (这里设置模拟的粒子数)
/run/beamOn 1000

# 保存并关闭
/analysis/write
/analysis/closeFile

# 退出程序
exit
EOF

    # 5. 运行程序，并带上刚生成的宏文件作为参数
    # 这样运行等同于 ./B2 temp_run.mac，会直接进入批处理模式，不会开可视化
    ${G4_APP} temp_run.mac > "log_E${E}.txt" 2>&1

    echo "能量点 ${E} GeV 模拟完成！结果已保存至 ${OUT_NAME}.root"
done

# 6. 任务结束后删除临时宏文件
rm temp_run.mac
echo "============================================="
echo "所有能量点模拟任务全部完成！"