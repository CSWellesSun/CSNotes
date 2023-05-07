# AnthropicLLM

## 标题

- Training a Helpful and Harmless Assistant with Reinforcement Learning from Human Feedback
- Claude的基础

## 引言

- 模型参数比较小的时候RLHF会导致精度下降，但是模型大的时候就不会
- 提出一个多轮对话数据集
- 使用OOD（out of domain）来拒绝奇怪或者有害的输入
- 相关工作
  - LaMDA（Bard）：只有监督学习
  - InstructGPT（ChatGPT）：使用了监督学习而Anthropic全部RL（还用到了context distillation，但是这个技术相当于prompt engineering），同时Anthropic使用最大模型作为RM模型，另外Anthropic关注harmlessness

## 数据收集

- 多轮对话，主要是让人随便提问然后在两个模型输出里选哪个更好
- 发现标注结果的agreement比较低
- 最后标注了两个数据集：Helpfulness和Harmlessness
  - 前者是让标注工选择更有帮助的response，后者是让标注工选择更harmful的response，这导致了模型分裂，后面有工作去解决这个问题

## 方法

- 模型：有点类似InstructGPT
  - 一开始的初始模型，没有微调过，比较原始
  - 奖励模型，使用$k=16$作为上面模型输出的个数
  - RLHF微调后的模型
- 奖励模型：分成三个阶段
  - Language Model pre-training
  - PMP (Preference Model Pretraining)：在网络上搜集comparison data（例如点赞等）来训练，学习率是0.1，数据量较大
  - 使用Human Feedback来微调：数据量较小，学习率为0.01
  - 以上窗口是1024 token，之后在线模型使用2048 token
- RLHF和InstructGPT差不多，也使用PPO，但是发现PPO的惩罚性其实对结果影响可能并不大
- 奖励模型 PM 的稳定性问题
  - 当两个答案都比较好的时候就会出现奖励模型不准的情况
  - 将数据集分成两半并在每一块上分别训练一个模型，一个是train PM，一个是test PM。然后用train PM去调模型，然后看模型是否在test PM上也能得到更高的分数，如果没有更多提升就表示过度优化了
  - 发现train PM的大小大的时候，在test PM上得到的分数是随着模型RLHF微调依旧不断上升的
- 作者认为 $\sqrt{D_{KL}}$ 和 Reward（test PM上的分数） 有线性关系（很玄学），作者说可以通过这样计算出想达到什么样的效果需要多少的数据量

