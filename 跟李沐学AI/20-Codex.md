# Codex

## 标题

- Evaluating Large Language Models Trained on Code

## 摘要

- 基于GPT，使用GitHub微调

- 将Python的docstrings（注释）翻译成code，作者自己实现了一个数据集HumanEval用来判断Code的正确性
- 使用sampling：即生成大量的samples然后看哪个能过，类似图片分类中的top five

## 方法

- 核采样，将预测的当前词按概念从大到小依次留下来，直到所有的词的概率达到95%
- 训练集也进行了额外的收集，为了和测试集的输入类似，相当于增加一个带标准答案的数据集

- 考虑一个新的任务：从代码生成文档或函数名等，BERT可以但是GPT是前向的，所以他们重新做了一个训练数据集，这个数据集里注释在最后

## 局限性

- Codex训练不efficient
- prompt的使用方法，当docstring很长的时候代码水平会打折

## 主要内容

- 基于GPT，使用GitHub数据微调，然后设计了164个问题来test，另外自己做了一些标准答案来提高准确率