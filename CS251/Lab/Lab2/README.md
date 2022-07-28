# 记录

IOU: I owe you

## Contract

- 一开始采用的数据结构是`map(address => struct(address, uint32))`，以为`solidity`不能map套map

- 提供的baseline中找circle是在前端js实现的，因此contract里面`add_IOU`需要直接提供circle以及最小值

## Client

- js的遍历不推荐使用`for in`

- 一开始想将contract中的map改成public，然后直接调用getter函数，但是发现只能提供key得到value而不能直接获得这个map。Baseline中提供了`getAllFunctionCalls`函数来得到所有区块中涉及某种函数的交易信息（包括调用者、参数以及时间戳）

- 注意大小写和数据类型（字符串or整数）

- 注意解决circle的逻辑：A欠B钱，应该从B开始找B欠谁，一直欠到A，然后再解决这个circle