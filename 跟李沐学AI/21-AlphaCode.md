# AlphaCode

## 摘要

- 标题：Competition-Level Code Generation with AlphaCode
- 在OpenAI的基础上支持了更长的文档和生成更长的代码

## 导言

- 数据集涉及多语言，比Codex大了5倍，同时微调数据集也更大
- 基于标准的Encoder-decoder transformer
  - 编码器是解码器的4倍，因为输出一般比较长
  - Attention中KV是共享矩阵参数同时小于Query的矩阵大小
  - 非对称，编码器的输入序列长度2倍于解码器
- 微调
  - 温度：高则分布靠近，低则分布远离，这里使用0.2让分布更加sharper
  - 保留不正确的解也是好的，增加两个判别方法：conditioning和prediction
    - conditioning：在输入问题前加上correct solution或者false solution
    - 预测
  - GOLD