# RT-Thread 三按键三LED实验

## 分工
- A：GPIO、LED/按键底层、事件对象、线程创建与 Keil 工程集成。
- B：模式状态机、按键消抖、三个线程具体功能、FinSH 仿真与逻辑测试。

## 分支
- main：稳定版本，只通过 Pull Request 合并。
- driver-main：A 的开发分支。
- thread-function：B 的开发分支。

## 验收
1. 至少创建 key_scan、mode_ctrl、led_effect 三个独立线程并设置不同优先级。
2. 使用 RT-Thread 事件集在线程间传递按键事件和 LED 命令。
3. 第三按键第一次全亮、第二次进入流水准备、第三次全灭并重新计数。
4. 流水准备状态下，按键1正向流水，按键2反向流水。
5. 无实物时可在 FinSH 输入 key_sim 1、key_sim 2、key_sim 3 验证。
